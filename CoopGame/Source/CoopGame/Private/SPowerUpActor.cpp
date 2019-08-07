// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUpActor.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ASPowerUpActor::ASPowerUpActor()
{
	PowerUpInterval = 0.0f;
	TotalNbOfTicks = 0;
	RU_TicksProcessed = 0;

	RU_bIsPowerUpActive = false;

	R_ActivatedTargetActor = nullptr;

	SetReplicates(true);
}

void ASPowerUpActor::SERVER_OnTickPowerUp()
{
	RU_TicksProcessed++;	
	OnRep_PowerUpTicked();

	if (RU_TicksProcessed >= TotalNbOfTicks)
	{
		// OnExpired();

		RU_bIsPowerUpActive = false;
		OnRep_PowerUpActive();

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerUpTick);
	}
}

void ASPowerUpActor::OnRep_PowerUpActive()
{
	OnPowerUpStateChange(RU_bIsPowerUpActive);
}

void ASPowerUpActor::OnRep_PowerUpTicked()
{
	OnPowerUpTicked();
}

void ASPowerUpActor::SERVER_ActivatePowerUp(AActor* ActivateFor)
{
	R_ActivatedTargetActor = ActivateFor;
	RU_bIsPowerUpActive = true;
	OnRep_PowerUpActive();

	// OnActivated();

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

	DOREPLIFETIME(ASPowerUpActor, RU_bIsPowerUpActive);
	DOREPLIFETIME(ASPowerUpActor, R_ActivatedTargetActor);
	DOREPLIFETIME(ASPowerUpActor, RU_TicksProcessed);	
}