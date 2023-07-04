// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Abilities/AbilityBaseComponent.h"
#include "DestroyPortalAbilityComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSetAvailableToDestroyPortal, bool, Available, float, maxTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDestroyPortal);

class APortal;
class UNiagaraComponent;
/**
 *
 */
UCLASS()
class LUNARPUNK_API UDestroyPortalAbilityComponent : public UAbilityBaseComponent
{
  GENERATED_BODY()

public:
  UDestroyPortalAbilityComponent();


  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config|Shake Effect")
    float UpDownShakeSpeed = 0.2;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config|Shake Effect")
    float RightLeftShakeSpeed = 0.1;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config|Shake Effect")
    float UpDownAngle = 1.0;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config|Shake Effect")
    float RightLeftAngle = 1.0;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config|Shake Effect")
    bool bShouldVibrate = true;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Destroy Portal")
    int IndexPortalToDestroy = 0;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Destroy Portal")
    float timeToDestroyPortal = 5.0;

  UPROPERTY()
    APortal* PortalToDestroy = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable, Category = "Destroy Portal")
    FSetAvailableToDestroyPortal SetAvailableToDestroyPortal;

  UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable, Category = "Destroy Portal")
    FDestroyPortal DestroyPortalEvent;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Destroy Portal")
    FTimerHandle DestroyPortalTimerHandle;

  virtual void Initialize(ALunarPunkPlayerController* _PlayableController, APlayableCharacter* _PlayableCharacter) override;

  virtual void StopAbility() override;

  virtual void ThrowAbility() override;

  virtual void Button2Pressed(bool bIsPressed) override;

  UFUNCTION()
    void ResetDestroyPortal();

protected:
  UFUNCTION()
    void Button1Pressed(bool bIsPressed);

  UPROPERTY(VisibleAnywhere)
    bool bButton1Pressed = false;

private:
  void StartDestroyPortal();

  void DestroyPortal();

  void CheckIfAbilityStart();
};
