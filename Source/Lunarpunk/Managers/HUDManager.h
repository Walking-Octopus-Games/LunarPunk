// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Player/PlayableCharacter.h"
#include "HUDManager.generated.h"

class UProgressBar;
class ALunarPunkPlayerController;
class ALunarPunkGameMode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetShowDamagePoints, bool, bShowDamagePoints);
/**
 *
 */
UCLASS()
class LUNARPUNK_API AHUDManager : public AHUD
{
  GENERATED_BODY()

public:

  UFUNCTION()
    void Initialize(APlayableCharacter* PlayableCharacter);

  UFUNCTION()
    void PostInitialize(ALunarPunkPlayerController* PlayerController, ALunarPunkGameMode* GameMode);

  UFUNCTION(BlueprintImplementableEvent)
    void CreatePlayerHUD();

  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void TogglePlayerHUDVisibility(bool bIsVisible);

  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void UpdatePlayerLifeInHUD(float Health);

  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void ShowDamageEffectInHUD(float Damage);

  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void ShowConstantDamageEffectInHUD(bool Show);

  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void ShowGodModeAnimation(bool IsActivated);

  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void ShowGodModeTurretsAnimation(bool IsActivated);

  UFUNCTION(BlueprintCallable)
    void SetShowDamagePoints(bool ShowDamage);

  UFUNCTION(BlueprintCallable)
    bool GetShowEnemiesDamagePoints()
  {
    return bShowDamagePointsInGame;
  }

  virtual void Tick(float DeltaSeconds) override;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UProgressBar* ProgressBarDestroyPortal;

  UPROPERTY()
    APlayableCharacter* PlayerCharacter;

  UPROPERTY(VisibleAnywhere)
    bool bPlayerDestroyingPortal;

  UPROPERTY(VisibleAnywhere)
    float PercentageEachSecond = 0;

  UFUNCTION()
    void UpdateDestroyPortalProgressBar(bool bUpdate, float TimeToDestroyPortal);

  void StartUpdatingDestroyPortalProgressBar(float TimeToDestroyPortal);

  void StopUpdatingDestroyPortalProgressBar();

  //If true, damage points of the enemies will be painted.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Options")
    bool bShowDamagePointsInGame = true;

  //Variable to handle the change in the editor.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Options")
    bool bShowTurretsInfo = true;

  ////If true, damage points of the enemies will be painted.
  //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Options")
  //  bool bShowDamagePoints = false;

  UPROPERTY()
    FSetShowDamagePoints SetShowDamagePointsEvent;


};
