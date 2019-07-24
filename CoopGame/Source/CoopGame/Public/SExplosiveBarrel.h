// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosiveBarrel.generated.h"

using namespace UP;

class USHealthComponent;
class URadialForceComponent;

UCLASS()
class COOPGAME_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASExplosiveBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* meshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	UParticleSystem* explosionFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	URadialForceComponent* radialForce;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* healthComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	float impulseForceUp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	UMaterialInterface* explodedMat;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bExploded;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwnerHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
};
