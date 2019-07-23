// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SProjectile.generated.h"

using namespace UP;
using namespace UF;

class USphereComponent;
class UProjectileMovementComponent;
//class UParticleSystem;

UCLASS()
class COOPGAME_API ASProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASProjectile();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* meshComp;

	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	USphereComponent* collisionComp;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	UProjectileMovementComponent* projectileMovement;

	// UPROPERTY(EditDefaultsOnly, Category = "FX")
	// UParticleSystem* explosionFX;

	// UFUNCTION()
	// void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
