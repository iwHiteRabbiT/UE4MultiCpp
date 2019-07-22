// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileWeapon.h"
#include "SProjectile.h"
#include "Engine/World.h"

void ASProjectileWeapon::Fire()
{
    			// 	const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				// const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				//World->SpawnActor<AFirstProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);

    if(projectileClass)
    {
        FVector muzzleLoc = meshComp->GetSocketLocation(muzzleSocketName);
        FRotator rot = GetActorRotation();

        GLog->Log(rot.Vector().ToString());

        GetWorld()->SpawnActor<ASProjectile>(projectileClass, muzzleLoc, rot);
    }

}