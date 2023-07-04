// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FMODAudioComponent.h"
#include "FMODBlueprintStatics.h"

#include "Components/ActorComponent.h"
#include "RewardsAndUpgrades/Upgrade.h"

#include "AudioTurretComponent.generated.h"

class ALunarPunkGameMode;
class AAudioManager;
class ATurret;
class UFMODEvent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUNARPUNK_API UAudioTurretComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UAudioTurretComponent();

protected:
  // Called when the game starts
  virtual void BeginPlay() override;

public:
  // Called every frame
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


  UFUNCTION()
    void PlayTurretMovementeEvent();

  UFUNCTION()
    void StopTurretMovementeEvent();

  UFUNCTION()
    void Initialize();

  UPROPERTY(EditAnywhere, Category = "Turret|Sound")
    UFMODEvent* ShootAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Turret|Sound")
    UFMODEvent* RepairAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Turret|Sound")
    UFMODEvent* PickUpAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Turret|Sound")
    UFMODEvent* DamageAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Turret|Sound")
    UFMODEvent* ReactivateAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Turret|Sound")
    UFMODEvent* DeathAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Turret|Sound")
    UFMODEvent* LeaveTurretAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Turret|Sound")
    UFMODEvent* CadenceUpgradeAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Turret|Sound")
    UFMODEvent* DamageUpgradeAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Turret|Sound")
    UFMODEvent* HealthUpgradeAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Turret|Sound")
    UFMODEvent* RangeUpgradeAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Turret|Sound")
    UFMODEvent* TurretMovementEvent;

  UPROPERTY()
    ALunarPunkGameMode* GameMode;

  UPROPERTY()
    AAudioManager* AudioManager;

  UPROPERTY()
    ATurret* TurretOwner;

  UPROPERTY()
    UFMODAudioComponent* ReactivateInstance;

  UPROPERTY()
    UFMODAudioComponent* DeathAudioInstance;

  UPROPERTY()
    UFMODAudioComponent* LeaveTurretInstance;

  UPROPERTY()
    UFMODAudioComponent* ReactivateEventInstance;

  UPROPERTY()
    UFMODAudioComponent* DamageEventInstance;

  UPROPERTY()
    UFMODAudioComponent* PickUpEventInstance;

  UPROPERTY()
    UFMODAudioComponent* UpgradeAddedAudioInstance;

  UPROPERTY()
    UFMODAudioComponent* RepairInstance;

  UPROPERTY()
    UFMODAudioComponent* MovementInstance;

  UFUNCTION()
    void PlayShootEvent();

  UFUNCTION()
    void PlayRepairEvent();

  UFUNCTION()
    void StopRepairEvent();

  UFUNCTION()
    void PlayPickUpEvent();

  UFUNCTION()
    void PlayDamageEvent(float Damage);

  UFUNCTION()
    void PlayReactivateEvent();

  UFUNCTION()
    void PlayLeaveTurretEvent();

  UFUNCTION()
    void PlayDeathTurretEvent();
  void StopUpgradeAudio();

  UFUNCTION()
    void PlayUpgradeAddedEvent(EUpgradeType UpgradeType);

  void StopAudio();

  void PauseAudio();

  void RestartAudio();

  virtual void BeginDestroy() override;
};
