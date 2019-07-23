// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthComponent.h"
#include "GameFramework/Actor.h"


// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	defaultHealth = 100.0f;
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* myOwner = GetOwner();

	if (myOwner)
		myOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);

	health = defaultHealth;
}

void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f)
		return;

	health = FMath::Clamp(health - Damage, 0.0f, defaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health changed %s"), *FString::SanitizeFloat(health));

	OnHealthChanged.Broadcast(this, health, Damage, DamageType, InstigatedBy, DamageCauser);
}