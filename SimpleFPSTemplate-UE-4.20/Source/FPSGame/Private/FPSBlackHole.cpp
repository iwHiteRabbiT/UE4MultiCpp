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
	// sphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// sphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	// sphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	sphereComp->SetupAttachment(meshComp);

	innerSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("InnerSphereComp"));
	innerSphereComp->SetupAttachment(meshComp);

	innerSphereComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSBlackHole::OnOverlapInner);
}

void AFPSBlackHole::OnOverlapInner(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp,
	int32 otherBodyIndex, bool fromSweep, const FHitResult &SweepResult)
{
	if (otherActor)
		otherActor->Destroy();
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

	for (int32 i = 0; i < collectedActors.Num(); i++) 
    {
        UPrimitiveComponent* anActor = collectedActors[i];

		if (anActor && anActor->IsSimulatingPhysics()) 
		{
			const float radius = sphereComp->GetScaledSphereRadius();

			//anActor->AddForce((GetActorLocation() - anActor->GetComponentLocation()).GetSafeNormal() * power);
			anActor->AddRadialForce(GetActorLocation(), radius, power, ERadialImpulseFalloff::RIF_Constant, true);
		}
		//anActor->AddRadialForce()
	}
}
