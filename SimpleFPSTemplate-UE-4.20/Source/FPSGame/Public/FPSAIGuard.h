// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSAIGuard.generated.h"

using namespace UP;
using namespace UF;
using namespace UC;

class UPawnSensingComponent;

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle,
	Suspicious,
	Alerted,
	MAX
};

UCLASS()
class FPSGAME_API AFPSAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSAIGuard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* pawnSensingComp;

	UFUNCTION()
	void HandleOnSeePawn(APawn* Pawn);

	UFUNCTION()
	void HandleOnHearNoise(APawn* noiseInstigator, const FVector& Location, float Volume);

	FRotator originalRot;
	FTimerHandle timerHamdle_ResetOrientation;
	UFUNCTION()
	void ResetOrientation();

	EAIState guardState;

	void SetGuardState(EAIState newState);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void OnStateChange(EAIState newState);

	UPROPERTY(EditInstanceOnly, Category = "AI")
	bool bPatrol;

	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bPatrol"))
	TArray<AActor*> goals;

	int currentGoal;
	void PatrolTo();
	void CheckPatrol();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
