// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	pawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::HandleOnSeePawn);
	pawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::HandleOnHearNoise);
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();

	originalRot = GetActorRotation();
	guardState = EAIState::MAX;
	SetGuardState(EAIState::Idle);
}

void AFPSAIGuard::HandleOnSeePawn(APawn* Pawn)
{
	if (Pawn == nullptr)
		return;

	DrawDebugSphere(GetWorld(), Pawn->GetActorLocation(), 32.0f, 12, FColor::Yellow, false, 10.0f);

	AFPSGameMode* gameMode = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());

	if (gameMode)
		gameMode->CompleteMission(Pawn, false);

	SetGuardState(EAIState::Alerted);
}

void AFPSAIGuard::HandleOnHearNoise(APawn* noiseInstigator, const FVector& Location, float Volume)
{
	if (guardState == EAIState::Alerted)
		return;

	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Red, false, 10.0f);

	FVector dir = Location - GetActorLocation();
	dir.Z = 0;
	dir.Normalize();

	FRotator rot = FRotationMatrix::MakeFromX(dir).Rotator();

	SetActorRotation(rot);

	GetWorldTimerManager().ClearTimer(timerHamdle_ResetOrientation);
	GetWorldTimerManager().SetTimer(timerHamdle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f);

	SetGuardState(EAIState::Suspicious);
}

void AFPSAIGuard::ResetOrientation()
{
	if (guardState == EAIState::Alerted)
		return;
		
	SetActorRotation(originalRot);
	SetGuardState(EAIState::Idle);
}

void AFPSAIGuard::SetGuardState(EAIState newState)
{
	if (guardState == newState)
		return;
	guardState = newState;

	// Nav
	if (guardState == EAIState::Idle)
		PatrolTo();
	else
	{
		AController* controller = GetController();

		if (controller)
			controller->StopMovement();
	}

	// if (guardState == EAIState::Alerted)
	// 	return;

	OnStateChange(guardState);
}

void AFPSAIGuard::PatrolTo()
{
	if (!bPatrol)
		return;

	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), goals[currentGoal]);
}

void AFPSAIGuard::CheckPatrol()
{
	if (!bPatrol)
		return;

	AActor* goal = goals[currentGoal];

	float dist = (goal->GetActorLocation() - GetActorLocation()).SizeSquared();
	if (dist < 100*100)	
	{
		currentGoal = (currentGoal + 1) % goals.Num();
		PatrolTo();
	}
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckPatrol();
}

