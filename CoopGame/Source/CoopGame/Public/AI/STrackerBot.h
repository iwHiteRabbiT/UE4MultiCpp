// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

using namespace UP;
using namespace UF;

class USHealthComponent;
class UMaterialInstanceDynamic;
class USphereComponent;
class USoundCue;

UCLASS()
class COOPGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* meshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USHealthComponent* healthComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* sphereComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* triggerPowerComp;

	FVector GetNextPathPoint();

	FVector nextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float movementForce;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	bool useVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float minDistToChange;

	UFUNCTION()
	void HandleTakeDamage(USHealthComponent* OwnerHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UMaterialInstanceDynamic* matInst;

	void SelfDestruct();

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	UParticleSystem* explodeFX;

	bool bExploded;

	UPROPERTY(ReplicatedUsing = OnRep_StartedSelfDestructChange)
	bool bStartedSelfDestruct;

	UFUNCTION()
	void OnRep_StartedSelfDestructChange(bool oldStartedSelfDestruct);


	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float explosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float explosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float selfDamageInterval;

	FTimerHandle timerHandle_SelfDamage;

	void DamageSelf();

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	USoundCue* startSelfDestructSound;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	USoundCue* selfDestructSound;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	USoundCue* selfDamageSound;

	UFUNCTION()
	void OnTrigger6SelfDestructBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerPowerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerPowerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
