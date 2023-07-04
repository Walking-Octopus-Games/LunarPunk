// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Abilities/AbilityBaseComponent.h"
#include "HealthSystem/DamageComponent.h"
#include "GranadeAbilityComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FThrowGrenade);
/**
 *
 */
UCLASS()
class LUNARPUNK_API UGranadeAbilityComponent : public UAbilityBaseComponent
{
  GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category = "Config.")
        float DamageGranade = 50.0f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UAudioComponent* AudioComponent;

    UPROPERTY(EditAnywhere, Category = "Debug")
        bool bDrawDebugSphere = false;

    UPROPERTY(EditAnywhere, Category = "Debug")
        float DrawTime = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        /*Force that will apply to the enemy when hitted*/
        float ThrustImpactForce = 1000.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        /*Force that will apply to the enemy when hitted*/
        float ThrustImpactForceWall = 0.0;


	virtual void Initialize(ALunarPunkPlayerController* _PlayableController, APlayableCharacter* _PlayableCharacter) override;
  
	virtual void StopAbility() override;

	virtual void ThrowAbility() override;

    virtual void Button2Pressed(bool bIsPressed) override;

    void SpawnNiagaraEffect(FVector ExplosionOriginLocation);

    void DestroyWall();

    //bool AssertThrowAbility();

    UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable)
        FThrowGrenade ThrowGrenadeEvent;


protected:
  UGranadeAbilityComponent();

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config.")
      class UNiagaraSystem* ExplosionEffect;

  UFUNCTION(BlueprintImplementableEvent)
    void ShowGranadePath();

  UFUNCTION(BlueprintImplementableEvent)
    void HideGranadePath();

  UFUNCTION(BlueprintImplementableEvent)
    void ThrowGrenadeAnimation();

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float GranadeDistance = 5;

  UPROPERTY(EditAnywhere, Category = "Granade")
    TSubclassOf<AActor> GranadeClass;

  UPROPERTY(EditAnywhere, Category = "Granade")
      TArray<AActor*> EnemiesInRange;

  UPROPERTY(EditAnywhere, Category = "Granade")
      TArray<AActor*> WallsInRange;

  UPROPERTY(EditAnywhere, Category = "Granade")
      AActor* WallToDestroy;


  UPROPERTY(EditAnywhere)
      UDamageComponent* DamageComponent;
};
