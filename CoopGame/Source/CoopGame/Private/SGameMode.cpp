// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMode.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Components/SHealthComponent.h"
#include "SGameState.h"
#include "SPlayerState.h"

ASGameMode::ASGameMode()
{
    WaveCount = 0;
    NbBotsToSpawn = 0;
    TimeBetweenWaves = 5.0f;

    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 1.0f;

    GameStateClass = ASGameState::StaticClass();
    PlayerStateClass = ASPlayerState::StaticClass();
}

void ASGameMode::StartWave()
{
    WaveCount++;

    NbBotsToSpawn = 2 * WaveCount;

    GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ASGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);

    SetWaveState(EWaveState::WaveInProgress);
}

void ASGameMode::SpawnBotTimerElapsed()
{
    SpawnNewBot();

    NbBotsToSpawn--;

    if (NbBotsToSpawn <= 0)
    {
        EndWave();
    }
}

void ASGameMode::EndWave()
{
    GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

    SetWaveState(EWaveState::WaitingToComplete);
}

void ASGameMode::PrepareForNextWave()
{
    GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASGameMode::StartWave, TimeBetweenWaves, false);

    SetWaveState(EWaveState::WaitingToStart);

    RestartDeadPlayers();
}

void ASGameMode::CheckWaveState()
{
    bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

    if (bIsPreparingForWave || NbBotsToSpawn > 0)
    {
        return;
    }

    bool bIsAnyBotAlive = false;

    for(FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
    {
        APawn* TestPawn = It->Get();

        if (TestPawn == nullptr || TestPawn->IsPlayerControlled())
        {
            continue;
        }

        USHealthComponent* HealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));

        if (HealthComp && HealthComp->GetHealth() > 0.0f)
        {
            bIsAnyBotAlive = true;
            break;
        }
    }

    if (!bIsAnyBotAlive)
    {
        SetWaveState(EWaveState::WaveComplete);

        PrepareForNextWave();
    }
}

void ASGameMode::CheckAnyPlayerAlive()
{
    for(FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
    {
        APlayerController* PC = It->Get();

        if (PC && PC->GetPawn())
        {
            APawn* MyPawn = PC->GetPawn();
            USHealthComponent* HealthComp = Cast<USHealthComponent>(MyPawn->GetComponentByClass(USHealthComponent::StaticClass()));

            if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f)
            {
                return;
            }
        }
    }

    GameOver();
}

void ASGameMode::GameOver()
{
    EndWave();

    UE_LOG(LogTemp, Log, TEXT("GameOver"));

    SetWaveState(EWaveState::GameOver);
}

void ASGameMode::SetWaveState(EWaveState NewWaveState)
{
    ASGameState* GS = GetGameState<ASGameState>();
    if (ensureAlways(GS))
    {
        GS->SetWaveState(NewWaveState);
    }
}

void ASGameMode::RestartDeadPlayers()
{
    for(FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
    {
        APlayerController* PC = It->Get();

        if (PC && PC->GetPawn() == nullptr)
        {
            RestartPlayer(PC);
        }
    }
}

void ASGameMode::StartPlay()
{
    Super::StartPlay();

    PrepareForNextWave();    
}

void ASGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    CheckWaveState();
    CheckAnyPlayerAlive();
}
