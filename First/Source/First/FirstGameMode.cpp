// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FirstGameMode.h"
#include "FirstHUD.h"
#include "FirstCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFirstGameMode::AFirstGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFirstHUD::StaticClass();
}
