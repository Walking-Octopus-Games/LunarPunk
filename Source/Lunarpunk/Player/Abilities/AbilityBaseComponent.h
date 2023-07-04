// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Managers/AbilitiesManger.h"
#include "AbilityBaseComponent.generated.h"

class UNiagaraComponent;
class UAbilitiesManger;
class ALunarPunkGameMode;
class APlayableCharacter;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAbilityNotThrown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityThrowed, EAbilityType, AbilityType);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUNARPUNK_API UAbilityBaseComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UAbilityBaseComponent();

  virtual void Initialize(ALunarPunkPlayerController* _PlayableController, APlayableCharacter* _PlayableCharacter);

  UFUNCTION()
    virtual void ThrowAbility();

  UFUNCTION()
    virtual void StopAbility();

  UFUNCTION()
    virtual void Button2Pressed(bool bIsPressed);

  UFUNCTION()
    bool AssertThrowAbility();

  UPROPERTY()
    FAbilityNotThrown AbilityNotThrown;

  //Event when a trap is lunched
  UPROPERTY()
    FAbilityThrowed AbilityThrowed;

protected:
  // Called when the game starts
  virtual void BeginPlay() override;

  void ShowRayEffect(UNiagaraComponent* NiagaraEffectComponent);

  UPROPERTY()
    EAbilityType AbilityType;

  UPROPERTY()
    UAbilitiesManger* AbilitiesManager;

  UPROPERTY()
    ALunarPunkGameMode* GameMode;

  UPROPERTY()
    APlayableCharacter* PlayableCharacter;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Ability")
    int CostAbility = 10;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Ability")
    float TraceRange = 5000.0f;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    float TimeToAbility = 2.0f;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    FTimerHandle AbilityTimerHandle;

  UPROPERTY(VisibleAnywhere)
    bool bButton2Pressed;

public:
  // Called every frame
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
