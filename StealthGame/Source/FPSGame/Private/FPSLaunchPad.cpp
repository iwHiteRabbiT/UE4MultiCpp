// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSLaunchPad.h"
#include "FPSCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DecalComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSLaunchPad::AFPSLaunchPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	overlapComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComp"));
	overlapComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	overlapComp->SetBoxExtent(FVector(200.0f));
	overlapComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSLaunchPad::OnEnterLaunchPad);
	RootComponent = overlapComp;

	baseBoxComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseBox"));
	baseBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	baseBoxComp->SetupAttachment(overlapComp);

	arrowBoxComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowDecalComp"));
	arrowBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	arrowBoxComp->SetupAttachment(overlapComp);
}

// Called when the game starts or when spawned
void AFPSLaunchPad::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFPSLaunchPad::OnEnterLaunchPad(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	FVector vel = GetActorForwardVector();
	vel.Z += vPower * vel.Size();
	vel *= power;

	AFPSCharacter* character = Cast<AFPSCharacter>(OtherActor);

	if (character)
	{
		character->LaunchCharacter(vel, true, true);
	}
	else if (OtherComp && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulse(vel, NAME_None, true);
	}

	UGameplayStatics::SpawnEmitterAtLocation(this, pickupFX, GetActorLocation());
}

// Called every frame
void AFPSLaunchPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

