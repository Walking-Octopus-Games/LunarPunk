// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Abilities/AbilityBaseComponent.h"
#include "ShootAbilityComponent.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerShooting);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerMakeDamage, float, damage);

/**
 *
 */
UCLASS()
class LUNARPUNK_API UShootAbilityComponent : public UAbilityBaseComponent
{
  GENERATED_BODY()

public:
  UShootAbilityComponent();

 
  //Shoot sound
  UPROPERTY(EditAnywhere, category = "Sound & Effects | Sound | Shot")
    USoundBase* ShootSound;

  //Hit effect spawn when Shoot hit shomething
  UPROPERTY(EditAnywhere, category = "Sound & Effects | Effects | Shot")
    UNiagaraSystem* HitEffect;
  
  UPROPERTY(EditAnywhere, category = "Sound & Effects | Effects | Shot")
    FLinearColor Color;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Shot")
    float ShotRadius = 8.0f;

  UPROPERTY(EditAnywhere, category = "Upgrades")
    bool bMultipleShot = false;

  UPROPERTY(EditAnywhere, category = "Player")
    float MinShootTimeRange = 0.0f;

  UPROPERTY(EditAnywhere, category = "Player")
    float  MaxShootTimeRange = 0.0f;

  UPROPERTY(VisibleAnywhere, category = "Player")
    float ActualShootTime = 0.0f;

  UPROPERTY(EditAnywhere, category = "Player")
    bool IsPressed = false;

  UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable, Category = "Player Shooting")
    FPlayerShooting PlayerShooting;

  UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable, Category = "Player Shooting")
    FPlayerShooting PlayerStopShooting;

  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
      void PlayShootAnimation();

  

  UPROPERTY()
    FPlayerMakeDamage PlayerMakeDamage;


  void StartFireRateTimer();

  virtual void Initialize(ALunarPunkPlayerController* _PlayableController, APlayableCharacter* _PlayableCharacter) override;

  virtual void StopAbility() override;

  virtual void ThrowAbility() override;

  void DisableAbility();

  void EnableAbility();

protected:

  void ShootCooldown();
  
  UFUNCTION()
    void Shoot(bool _IsPressed);

private:

  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
 
  // Function to get vector direction of shoot 
  UFUNCTION()
    static FVector GetShootDirection(const FRotator ActualRotation, const FRotator Rotation);

  UFUNCTION()
    void ShowShootEffectComponent(FVector ShootDirection, FVector LineStart, FHitResult Hit);

  UFUNCTION()
    void ShowHitEffect(FVector ShootDirection, FVector LineStart, FHitResult Hit) const;

  UFUNCTION()
    void ActionsTriggeredByHit(const FHitResult& Hit) const;

  // Function to verify shoot impact and 
  UFUNCTION()
    void ShootImpactCheck(FVector ShootDirection);

  FTimerHandle TimerHandler;

  UPROPERTY()
    bool bCanShoot;
};
