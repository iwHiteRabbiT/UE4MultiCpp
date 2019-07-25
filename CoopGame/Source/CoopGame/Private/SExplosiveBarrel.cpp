// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SHealthComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	meshComp->SetSimulatePhysics(true);
	meshComp->SetCollisionObjectType(ECC_PhysicsBody);
	RootComponent = meshComp;

	radialForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	radialForce->SetupAttachment(meshComp);
	radialForce->Radius = 250;
	radialForce->bIgnoreOwningActor = true;
	radialForce->bImpulseVelChange = true;
	radialForce->bAutoActivate = false;


	healthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	bExploded = false;

	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

	healthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);
}

void ASExplosiveBarrel::OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (!bExploded && Health <= 0.0f)
	{
		bExploded = true;

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explosionFX, GetActorLocation());

		meshComp->SetMaterial(0, explodedMat);
		meshComp->AddImpulse(FVector::UpVector * impulseForceUp);

		radialForce->FireImpulse();

		//SetLifeSpan(5.0f);
	}
}
