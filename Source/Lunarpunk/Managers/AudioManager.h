// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FMODAudioComponent.h"
#include "FMODBlueprintStatics.h"
#include "GameFrameworks/LunarPunkGameMode.h"
#include "AudioManager.generated.h"

//namespace FMOD
//{
//  namespace Studio
//  {
//    class Bus;
//  }
//}

UCLASS()
class LUNARPUNK_API AAudioManager : public AActor
{
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  AAudioManager();

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

public:
  // Called every frame
  virtual void Tick(float DeltaTime) override;

  void Initialize(ALunarPunkGameMode* LunarPunkGameMode);

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MasterVolume = 1.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SFXVolume = 1.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MusicVolume = 1.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ambient Music")
    class UFMODEvent* Level1AmbientMusic;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ambient Music")
    UFMODEvent* Level2AmbientMusic;

  UPROPERTY()
    FFMODEventInstance ActualMusicInstance;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ambient Music")
    class UFMODEvent* Level1Ambient;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ambient Music")
    class UFMODEvent* Level2Ambient;

  UPROPERTY()
    FFMODEventInstance ActualAmbientInstance;

  UPROPERTY(VisibleAnywhere)
    int ActualLayer = 0;

  UPROPERTY(VisibleAnywhere)
    ALunarPunkGameMode* GameMode;


  UFUNCTION(BlueprintCallable)
    void StopMusic();

  UFUNCTION(BlueprintCallable)
    void PauseMusic();

  UFUNCTION(BlueprintCallable)
    void RestartMusic();

  UFUNCTION(BlueprintCallable)
    void StartLevel1Music();

  UFUNCTION(BlueprintCallable)
    void StartLevel2Music();

  UFUNCTION(BlueprintCallable)
    void AddLayer();

  UFUNCTION(BlueprintCallable)
    void MinusLayer();

  virtual void BeginDestroy() override;
};
