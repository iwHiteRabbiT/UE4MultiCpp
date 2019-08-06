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
	int32 TicksProcessed;

	UFUNCTION()
	void SERVER_OnTickPowerUp();

	UPROPERTY(ReplicatedUsing = OnRep_PowerUpActive)
	bool bIsPowerUpActive;

	UFUNCTION()
	void OnRep_PowerUpActive();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnPowerUpStateChange(bool bIsActive);

public:	

	void SERVER_ActivatePowerUp();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnPowerUpTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnExpired();
};
