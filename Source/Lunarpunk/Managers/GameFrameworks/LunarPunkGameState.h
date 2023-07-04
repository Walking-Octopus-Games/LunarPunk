// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LunarPunkGameState.generated.h"

class ALunarPunkGameMode;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOpenZone);
/**
 *
 */
UCLASS()
class LUNARPUNK_API ALunarPunkGameState : public AGameStateBase
{
  GENERATED_BODY()

public:
  ALunarPunkGameState();



  UFUNCTION()
    void OpenNextZone(int32 IndexZoneToOpen);

  //Array to store the Conquest Zones in the level
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 IndexFirstConquestZoneToOpen = 0;


  UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
    FOpenZone OpenZone;

  UFUNCTION(BlueprintCallable)
    float GetLimitTime();

  UFUNCTION(BlueprintCallable)
    void PauseLimitTimer();

  UFUNCTION(BlueprintCallable)
    void ResumeLimitTimer();

  UFUNCTION(BlueprintCallable)
    float GetRemainingGameTime();

  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void EndGame();

  UFUNCTION()
    void Initialize(ALunarPunkGameMode* LunarPunkGameMode);
private:
  UPROPERTY()
    const ALunarPunkGameMode* GameMode;

  UFUNCTION()
    void InitializeConquestZones();

  UPROPERTY(EditAnywhere, Category = "Gameplay")
    float LimitTime = 60.0f;

  //Handle del Timer
  FTimerHandle TimerHandle;


};
