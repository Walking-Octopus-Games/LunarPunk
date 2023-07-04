// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/GameFrameworks/LunarPunkGameState.h"

#include "LunarPunkGameMode.h"
#include "HealthSystem/HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LevelLayout/ConquestZone.h"
#include "Managers/EntityManager.h"
#include "Player/PlayableCharacter.h"

ALunarPunkGameState::ALunarPunkGameState()
{

}


void ALunarPunkGameState::Initialize(ALunarPunkGameMode* _GameMode)
{

  GameMode = _GameMode;

  InitializeConquestZones();

  //Opens the first Zone.
  OpenNextZone(IndexFirstConquestZoneToOpen);

  GetWorld()->GetTimerManager().SetTimer(TimerHandle,
    this,
    &ALunarPunkGameState::EndGame,
    0.01f,
    false,
    LimitTime - 0.01f);
}


void ALunarPunkGameState::OpenNextZone(int32 IndexZoneToOpen)
{
  //UE_LOG(LogTemp, Warning, TEXT("Abrimos Sector"));

  TArray<AActor*> ConquestZones = GameMode->EntityManager->ConquestZones;

  if (ConquestZones.Num() > IndexZoneToOpen && IsValid(ConquestZones[IndexZoneToOpen]))
  {
    AConquestZone* ConquestZoneCasted = Cast<AConquestZone>(ConquestZones[IndexZoneToOpen]);
    if (IsValid(ConquestZoneCasted))
    {
      ConquestZoneCasted->OpenConquestZone();
      OpenZone.Broadcast();
    }
  }
}

void ALunarPunkGameState::InitializeConquestZones()
{
  //GameMode->EntityManager->InitializeConquestZones();

  //Binding open next zone to event
  for (AActor* ConquestZone : GameMode->EntityManager->ConquestZones)
  {
    AConquestZone* ConquestZoneCasted = Cast<AConquestZone>(ConquestZone);
    if (IsValid(ConquestZoneCasted))
    {
      //Bind to the SectorClosed delegate of each sector.
      ConquestZoneCasted->ZoneCompleted.AddUObject(this, &ALunarPunkGameState::OpenNextZone);
    }
  }

}

float ALunarPunkGameState::GetLimitTime()
{
  return LimitTime;
}

void ALunarPunkGameState::PauseLimitTimer()
{
  GetWorld()->GetTimerManager().PauseTimer(TimerHandle);
}

void ALunarPunkGameState::ResumeLimitTimer()
{
  GetWorld()->GetTimerManager().UnPauseTimer(TimerHandle);
}

float ALunarPunkGameState::GetRemainingGameTime()
{
  return GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle);
}
