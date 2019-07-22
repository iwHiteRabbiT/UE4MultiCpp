// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASProjectile::ASProjectile()
{
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = meshComp;

	collisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	collisionComp->SetupAttachment(meshComp);
	//CollisionComp->InitSphereRadius(5.0f);
	//CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	collisionComp->OnComponentHit.AddDynamic(this, &ASProjectile::OnHit);		// set up a notification for when this component hits something blocking
	// Players can't walk on it
	// CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	// CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	projectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	projectileMovement->UpdatedComponent = meshComp;
	projectileMovement->InitialSpeed = 3000.f;
	projectileMovement->MaxSpeed = 3000.f;
	projectileMovement->bRotationFollowsVelocity = true;
	projectileMovement->bShouldBounce = true;
}

void ASProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	// if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	// {
	// 	OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

	 	Destroy();
	// }

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explosionFX, GetActorLocation());

}