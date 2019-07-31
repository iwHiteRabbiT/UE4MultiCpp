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

#include "Net/UnrealNetwork.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(400);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(RootComponent);

	TriggerPowerComp = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerPowerComp"));
	TriggerPowerComp->SetSphereRadius(600);
	TriggerPowerComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerPowerComp->SetCollisionResponseToChannels(ECR_Ignore);
	TriggerPowerComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerPowerComp->SetupAttachment(RootComponent);

	MovementForce = 1000.0f;
	bUseVelocityChange = true;
	MinDistToChange = 100.0f;

	ExplosionRadius = 300;
	ExplosionDamage = 40.0f;

	bExploded = false;

	SelfDamageInterval = 1.0f;

	SetReplicates(true);
	SetReplicateMovement(true);
}

void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);

	if (Role == ROLE_Authority)
	{
		S_NextPathPoint = SERVER_GetNextPathPoint();	

		SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASTrackerBot::SERVER_OnTriggerSelfDestructBeginOverlap);
		TriggerPowerComp->OnComponentBeginOverlap.AddDynamic(this, &ASTrackerBot::SERVER_OnTriggerPowerBeginOverlap);
		TriggerPowerComp->OnComponentEndOverlap.AddDynamic(this, &ASTrackerBot::SERVER_OnTriggerPowerEndOverlap);

		R_bStartedSelfDestruct = false;
		R_TrackerBotNearCount = 0;
	}
}

FVector ASTrackerBot::SERVER_GetNextPathPoint()
{
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	if (PlayerPawn)
	{
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

		if (NavPath)
		{
			if(NavPath->PathPoints.Num() > 1)
			{
				return NavPath->PathPoints[1];
			}
		}
	}

	return GetActorLocation();
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent* OwnerHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (bExploded)
	{
		return;
	}

	if (Health <= 0)
	{
		SelfDestruct();
	}

	if(GetNetMode() != NM_DedicatedServer)
	{
		CLIENT_TakeDamage(DamageCauser == this);
	}
}

void ASTrackerBot::CLIENT_TakeDamage(bool bByItself)
{
	UE_LOG(LogTemp, Log, TEXT("CLIENT: TakeDamage"));

	CLIENT_SetMatFloat("LastTimeDamageTaken", GetWorld()->TimeSeconds);

	if (bByItself)
	{
		UE_LOG(LogTemp, Log, TEXT("CLIENT: SpawnSoundAtLocation"));
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SelfDamageSound, GetActorLocation());
	}
}

void ASTrackerBot::CLIENT_SetMatFloat(FName ParamName, float Value)
{
	if (C_MatInst == nullptr)
	{
		C_MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	if (C_MatInst)
	{
		C_MatInst->SetScalarParameterValue(ParamName, Value);
	}
}

void ASTrackerBot::SelfDestruct()
{
	if (bExploded)
	{
		return;
	}

	bExploded = true;

	if(GetNetMode() != NM_DedicatedServer)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeFX, GetActorLocation());
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SelfDestructSound, GetActorLocation());

		RootComponent->SetVisibility(false, true);
		MeshComp->SetSimulatePhysics(false);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (Role == ROLE_Authority)
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);

		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage * R_TrackerBotNearCount, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);
		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 2.0f, 0, 1.0f);

		SetLifeSpan(2.0f);
	}
}

void ASTrackerBot::OnRep_StartedSelfDestructChange(bool oldStartedSelfDestruct)
{
	if (!oldStartedSelfDestruct && R_bStartedSelfDestruct)
		UGameplayStatics::SpawnSoundAttached(StartSelfDestructSound, RootComponent);
}

void ASTrackerBot::OnRep_TrackerBotNearCount()
{
	CLIENT_SetMatFloat("TrackerBotNearCount", (float)R_TrackerBotNearCount);
}

void ASTrackerBot::SERVER_DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

void ASTrackerBot::SERVER_OnTriggerSelfDestructBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	return;

	if (R_bStartedSelfDestruct || bExploded)
		return;

	ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);

	if (PlayerPawn)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTrackerBot::SERVER_DamageSelf, SelfDamageInterval, true, 0.0f);

		R_bStartedSelfDestruct = true;
		OnRep_StartedSelfDestructChange(false);
	}
}

void ASTrackerBot::SERVER_OnTriggerPowerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor != this && Cast<ASTrackerBot>(OtherActor))
	{
		R_TrackerBotNearCount++;
		OnRep_TrackerBotNearCount();
	}
}

void ASTrackerBot::SERVER_OnTriggerPowerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor != this && Cast<ASTrackerBot>(OtherActor))
	{
		R_TrackerBotNearCount = --R_TrackerBotNearCount<0 ? 0 : R_TrackerBotNearCount;
		OnRep_TrackerBotNearCount();
	}
}

void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bExploded)
	{
		return;
	}

	if (Role == ROLE_Authority)
	{
		FVector Dir = S_NextPathPoint - GetActorLocation();
		float dist = Dir.Size();

		if(dist < MinDistToChange)
		{
			S_NextPathPoint = SERVER_GetNextPathPoint();

			DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached!");
		}
		else	
		{
			Dir *= 1/dist;

			MeshComp->AddForce(MovementForce * Dir, NAME_None, bUseVelocityChange);

			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + MovementForce * Dir, 32, FColor::Yellow, false, 0.0f, 0, 1.0f);
		}

		DrawDebugSphere(GetWorld(), S_NextPathPoint, 20, 12, FColor::Yellow, false, 4.0f, 1.0f);
	}
}

void ASTrackerBot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASTrackerBot, R_bStartedSelfDestruct);
	DOREPLIFETIME(ASTrackerBot, R_TrackerBotNearCount);
}