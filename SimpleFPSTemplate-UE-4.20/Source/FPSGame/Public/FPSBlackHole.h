// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSBlackHole.generated.h"

using namespace UP;

class USphereComponent;

UCLASS()
class FPSGAME_API AFPSBlackHole : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSBlackHole();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* meshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* sphereComp;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* pickupFX;

	UPROPERTY(EditAnywhere)
	float power;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	void Tick(float DeltaTime);

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
