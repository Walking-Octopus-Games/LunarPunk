// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EngineUtils.h"
#include "Player/PlayableCharacter.h"
#include "LunarPunkGameMode.generated.h"


class APlatesManager;
class UAbilitiesManger;
class UAbilitiesData;
class UUpgradesManager;
class UUpgradesData;
class UEntityManager;
class ACameraManager;
class AHUDManager;
class ALunarPunkGameState;
class ULevelSequencePlayer;
class AAudioManager;
class UStatsManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartGameModeInitialize);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndGameModeInitialize);


/**
 *
 */
UCLASS()
class LUNARPUNK_API ALunarPunkGameMode : public AGameModeBase
{
  GENERATED_BODY()
public:


  UPROPERTY(EditAnywhere)
    TSubclassOf<UEntityManager> EntityManagerClass;

  UPROPERTY(BlueprintReadWrite)
    UEntityManager* EntityManager;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AAudioManager* AudioManager;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UUpgradesManager* UpgradesManager;

  UPROPERTY(VisibleAnywhere)
    UUpgradesData* UpgradesDataAsset;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf< APlatesManager> PlatesManagerClass;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    APlatesManager* PlatesManager;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UAbilitiesManger* AbilitiesManager;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    ACameraManager* CameraManager;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    AHUDManager* HUDManager;

  UPROPERTY(BlueprintAssignable)
    FStartGameModeInitialize StartGameModeInitializeEvent;

  UPROPERTY(BlueprintAssignable)
    FEndGameModeInitialize EndGameModeInitializeEvent;

  UPROPERTY()
    ALunarPunkGameState* LPGameState;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ULevelSequencePlayer* SequencePlayer;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bUIMap = false;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UStatsManager* StatsManager;

  ALunarPunkGameMode(const class FObjectInitializer& ObjectInitializer);

  virtual void BeginPlay() override;

  UFUNCTION()
    void InitAudioManager();

  UFUNCTION(BlueprintCallable)
    void Initialize();

  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void EndGame();

  UFUNCTION(BlueprintCallable)
    void GodMode();

  UFUNCTION(BlueprintCallable)
    void GodModeTurrets();

  UFUNCTION(BlueprintCallable)
    bool GetGodMode(){return bIsGodModeActivated;}

  UFUNCTION(BlueprintCallable)
    bool GetGodModeTurrets() {return bIsGodModeTurretsActivated;  }

  //Return the Plates Manager.
  UFUNCTION(BlueprintCallable)
    APlatesManager* GetPlatesManager() const
  {
    return PlatesManager;
  }

  virtual void PostLogin(APlayerController* NewPlayer) override;
  virtual void StartPlay() override;
  virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

private:


  bool bIsGodModeActivated = false;

  bool bIsGodModeTurretsActivated = false;

  //Handle del Timer
  FTimerHandle TimerHandle;

  UFUNCTION(BlueprintCallable)
    void InitEntityManager(APlayableCharacter* PlayableCharacter, ALunarPunkPlayerController* PlayerController);

  UFUNCTION(BlueprintCallable)
    void InitUpgradesManager(ALunarPunkPlayerController* PlayerController);

  UFUNCTION()
    void InitPlatesManager(ALunarPunkPlayerController* PlayerController, UEntityManager* EntityManagerReference);

  UFUNCTION()
    void InitAbilitiesManager(ALunarPunkPlayerController* PlayerController);

  UFUNCTION()
    void InitHUDManager(ALunarPunkPlayerController* PlayerController, APlayableCharacter* PlayableCharacter);

  UFUNCTION()
    void InitStatsManager(ALunarPunkPlayerController* PlayerController);
};
