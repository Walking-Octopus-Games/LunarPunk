// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradesManager.h"

#include "EntityManager.h"
#include "FMODBlueprintStatics.h"
#include "GameFrameworks/LunarPunkGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LunarPunkPlayerController.h"
#include "Player/PlayableCharacter.h"
#include "RewardsAndUpgrades/Upgrade.h"
#include "RewardsAndUpgrades/UpgradesData.h"
#include "Managers/EntityManager.h"
#include "Turrets/Turret.h"

UUpgradesManager::UUpgradesManager()
{
}

void UUpgradesManager::Initialize(ALunarPunkGameMode* LPGameMode, UUpgradesData* _UpgradesData, ALunarPunkPlayerController* PlayerController, UEntityManager* EntityManager)
{
  this->UpgradesData = _UpgradesData;

  for (auto Upgrade : this->UpgradesData->UpgradesOrder)
  {
    AddUpgrade(Upgrade, 0);
  }

  GameMode = LPGameMode;
  PlayerController->SelectNextUpgradeEvent.AddDynamic(this, &UUpgradesManager::SelectNextUpgrade);
  PlayerController->SelectPreviousUpgradeEvent.AddDynamic(this, &UUpgradesManager::SelectPreviousUpgrade);
  PlayerController->RefillUpgradesEvent.AddDynamic(this, &UUpgradesManager::RefillUpgrades);
  UpgradesMenuScroll.AddDynamic(this, &UUpgradesManager::PlaySelectUpgradeSound);


  if (EntityManager)
  {
      for (ATurret* Turret : EntityManager->Turrets)
      {
          NumUpgradesChanged.AddDynamic(Turret, &ATurret::RefreshInfoWidget);
      }
  }
}

void UUpgradesManager::PlaySelectUpgradeSound()
{
  UFMODBlueprintStatics::PlayEvent2D(GetWorld(), UpgradeSelectedAudioEvent, true);
}

void UUpgradesManager::AddUpgrade(EUpgradeType AddUpgrade, int32 Count)
{

  bool Exist = false;
  for (const auto Upgrade : Upgrades)
  {
    if (Upgrade.Key == AddUpgrade)
    {
      FUpgradesDataStruct* UpgradeData = UpgradesData->Upgrades.Find(AddUpgrade);
      if (Upgrade.Value < UpgradeData->MaxUpgrades)
      {
          if (Upgrades.Contains(Upgrade.Key))
          {
              if (Upgrades[Upgrade.Key] == 0)
              {
                  AddOrderUpgrades(AddUpgrade);
                  UpgradesMenuScroll.Broadcast();
              }
              Upgrades[Upgrade.Key] += Count;
              Exist = true;
              break;
          }
      }
    }
  }

  if (!Exist)
  {
    Upgrades.Add(AddUpgrade, Count);
    if (Count > 0)
    {
        AddOrderUpgrades(AddUpgrade);
        UpgradesMenuScroll.Broadcast();
    }
  }

  UpgradePickedUp.Broadcast(AddUpgrade);
  NumUpgradesChanged.Broadcast(0);

}


bool UUpgradesManager::SpendUpgrade(EUpgradeType SpendUpgrade, int Quantity)
{
  for (const auto Upgrade : OrderUpgrades)
  {
    if (Upgrade == SpendUpgrade)
    {
      int result = Upgrades[Upgrade] - Quantity;
      if (result < 0)
      {
        return false;
      }

      Upgrades[Upgrade] = result;
    
    
    
      if (result == 0)
      {
          OrderUpgrades.Remove(Upgrade);
          UpgradesMenuScroll.Broadcast();
      }

      //Refresh the HUD information about the number of the rewards.
      UpgradePickedUp.Broadcast(SpendUpgrade);
      NumUpgradesChanged.Broadcast(0);
      return true;
    }
  }
  return false;
}

int32 UUpgradesManager::GetNumberOfUpgrade(EUpgradeType UpgradeType) const
{
  if (Upgrades.Contains(UpgradeType))
  {
    return Upgrades[UpgradeType];
  }
  else
  {
    return 0;
  }
}

void UUpgradesManager::SelectPreviousUpgrade()
{
  //Shift the CurrentUpgrades array to the left and refresh the HUD
  const int32 NumCurrentUpgrades = OrderUpgrades.Num();
  if (NumCurrentUpgrades > 1)
  {
    EUpgradeType Previous = OrderUpgrades[0];
    OrderUpgrades[0] = OrderUpgrades[NumCurrentUpgrades - 1];
    for (int32 i = 1; i < NumCurrentUpgrades; ++i)
    {
      EUpgradeType Next = OrderUpgrades[i];
      OrderUpgrades[i] = Previous;
      Previous = Next;
    }
  }
  UpgradesMenuScroll.Broadcast();
}

EUpgradeType UUpgradesManager::GetCurrentUpgrade()
{
  if (OrderUpgrades.Num() > 0)
  {
    return OrderUpgrades[0];
  }

  return EUpgradeType::None;
}

void UUpgradesManager::SelectNextUpgrade()
{
  //Shift the CurrentUpgrades array to the right and refresh the HUD
  const int32 NumCurrentUpgrades = OrderUpgrades.Num();
  if (NumCurrentUpgrades > 1)
  {
    EUpgradeType Next = OrderUpgrades[NumCurrentUpgrades - 1];
    OrderUpgrades[NumCurrentUpgrades - 1] = OrderUpgrades[0];
    for (int32 i = NumCurrentUpgrades - 2; i >= 0; --i)
    {
        EUpgradeType Previous = OrderUpgrades[i];
        OrderUpgrades[i] = Next;
        Next = Previous;
    }
  }
  UpgradesMenuScroll.Broadcast();
}



void UUpgradesManager::RefillUpgrades()
{
  int32 Max = UpgradesData->Upgrades[EUpgradeType::Power].MaxUpgrades;
  int32 Num = GetNumberOfUpgrade(EUpgradeType::Power);
  if (Num < Max)
  {
    AddUpgrade(EUpgradeType::Power, Max - Num);
  }

  int32 Max2 = UpgradesData->Upgrades[EUpgradeType::Range].MaxUpgrades;
  int32 Num2 = GetNumberOfUpgrade(EUpgradeType::Range);
  if (Num2 < Max2)
  {
    AddUpgrade(EUpgradeType::Range, Max2 - Num2);
  }

  int32 Max3 = UpgradesData->Upgrades[EUpgradeType::Health].MaxUpgrades;
  int32 Num3 = GetNumberOfUpgrade(EUpgradeType::Health);
  if (Num3 < Max3)
  {
    AddUpgrade(EUpgradeType::Health, Max3 - Num3);
  }

  int32 Max4 = UpgradesData->Upgrades[EUpgradeType::Cadence].MaxUpgrades;
  int32 Num4 = GetNumberOfUpgrade(EUpgradeType::Cadence);
  if (Num4 < Max4)
  {
    AddUpgrade(EUpgradeType::Cadence, Max4 - Num4);
  }
}


void UUpgradesManager::AddOrderUpgrades(EUpgradeType upgrade)
{
    OrderUpgrades.Add(upgrade);
    OrderUpgrades.Sort([](const EUpgradeType& Type1, const EUpgradeType& Type2) {
        int value1 = 0;
        int value2 = 0;

        switch (Type1)
        {
        case EUpgradeType::Power:
            value1 = 1;
            break;
        case EUpgradeType::Cadence:
            value1 = 2;
            break;
        case EUpgradeType::Range:
            value1 = 3;
            break;
        case EUpgradeType::Health:
            value1 = 4;
            break;
        }

        switch (Type2)
        {
        case EUpgradeType::Power:
            value2 = 1;
            break;
        case EUpgradeType::Cadence:
            value2 = 2;
            break;
        case EUpgradeType::Range:
            value2 = 3;
            break;
        case EUpgradeType::Health:
            value2 = 4;
            break;
        }

        return value1 < value2;
        });
}