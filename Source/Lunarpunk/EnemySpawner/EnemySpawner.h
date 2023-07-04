// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FMODBlueprintStatics.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner/HowManySpawns.h"
#include "WaveData.h"
#include "EnemySpawner.generated.h"

class UFMODEvent;
struct FFMODEventInstance;

UCLASS()
class LUNARPUNK_API AEnemySpawner : public AActor, public IHowManySpawns
{
  GENERATED_BODY()

public:

  AEnemySpawner();

  //Array to stores the classes to spawn in the current wave.
  UPROPERTY(VisibleAnyWhere)
    TArray<TSubclassOf<AActor>> ClassesToSpawn;

  //Waves contains in the spawner.
  UPROPERTY(EditAnyWhere)
    UWaveData* WavesData;

  //Bool to Activate the Spawner
  UPROPERTY(EditAnyWhere)
    bool bIsActive = false;

  //Bool to Deactivate the spawner for debug reasons
  UPROPERTY(EditAnyWhere, Category = "Debug")
    bool bDeactivateForDebug = false;

  //Num of previous last waves to repeat for ever.
  UPROPERTY(EditAnyWhere)
    int32 NumWavesToRepeatAtEnd = 1;


  //Index to asociate the Spawner to a level sector.
  UPROPERTY(EditAnyWhere)
    int32 OwnerSectorIndex = 0;


  //Reference to the game Pool.
  UPROPERTY(VisibleAnywhere)
    class APool* MyPool;

  //Spawn area
  UPROPERTY(VisibleAnyWhere)
    class UBoxComponent* SpawnArea;

private:

  //Max Num of spawns in the current iteration of the wave. a Spanwear de cada tipo en la ronda actual
  TMap<TSubclassOf<AActor>, int32> MaxEnemiesToSpawn;
  //Num of enemies spanwed in the current wave.
  TMap<TSubclassOf<AActor>, int32> CurrentEnemiesSpawned;
  //Num of enemies spawn each time by class. (From 0 to MaxEnemiesSpawnedEachTime).
  TMap<TSubclassOf<AActor>, int32> EnemiesSpawnedEachTime;


  int32 CurrentWaveCounter = 0;
  //Reference to the current wave.
  FWaveDataStruct CurrentWave;


  //Bool to know when the spawner has started.
  bool bStarted = false;


  //Handle del Timer
  FTimerHandle TimerHandle;


protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

  UPROPERTY(EditAnyWhere, Category = "Type Class")
    UClass* EvolvedEnemy;


public:
  // Called every frame
  virtual void Tick(float DeltaTime) override;




  //Function to calculate how many actors are required for each class.
  virtual void GetTotalSpawns(TMap<TSubclassOf<AActor>, int32>& ArraySpawns) override;



  UFUNCTION(BlueprintCallable, Category = "Enemies")
    int32 GetMaxRounds()
  {
    return WavesData->Waves.Num();
  };

  UFUNCTION(BlueprintCallable)
  void ActivateSpawner(const TMap<TSubclassOf<AActor>, int32>& MaxEnemiesOfEachClassInScreen);

  UFUNCTION(BlueprintCallable)
  void DeactivateSpawner();


  UPROPERTY(EditAnywhere, Category = "Spawner|Sound")
    UFMODEvent* AmbientHordeAudioEvent;

  UPROPERTY()
    FFMODEventInstance AmbientHordeAudioInstance;

private:


  //Function to Start a new wave
  void StartNewWave();

  //Do the next iterate of the wave
  void IterateWave();

  //Do the spawn of one actor of the specified class
  void SpawnEnemy(TSubclassOf<AActor> classToSpawn);

  //Checks if an enemy can be spawned and how many oh them.
  bool CanSpawnEnemy(TMap<TSubclassOf<AActor>, int32>& enemiesTryToSpawn);

  //Function to obtain how many enemies of each class we have to spawn in the current Wave 
  void NumEnemiesToSpawnInCurrentWave(TMap<TSubclassOf<AActor>, int32>& EnemiesToSpawn);
  void StopAudio();

  TMap<TSubclassOf<AActor>, int32> MaxEnemiesInScreen;

public:
  virtual void BeginDestroy() override;
};
