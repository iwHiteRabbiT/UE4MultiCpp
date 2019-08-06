// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUpActor.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ASPowerUpActor::ASPowerUpActor()
{
	PowerUpInterval = 0.0f;
	TotalNbOfTicks = 0;
	TicksProcessed = 0;

	bIsPowerUpActive = false;

	SetReplicates(true);
}

void ASPowerUpActor::SERVER_OnTickPowerUp()
{
	TicksProcessed++;

	OnPowerUpTicked();

	if (TicksProcessed >= TotalNbOfTicks)
	{
		OnExpired();

		bIsPowerUpActive = false;
		OnRep_PowerUpActive();

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerUpTick);
	}
}

void ASPowerUpActor::OnRep_PowerUpActive()
{
	OnPowerUpStateChange(bIsPowerUpActive);
}

void ASPowerUpActor::SERVER_ActivatePowerUp()
{
	OnActivated();

	bIsPowerUpActive = true;
	OnRep_PowerUpActive();

	if (PowerUpInterval > 0)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerUpTick, this, &ASPowerUpActor::SERVER_OnTickPowerUp, PowerUpInterval, true);
	}
	else
	{
		SERVER_OnTickPowerUp();
	}
}

void ASPowerUpActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerUpActor, bIsPowerUpActive);
}