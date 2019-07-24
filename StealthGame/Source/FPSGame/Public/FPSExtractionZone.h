// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSExtractionZone.generated.h"

using namespace UP;

class UBoxComponent;
class UDecalComponent;
class USoundBase;

UCLASS()
class FPSGAME_API AFPSExtractionZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSExtractionZone();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* overlapComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UDecalComponent* decalComp;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundBase* objectiveMissingSound;

	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp,
		int32 otherBodyIndex, bool fromSweep, const FHitResult &SweepResult);

public:	
};
