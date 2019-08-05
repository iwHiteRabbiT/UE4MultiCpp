// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPickupActor.generated.h"

using namespace UP;
using namespace UF;

class USphereComponent;
class ASPowerUpActor;

UCLASS()
class COOPGAME_API ASPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPickupActor();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UDecalComponent* DecalComp;

	UPROPERTY(EditDefaultsOnly, Category = "PowerUp")
	TSubclassOf<ASPowerUpActor> PowerUpClass;

	UFUNCTION()
	void Respawn();

	ASPowerUpActor* PowerUpInstance;

	UPROPERTY(EditDefaultsOnly, Category = "PowerUp")
	float CooldownDuration;

	FTimerHandle TimerHandle_Cooldown;

public:	

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
