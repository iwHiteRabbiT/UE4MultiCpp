// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	defaultHealth = 100.0f;

	SetIsReplicated(true);
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Log, TEXT("SERVER: Health BeginPlay"));

		AActor* myOwner = GetOwner();

		if (myOwner)
			myOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("CLIENT: Health BeginPlay"));
	}

	health = defaultHealth;
}

void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f)
		return;

	damageCauser = DamageCauser;
	health = FMath::Clamp(health - Damage, 0.0f, defaultHealth);

	UE_LOG(LogTemp, Log, TEXT("SERVER: Health changed %s"), *FString::SanitizeFloat(health));

	OnHealthChanged.Broadcast(this, health, Damage, DamageType, InstigatedBy, DamageCauser);
}

void USHealthComponent::OnRep_HealthChange(float oldHealth)
{
	UE_LOG(LogTemp, Log, TEXT("CLIENT: Health changed %s"), *FString::SanitizeFloat(health));

	float damage = oldHealth - health;

	OnHealthChanged.Broadcast(this, health, damage, nullptr, nullptr, damageCauser);
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, health);
	DOREPLIFETIME(USHealthComponent, damageCauser);
}