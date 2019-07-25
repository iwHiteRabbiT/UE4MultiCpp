// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CoopGame.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines for Weapons"),
	ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
	meshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = meshComp;

	muzzleSocketName = "MuzzleSocket";
	tracerTargetName = "Target";

	baseDamage = 20.0f;
	vunerableDamageMultiplier = 4.0f;

	rateOfFire = 600;

	SetReplicates(true);

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	timeBetweenShots = 60.0f / rateOfFire;
}

void ASWeapon::Fire()
{
	if (Role < ROLE_Authority)
	{
		ServerFire();
	}

	AActor* myOwner = GetOwner();
	if (myOwner)
	{
		FVector eyeLoc;
		FRotator eyeRot;
		myOwner->GetActorEyesViewPoint(eyeLoc, eyeRot);

		FVector shotDir = eyeRot.Vector();
		FVector eyeEnd = eyeLoc + (shotDir * 10000);

		FCollisionQueryParams qParams;
		qParams.AddIgnoredActor(myOwner);
		qParams.AddIgnoredActor(this);
		qParams.bTraceComplex = true;
		qParams.bReturnPhysicalMaterial = true;

		FVector tracerEnd = eyeEnd;

		EPhysicalSurface surfaceType = SurfaceType_Default;

		FHitResult hit;
		if (GetWorld()->LineTraceSingleByChannel(hit, eyeLoc, eyeEnd, COLLISION_WEAPON, qParams))
		{
			AActor* hitActor = hit.GetActor();

			surfaceType = UPhysicalMaterial::DetermineSurfaceType(hit.PhysMaterial.Get());

			float actualDamage = baseDamage;
			if (surfaceType == SURFACE_FLESHDVULNERABLE)
				actualDamage *= vunerableDamageMultiplier;

			UGameplayStatics::ApplyPointDamage(hitActor, actualDamage, shotDir, hit, myOwner->GetInstigatorController(), this, damageType);

			tracerEnd = hit.ImpactPoint;

			PlayImpactFX(surfaceType, tracerEnd);
		}

		if(DebugWeaponDrawing > 0)
			DrawDebugLine(GetWorld(), eyeLoc, eyeEnd, FColor::White, false, 1, 0, 1);

		PlayFireFX(tracerEnd);

		if (Role == ROLE_Authority)
		{
			hitScanTrace.TraceTo = tracerEnd;
			hitScanTrace.SurfaceType = surfaceType;
			hitScanTrace.ReplicationCount++;
		}

		lastFireTime = GetWorld()->TimeSeconds;
	}
}

void ASWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ASWeapon::ServerFire_Validate()
{
	return true;
}

void ASWeapon::OnRep_HitScanTrace()
{
	PlayFireFX(hitScanTrace.TraceTo);
	PlayImpactFX(hitScanTrace.SurfaceType, hitScanTrace.TraceTo);
}

void ASWeapon::StartFire()
{
	float firstDelay = lastFireTime + timeBetweenShots - GetWorld()->TimeSeconds;
	firstDelay = FMath::Max(0.0f, firstDelay);

	GetWorldTimerManager().SetTimer(timerHandle_TimeBetweenShots, this, &ASWeapon::Fire, timeBetweenShots, true, firstDelay);
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(timerHandle_TimeBetweenShots);
}

void ASWeapon::PlayFireFX(FVector tracerEnd)
{
	if(muzzleFX)
		UGameplayStatics::SpawnEmitterAttached(muzzleFX, meshComp, muzzleSocketName);

	if (tracerFX)
	{
		FVector muzzleLoc = meshComp->GetSocketLocation(muzzleSocketName);
		UParticleSystemComponent* tracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), tracerFX, muzzleLoc);

		if (tracerComp)
			tracerComp->SetVectorParameter(tracerTargetName, tracerEnd);
	}

	APawn* myOwner = Cast<APawn>(GetOwner());
	if (myOwner)
	{
		APlayerController* pc = Cast<APlayerController>(myOwner->GetController());

		if (pc)
			pc->ClientPlayCameraShake(fireCamShake);
	}
}

void ASWeapon::PlayImpactFX(EPhysicalSurface surfaceType, FVector impactPoint)
{
	UParticleSystem* fx = impactDefaultFX;

	switch (surfaceType)
	{
		case SURFACE_FLESHDEFAULT:
			fx = impactFleshDefaultFX;
			break;
	
		case SURFACE_FLESHDVULNERABLE:
			fx = impactFleshVulnerableFX;
			break;
	
		// default:
		// 	fx = impactFleshDefaultFX;
		// 	break;
	}

	if (fx)
	{
		FVector muzzleLoc = meshComp->GetSocketLocation(muzzleSocketName);
		FVector shotDir = impactPoint - muzzleLoc;
		shotDir.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fx, impactPoint, shotDir.Rotation());
	}

}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASWeapon, hitScanTrace, COND_SkipOwner);
}