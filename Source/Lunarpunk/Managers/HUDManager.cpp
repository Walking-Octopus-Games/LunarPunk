// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/HUDManager.h"
#include <cassert>
#include "Components/ProgressBar.h"
#include "Player/LunarPunkPlayercontroller.h"
#include "Player/Abilities/DestroyPortalAbilityComponent.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "Managers/EntityManager.h"
#include "Turrets/Turret.h"
#include "Turrets/PositionHelperIcon.h"


void AHUDManager::Initialize(APlayableCharacter* PlayableCharacter)
{
  PlayerCharacter = PlayableCharacter;
  PlayerCharacter->DestroyPortalAbilityComponent->SetAvailableToDestroyPortal.AddDynamic(this, &AHUDManager::UpdateDestroyPortalProgressBar);
}

void AHUDManager::SetShowDamagePoints(bool ShowDamage)
{
  bShowDamagePointsInGame = ShowDamage;
  SetShowDamagePointsEvent.Broadcast(bShowDamagePointsInGame);
}

void AHUDManager::Tick(float DeltaSeconds)
{
  Super::Tick(DeltaSeconds);
  if (bPlayerDestroyingPortal)
  {
    if (ProgressBarDestroyPortal->Percent == 100.f)
    {
      StopUpdatingDestroyPortalProgressBar();
    }
    else
    {
      ProgressBarDestroyPortal->SetPercent((PercentageEachSecond * DeltaSeconds + ProgressBarDestroyPortal->Percent));
    }
  }
}

void AHUDManager::UpdateDestroyPortalProgressBar(bool bUpdate, float TimeToDestroyPortal)
{
  if (bUpdate)
  {
    StartUpdatingDestroyPortalProgressBar(TimeToDestroyPortal);
  }
  else
  {
    StopUpdatingDestroyPortalProgressBar();
  }
}

void AHUDManager::StartUpdatingDestroyPortalProgressBar(float TimeToDestroyPortal)
{
  assert(TimeToDestroyPortal == 0);

  PercentageEachSecond = 1 / TimeToDestroyPortal;

  bPlayerDestroyingPortal = true;
}

void AHUDManager::StopUpdatingDestroyPortalProgressBar()
{
  bPlayerDestroyingPortal = false;
  ProgressBarDestroyPortal->SetPercent(0.f);
}

void AHUDManager::PostInitialize(ALunarPunkPlayerController* PlayerController, ALunarPunkGameMode* GameMode)
{
  CreatePlayerHUD();
  SetShowDamagePoints(bShowDamagePointsInGame);

  for (ATurret* Turret : GameMode->EntityManager->Turrets)
  {
      Turret->FindComponentByClass<UPositionHelperIcon>()->StartPositionIcon();
  }
  
  
}
