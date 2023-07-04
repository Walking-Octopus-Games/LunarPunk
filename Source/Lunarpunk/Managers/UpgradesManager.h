// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/LunarPunkPlayerController.h"
#include "RewardsAndUpgrades/Upgrade.h"
#include "UpgradesManager.generated.h"

class ALunarPunkGameMode;

/**
 *
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpgradePickedUp, EUpgradeType, UpgradeType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpgradesMenuScroll);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUpgradeSpent, TSubclassOf<AActor>, UpgradeClass, int32, NumUpgradeSpent);
UCLASS()
class LUNARPUNK_API UUpgradesManager : public UObject
{
  GENERATED_BODY()

public:
  UUpgradesManager();

  UFUNCTION()
    void Initialize(ALunarPunkGameMode* LPGameMode, UUpgradesData* _UpgradesData, ALunarPunkPlayerController* PlayerController);
  UFUNCTION()
    void PlaySelectUpgradeSound();

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UUpgradesData* UpgradesData;

  UFUNCTION(BlueprintCallable)
    void AddUpgrade(EUpgradeType AddUpgrade, int CountUpgrade);

  void AddOrderUpgrades(EUpgradeType upgrade);

  //Select the next available upgrade
  UFUNCTION()
    void SelectNextUpgrade();

  //Select the previous available upgrade
  UFUNCTION()
    void SelectPreviousUpgrade();

  UFUNCTION()
    bool SpendUpgrade(EUpgradeType SpendUpgrade, int CountUpgrade);

  //return count of one type of reward
  UFUNCTION(BlueprintCallable)
    int32 GetNumberOfUpgrade(EUpgradeType UpgradeType) const;

  UFUNCTION(BlueprintCallable)
    void RefillUpgrades();

  UFUNCTION(BlueprintCallable)
    TArray<EUpgradeType> GetOrderUpgrades()
  {
    return OrderUpgrades;
  }
  EUpgradeType GetCurrentUpgrade();


  UFUNCTION(BlueprintCallable)
    TMap<EUpgradeType, int32> GetUpgrades()
  {
    return Upgrades;
  };
  /*
   * Events
   */

   //Delegates to refresh the HUD information about the upgrades.
  UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
    FUpgradePickedUp UpgradePickedUp;

  UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
    FUpgradesMenuScroll UpgradesMenuScroll;

  UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
    FUpgradeSpent UpgradeSpent;

  UPROPERTY(EditAnywhere, Category = "Turret|Sound", BlueprintReadWrite)
    class UFMODEvent* UpgradeSelectedAudioEvent;
private:
  //Map to save actual upgrades and quantity.
  UPROPERTY(EditAnywhere)
    TMap<EUpgradeType, int32> Upgrades;

  UPROPERTY(EditAnywhere)
    TArray<EUpgradeType> OrderUpgrades;

  UPROPERTY()
    ALunarPunkGameMode* GameMode;

};
