// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FMODAudioComponent.h"
#include "FMODBlueprintStatics.h"

#include "Components/ActorComponent.h"
#include "AudioPlayerComponent.generated.h"

class ALunarPunkPlayerController;
class UFMODEvent;
class APlayableCharacter;
class UFMODAudioComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUNARPUNK_API UAudioPlayerComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UAudioPlayerComponent();

protected:
  // Called when the game starts
  virtual void BeginPlay() override;

public:
  // Called every frame
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  UFUNCTION()
    void PlayEndSuperShootEvent();

  UFUNCTION()
    void PlaySparkleAudio();

  UFUNCTION()
    void PlayGrenadaAudio();

  UFUNCTION()
    void PlayShieldAudio();

  UFUNCTION()
    void DeactivateShieldAudio();

  UFUNCTION()
    void PlayAbilityNotAvailable();

  UFUNCTION()
    void PlayHealthResetEvent();

  UFUNCTION()
    void PlayDeathEvent();

  UFUNCTION()
    void Initialize(ALunarPunkPlayerController* LunarPunkPlayerController);

  UFUNCTION()
    void PlayDamageEvent(float Damage);

  UFUNCTION()
    void PlayShootEvent();

  UFUNCTION()
    void PlaySuperShootEvent(bool bIsAvailable, float MaxTime);

  UFUNCTION()
    void PlayStopMovingEvent();

  //UFUNCTION()
  //  void PlayIsMovingEvent();

  void StopAudio();

  void PauseAudio();

  void RestartAudio();

  virtual void BeginDestroy() override;

  UPROPERTY()
    APlayableCharacter* PlayableCharacter;

  UPROPERTY(EditAnywhere, Category = "Player|Sound")
    UFMODEvent* ShootAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Player|Sound")
    UFMODEvent* SuperShotAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Player|Sound")
    UFMODEvent* EndSuperShotAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Player|Sound")
    UFMODEvent* DamageAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Player|Sound")
    UFMODEvent* SparkleAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Player|Sound")
    UFMODEvent* GrenadeAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Player|Sound")
    UFMODEvent* ShieldAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Player|Sound")
    UFMODEvent* AbilityNotAvailableAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Player|Sound")
    UFMODEvent* ResetHealthAudioEvent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Sound")
    UFMODEvent* ActualAudioStepsEvent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Sound")
    UFMODEvent* PreviousStepsAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Player|Sound")
    UFMODEvent* PlayerDeathAudioEvent;

  UPROPERTY()
    UFMODAudioComponent* ShootAudioInstance;

  UPROPERTY()
    UFMODAudioComponent* SuperShotAudioInstance;

  UPROPERTY()
    UFMODAudioComponent* PlayerStepsAudioInstance;

  UPROPERTY()
    UFMODAudioComponent* EndSuperShotInstance;

  UPROPERTY()
    UFMODAudioComponent* DamageReceiveInstance;

  UPROPERTY()
    UFMODAudioComponent* ShieldAudioInstance;

  UPROPERTY()
    UFMODAudioComponent* GrenadeAudioInstance;

  UPROPERTY()
    UFMODAudioComponent* SparkleAudioInstance;

  UPROPERTY()
    UFMODAudioComponent* AbilityNotAvailableAudioInstance;
  UPROPERTY()
    UFMODAudioComponent* ResetHealthAudioInstance;

  UPROPERTY()
    UFMODAudioComponent* PlayerDeathAudioInstance;
};
