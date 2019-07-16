// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSExtractionZone.h"
#include "FPSCharacter.h"
#include "FPSGameMode.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSExtractionZone::AFPSExtractionZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	overlapComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComp"));
	overlapComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	overlapComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	overlapComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	overlapComp->SetBoxExtent(FVector(200.0f));
	RootComponent = overlapComp;

	overlapComp->SetHiddenInGame(false);

	overlapComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSExtractionZone::HandleOverlap);


	decalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	decalComp->DecalSize = FVector(200.0f);
	decalComp->SetupAttachment(RootComponent);
}

void AFPSExtractionZone::HandleOverlap(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp,
	int32 otherBodyIndex, bool fromSweep, const FHitResult &SweepResult)
{

	AFPSCharacter* myPawn = Cast<AFPSCharacter>(otherActor);

	if(myPawn == nullptr)
		return;

	UE_LOG(LogTemp, Log, TEXT("Overlapped with extraction zone!"));

	if(!myPawn->bIsCarryingObjective)
	{
		UGameplayStatics::PlaySound2D(this, objectiveMissingSound);
		return;
	}

	//if (myPawn && myPawn->bIsCarryingObjective)
	//{

		AFPSGameMode* gameMode = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());

		if (gameMode)
			gameMode->CompleteMission(myPawn);
	//}
}

