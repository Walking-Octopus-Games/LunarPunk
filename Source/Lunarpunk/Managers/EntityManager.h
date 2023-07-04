// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EntityManager.generated.h"


class ALunarpunkGameModeBase;
class APortal;
class APlayableCharacter;
class ATurret;
class ABasicEnemy;
class AZoneTrigger;
class AEnemySpawner;
/**
 *
 */
UCLASS(Blueprintable)
class LUNARPUNK_API UEntityManager : public UObject
{
  GENERATED_BODY()


public:

  UPROPERTY(VisibleAnywhere)
    class APool* LevelPool;

  UPROPERTY(EditAnywhere)
    TMap<TSubclassOf<AActor>, int32> MaxActorsInScrren;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bDeactivateAI = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bDeactivateVFX = false;

  //Array to store the Conquest Zones in the level
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<AActor*> ConquestZones;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TArray<ATurret*> Turrets;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    APlayableCharacter* PlayerCharacter;

  UPROPERTY()
    TArray<APortal*> Portals;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<ABasicEnemy*> Enemies;

  UPROPERTY()
    TArray<AZoneTrigger*> ZoneTriggers;

  UPROPERTY()
    TArray<AEnemySpawner*> EnemySpawners;


  UEntityManager();

  UFUNCTION()
    void InitEnemySpawners();

  UFUNCTION(BlueprintCallable)
    void Initialize(UWorld* _World, APlayableCharacter* PlayableCharacter, ALunarPunkPlayerController* PlayerController);

  UFUNCTION()
    void InitializeConquestZones();

  UFUNCTION(BlueprintCallable)
    bool IsAnyTurretMoving();

  UFUNCTION()
    void DeactivateComponents(AActor* Actor);

  UFUNCTION(BlueprintCallable)
    void InitNewTurret(ATurret* NewTurret);

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT || UE_EDITOR

  void InitializeProfilling();

  void DeactivateAI(AActor* Actor);

  void DeactivateNiagara(AActor* Actor);

  void DeactivateAllActiveAI();

  void DeactivateAllActiveNiagara();

  void SuscribeToReactivateDelegate();
#endif

  UFUNCTION()
    TArray<APortal*>& GetPortals();

  UFUNCTION(BlueprintCallable)
    void KillAllEnemiesInMap();

  UFUNCTION(BlueprintCallable)
    void ChangeTurretsMovementSpeed(float NewSpeed);

  UFUNCTION(BlueprintCallable)
    float GetTurretsSpeed();

  UFUNCTION(BlueprintCallable)
    float GetTurretsDefaultSpeed();

  UFUNCTION(BlueprintCallable)
    void RestoreTurretsSpeed();

private:

  UPROPERTY()
    UWorld* World;

  float TurretsDefaultSpeed;

  void InitTurrets();

  void InitEnemies();

  void InitPortals();

  void InitZoneTriggers();

};
