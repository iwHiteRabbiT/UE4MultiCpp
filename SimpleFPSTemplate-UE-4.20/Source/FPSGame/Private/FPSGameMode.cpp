// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "FPSGameState.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();

	GameStateClass = AFPSGameState::StaticClass();
}

void AFPSGameMode::CompleteMission(APawn* instigatorPawn, bool bMissionSuccess)
{
	if (spectatingViewpointClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("spectatingViewpointClass == nullptr"));
		return;
	}

	if(instigatorPawn)
	{
		TArray<AActor*> returnedActors;
		UGameplayStatics::GetAllActorsOfClass(this, spectatingViewpointClass, returnedActors);

		if (returnedActors.Num() > 0)
		{
			AActor* newViewTarget = returnedActors[0];

			for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it; it++)
		    {
        	    APlayerController* pc = it->Get();
				if (pc)
				{
					pc->SetViewTargetWithBlend(newViewTarget, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
				}
			}
		}
	}

	AFPSGameState* gs = GetGameState<AFPSGameState>();
	if(gs)
		gs->MulticastOnMissionComplete(instigatorPawn, bMissionSuccess);

	OnMissionCompleted(instigatorPawn, bMissionSuccess);
}
