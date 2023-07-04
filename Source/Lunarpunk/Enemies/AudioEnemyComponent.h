// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AudioEnemyComponent.generated.h"

class UHealthComponent;
class UFMODEvent;
class UFMODAudioComponent;
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUNARPUNK_API UAudioEnemyComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UAudioEnemyComponent();

protected:
  // Called when the game starts
  virtual void BeginPlay() override;

public:

  UFUNCTION()
    void PlayDamageReceive();

  UFUNCTION()
    void Initialize(UHealthComponent* HealthComponent);
  // Called every frame
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
  virtual void BeginDestroy() override;

  UPROPERTY(EditAnywhere, Category = "Enemy|Sound")
    UFMODEvent* DamageReceiveAudioEvent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Enemy|Sound")
    UFMODEvent* ActualStepsAudioEvent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Enemy|Sound")
    UFMODEvent* PreviousStepsAudioEvent;

  UPROPERTY(VisibleAnywhere, Category = "Enemy|Sound")
    UFMODAudioComponent* DamageReceiveInstance;

};
