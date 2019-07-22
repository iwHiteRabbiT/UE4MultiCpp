// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	meshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = meshComp;

	muzzleSocketName = "MuzzleSocket";
	tracerTargetName = "Target";	
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASWeapon::Fire()
{
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

		FVector tracerEnd = eyeEnd;

		FHitResult hit;
		if (GetWorld()->LineTraceSingleByChannel(hit, eyeLoc, eyeEnd, ECC_Visibility, qParams))
		{
			AActor* hitActor = hit.GetActor();

			UGameplayStatics::ApplyPointDamage(hitActor, 20.0f, shotDir, hit, myOwner->GetInstigatorController(), this, damageType);

			if(impactFX)
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), impactFX, hit.ImpactPoint, hit.ImpactNormal.Rotation());

			tracerEnd = hit.ImpactPoint;
		}

		DrawDebugLine(GetWorld(), eyeLoc, eyeEnd, FColor::White, false, 1, 0, 1);

		if(muzzleFX)
			UGameplayStatics::SpawnEmitterAttached(muzzleFX, meshComp, muzzleSocketName);

		if (tracerFX)
		{
			FVector muzzleLoc = meshComp->GetSocketLocation(muzzleSocketName);
			UParticleSystemComponent* tracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), tracerFX, muzzleLoc);

			if (tracerComp)
				tracerComp->SetVectorParameter(tracerTargetName, tracerEnd);
		}
	}
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

