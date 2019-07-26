// Fill out your copyright notice in the Description page of Project Settings.


#include "STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/Character.h"

#include "AI/NavigationSystemBase.h"
#include "Public/NavigationPath.h"
#include "Public/NavigationSystem.h"

#include "DrawDebugHelpers.h"

#include "Components/SHealthComponent.h"
#include "Components/SphereComponent.h"
#include "SCharacter.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "TimerManager.h"

#include "Sound/SoundCue.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	meshComp->SetCanEverAffectNavigation(false);
	meshComp->SetSimulatePhysics(true);
	RootComponent = meshComp;

	healthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	healthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);

	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	sphereComp->SetSphereRadius(400);
	sphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	sphereComp->SetCollisionResponseToChannels(ECR_Ignore);
	sphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	sphereComp->SetupAttachment(RootComponent);

	movementForce = 1000.0f;
	useVelocityChange = false;
	minDistToChange = 100.0f;

	explosionRadius = 300;
	explosionDamage = 40.0f;

	bExploded = false;
	bStartedSelfDestruct = false;

	selfDamageInterval = 1.0f;

	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority)
		nextPathPoint = GetNextPathPoint();	
}

FVector ASTrackerBot::GetNextPathPoint()
{
	ACharacter* playerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	if (playerPawn)
	{
		UNavigationPath* navPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), playerPawn);

		if (navPath)
		{
			if(navPath->PathPoints.Num() > 1)
			{
				return navPath->PathPoints[1];
			}
		}
	}

	return GetActorLocation();
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent* OwnerHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(Health), *GetName());

	if (matInst == nullptr)
		matInst = meshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, meshComp->GetMaterial(0));

	if (matInst)
		matInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);

	if (Health <= 0)
		SelfDestruct();
}

void ASTrackerBot::SelfDestruct()
{
	if (bExploded)
		return;

	bExploded = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explodeFX, GetActorLocation());
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), selfDestructSound, GetActorLocation());

	RootComponent->SetVisibility(false, true);
	meshComp->SetSimulatePhysics(false);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (Role == ROLE_Authority)
	{
		TArray<AActor*> ignoredActors;
		ignoredActors.Add(this);

		UGameplayStatics::ApplyRadialDamage(this, explosionDamage, GetActorLocation(), explosionRadius, nullptr, ignoredActors, this, GetInstigatorController(), true);
		DrawDebugSphere(GetWorld(), GetActorLocation(), explosionRadius, 12, FColor::Red, false, 2.0f, 0, 1.0f);

		SetLifeSpan(2.0f);
	}
}

void ASTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), selfDamageSound, GetActorLocation());
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (bStartedSelfDestruct || bExploded)
		return;

	ASCharacter* playerPawn = Cast<ASCharacter>(OtherActor);

	if (playerPawn)
	{
		if (Role == ROLE_Authority)
			GetWorldTimerManager().SetTimer(timerHandle_SelfDamage, this, &ASTrackerBot::DamageSelf, selfDamageInterval, true, 0.0f);

		bStartedSelfDestruct = true;

		UGameplayStatics::SpawnSoundAttached(startSelfDestructSound, RootComponent);
	}
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role != ROLE_Authority || bExploded)
		return;

	FVector fDir = nextPathPoint - GetActorLocation();
	float dist = fDir.Size();

	if(dist < minDistToChange)
	{
		nextPathPoint = GetNextPathPoint();

		DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached!");
	}
	else	
	{
		fDir *= 1/dist;

		meshComp->AddForce(movementForce * fDir, NAME_None, useVelocityChange);

		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + movementForce * fDir, 32, FColor::Yellow, false, 0.0f, 0, 1.0f);
	}

	DrawDebugSphere(GetWorld(), nextPathPoint, 20, 12, FColor::Yellow, false, 4.0f, 1.0f);
}
