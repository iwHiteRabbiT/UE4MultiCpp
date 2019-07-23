// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileWeapon.h"
#include "SProjectile.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"

void ASProjectileWeapon::Fire()
{
    AActor* myOwner = GetOwner();

    if(projectileClass && myOwner)
	{
		FVector eyeLoc;
		FRotator eyeRot;
		myOwner->GetActorEyesViewPoint(eyeLoc, eyeRot);

		//FVector shotDir = eyeRot.Vector();
		//FVector eyeEnd = eyeLoc + (shotDir * 10000);


        FVector muzzleLoc = meshComp->GetSocketLocation(muzzleSocketName);
        //FRotator muzzleRot = meshComp->GetSocketRotation(muzzleSocketName);

        FActorSpawnParameters spawnPrms;
        spawnPrms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        GetWorld()->SpawnActor<ASProjectile>(projectileClass, muzzleLoc, eyeRot);

        lastFireTime = GetWorld()->TimeSeconds;
    }

}