// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Abilities/AbilityBaseComponent.h"
#include "ShieldAbilityComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FThrowShield);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeactivateShield);
/**
 *
 */
UCLASS()
class LUNARPUNK_API UShieldAbilityComponent : public UAbilityBaseComponent
{
  GENERATED_BODY()

public:
  UShieldAbilityComponent();

  virtual void Initialize(ALunarPunkPlayerController* _PlayableController, APlayableCharacter* _PlayableCharacter) override;

  virtual void ThrowAbility() override;

  virtual void StopAbility() override;

  virtual void Button2Pressed(bool bIsPressed) override;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shield")
    float TimeShield = 5.0f;

  FTimerHandle ShieldTimerHandle;


  UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable)
    FThrowShield ThrowShieldEvent;

  UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable)
    FDeactivateShield DeactivateShieldEvent;

private:

  UFUNCTION()
    void ActiveShields();

  UFUNCTION()
    void DeactivateShield();
};
