// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "UObject/Object.h"
#include "Engine/EngineTypes.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActorDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHealthRestored);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateHealth, float, Health);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDamageReceived, float, Damage);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUNARPUNK_API UHealthComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UHealthComponent();

  // Called every frame
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  UFUNCTION(BlueprintCallable, Category = "Health")
    float GetHealth();

  UFUNCTION(BlueprintCallable, Category = "Health")
    void SetHealth(float newHealth);

  UFUNCTION(BlueprintCallable, Category = "Health")
    float GetBaseMaxHealth()
  {
    return BaseMaxHealth;
  }

  UFUNCTION(BlueprintCallable, Category = "Health")
    float GetActualHealth()
  {
    return ActualHealth;
  }

  UFUNCTION(BlueprintCallable, Category = "Health")
    float GetMaxHealth();

  UFUNCTION(BlueprintCallable, Category = "Health")
    void SetMaxHealth(float NewMaxHealth, bool ShouldResetHealth = true);

  UFUNCTION(BlueprintCallable, Category = "Health")
    void IncreaseMaxHealthWithPercentage(float Percentage);

  UFUNCTION(BlueprintCallable, Category = "Health")
    bool CanBeDamaged();

  UFUNCTION(BlueprintCallable, Category = "Health")
    void SetCanBeDamaged(bool Immunity);

  UFUNCTION(BlueprintCallable, Category = "Health")
    void SetGodModeEnabled(bool GodMode);

  UFUNCTION(BlueprintCallable, Category = "Health")
    void RegainHealth(float regainHealth);

  UFUNCTION(BlueprintCallable, Category = "Health")
    void RestoreAllHealth()
  {
    bIsRestoringHealth = true;
    if (ActualHealth < MaxHealth)
    {
      HealthRestored.Broadcast();
    }
  }

  UFUNCTION(BlueprintCallable, Category = "Health")
    void ReceiveDamage(float damage);

  UFUNCTION(BlueprintCallable, Category = "Health")
    void ReceivePercentageDamage(float Percentage);

  UFUNCTION(BlueprintCallable, Category = "Health")
    void InstantKill();

  UFUNCTION(BlueprintCallable, Category = "Health")
    void ResetHealth();

  void ResetComponent();

  //If true, the actor can be damaged. Use to handle the actors death.
  UPROPERTY(EditAnywhere, Category = "Health")
    bool bCanBeDamaged = true;

  //If true, the actor can't be damaged.
  UPROPERTY(EditAnywhere, Category = "Health")
    bool bGodMode = false;

  UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable)
    FActorDeath ActorDeath;

  UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
    FUpdateHealth UpdateHealth;

  UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
    FHealthRestored HealthRestored;

  UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
    FDamageReceived DamageReceived;

protected:
  // Called when the game starts
  virtual void BeginPlay() override;


private:
  UPROPERTY(EditAnywhere, Category = "Health")
    float ActualHealth;


  /* Base Max Health to reset the component when its owner dies to a common default value.*/
  UPROPERTY(EditAnywhere, Category = "Health")
    float BaseMaxHealth = 100.f;

  /*Max Health of the owner in the gameplay. Could be different than the BaseMaxHealth when the actor spawns.*/
  UPROPERTY(EditAnywhere, Category = "Health")
    float MaxHealth = BaseMaxHealth;

  bool bIsRestoringHealth = false;
  float RestoringHealthSpeed = 10.0f;

  /*INITIAL LIFE OF THE ACTOR, ONLY USE IN TESTS*/
  /*UPROPERTY(EditAnywhere, Category = "Health")
    float InitialHealth=BaseMaxHealth;*/





    //UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable)
    //	FPlayerDied PlayerDied;


  //public:
  //	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
  //		void RefreshLifeHUD();
};
