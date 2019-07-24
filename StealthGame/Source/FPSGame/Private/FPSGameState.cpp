// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameState.h"
#include "FPSPlayerController.h"

void AFPSGameState::MulticastOnMissionComplete_Implementation(APawn* instigatorPawn, bool bMissionSuccess)
{
    for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it; it++)
    {
            AFPSPlayerController* pc = Cast<AFPSPlayerController>(it->Get());

            if (pc && pc->IsLocalController())
            {
                pc->OnMissionComplete(instigatorPawn, bMissionSuccess);

                APawn* pawn = pc->GetPawn();
                if (pawn)
                    pawn->DisableInput(nullptr);
            }
    }
}