// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

using namespace UF;

/**
 * 
 */
UCLASS()
class FPSGAME_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerController")
	void OnMissionComplete(APawn* instigatorPawn, bool bMissionSuccess);
	
};
