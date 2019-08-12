// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

using namespace UP;
using namespace UF;

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UCameraShake;

USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	FVector_NetQuantize TraceTo;

	UPROPERTY()
 	uint8 ReplicationCount; // Force Replication
};

UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* meshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> damageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* muzzleFX;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName muzzleSocketName;	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* impactDefaultFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* impactFleshDefaultFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* impactFleshVulnerableFX;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName tracerTargetName;	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* tracerFX;

	void PlayFireFX(FVector tracerEnd);

	void PlayImpactFX(EPhysicalSurface surfaceType, FVector impactPoint);
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> fireCamShake;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float baseDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 1))
	float vunerableDamageMultiplier;

	/* Bullet Spread in Degrees */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 0))
	float BulletSpread;

	virtual void Fire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	FTimerHandle timerHandle_TimeBetweenShots;

	float lastFireTime;

	/* RPM */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float rateOfFire;

	float timeBetweenShots;

	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
	FHitScanTrace hitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();

public:
	virtual void StartFire();

	virtual void StopFire();
};
