// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "SGameMode.h"
#include "Engine/World.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	DefaultHealth = 100.0f;
	bIsDead = false;

	SetIsReplicated(true);
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		R_Health = DefaultHealth;

		AActor* MyOwner = GetOwner();

		if (MyOwner)
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::SERVER_HandleTakeAnyDamage);			
	}
}

void USHealthComponent::SERVER_HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	R_DamageCauser = DamageCauser;
	float Old_Health = R_Health;

	R_Health = FMath::Clamp(R_Health - Damage, 0.0f, DefaultHealth);
	OnRep_HealthChange(Old_Health);

	// UE_LOG(LogTemp, Log, TEXT("SERVER: HandleTakeAnyDamage %s"), *FString::SanitizeFloat(R_Health));
	//OnHealthChanged.Broadcast(this, R_Health, Damage, DamageType, InstigatedBy, DamageCauser);

	bIsDead = R_Health <= 0.0f;

	if (bIsDead)
	{
		ASGameMode* GM = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}
}

void USHealthComponent::OnRep_HealthChange(float OldHealth)
{
	// AActor* MyOwner = GetOwner();
	// UE_LOG(LogTemp, Log, TEXT("CLIENT: OnRep_HealthChange %s %s"), *FString::SanitizeFloat(R_Health), *MyOwner->GetName());

	float Damage = OldHealth - R_Health;

	OnHealthChanged.Broadcast(this, R_Health, Damage, nullptr, nullptr, R_DamageCauser);
}

float USHealthComponent::GetHealth() const
{
	return R_Health;
}

void USHealthComponent::Heal(float HealAmount)
{	
	if (HealAmount <= 0.0f || R_Health <= 0.0f)
	{
		return;
	}

	if (GetOwnerRole() == ROLE_Authority)
	{
		float Old_Health = R_Health;
		R_Health = FMath::Clamp(R_Health + HealAmount, 0.0f, DefaultHealth);

		UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(R_Health), *FString::SanitizeFloat(HealAmount));

		R_DamageCauser = nullptr;
		//OnHealthChanged.Broadcast(this, R_Health, HealAmount, nullptr, nullptr, nullptr);
		OnRep_HealthChange(Old_Health);
	}
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, R_Health);
	DOREPLIFETIME(USHealthComponent, R_DamageCauser);
}