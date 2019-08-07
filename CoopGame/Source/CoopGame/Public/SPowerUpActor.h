// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerUpActor.generated.h"

using namespace UP;
using namespace UF;

UCLASS()
class COOPGAME_API ASPowerUpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerUpActor();

protected:
	/* Time between power ticks */
	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
	float PowerUpInterval;

	/* Total times we apply the PowerUp */
	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
	int32 TotalNbOfTicks;

	FTimerHandle TimerHandle_PowerUpTick;

	// Total number of ticks applied
	UPROPERTY(ReplicatedUsing = OnRep_PowerUpTicked)
	int32 RU_TicksProcessed;

	UFUNCTION()
	void OnRep_PowerUpTicked();

	UFUNCTION()
	void SERVER_OnTickPowerUp();

	UPROPERTY(ReplicatedUsing = OnRep_PowerUpActive)
	bool RU_bIsPowerUpActive;

	UFUNCTION()
	void OnRep_PowerUpActive();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnPowerUpStateChange(bool bIsActive);

	UPROPERTY(BlueprintReadOnly, Replicated)
	AActor* R_ActivatedTargetActor;

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnPowerUpTicked();

public:	

	void SERVER_ActivatePowerUp(AActor* ActivateFor);
	
	// UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	// void OnActivated();

	// UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	// void OnExpired();
};
