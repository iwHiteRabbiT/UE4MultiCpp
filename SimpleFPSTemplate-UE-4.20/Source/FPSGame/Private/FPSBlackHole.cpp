// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSBlackHole.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
AFPSBlackHole::AFPSBlackHole()
{
	PrimaryActorTick.bCanEverTick = true;

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = meshComp;

	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	sphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	sphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	sphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	sphereComp->SetupAttachment(meshComp);
}

// Called when the game starts or when spawned
void AFPSBlackHole::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFPSBlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<UPrimitiveComponent*> collectedActors;
	sphereComp->GetOverlappingComponents(collectedActors);

	for (int i = 0; i < collectedActors.Num(); i++) 
    { 
        UPrimitiveComponent* anActor = collectedActors[i];
		anActor->AddForce((GetActorLocation() - anActor->GetComponentLocation()).GetSafeNormal() * power);
		//anActor->AddRadialForce()
	}
}

void AFPSBlackHole::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// AFPSCharacter* myChar = Cast<AFPSCharacter>(OtherActor);

	// if (myChar)
	// {
	// 	myChar->bIsCarryingObjective = true;

	// 	Destroy();
	// }
}

