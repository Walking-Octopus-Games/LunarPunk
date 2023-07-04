// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Turrets/ProjectileTurret.h"
#include "RewardsAndUpgrades/Upgrade.h"
#include "HealingTurret.generated.h"

class UBoxComponent;
class APool;
class USphereComponent;
class USparkUpgradeComponent;
class UParticleSystemComponent;
struct FPredictProjectilePathResult;

/**
 * 
 */
UCLASS()
class LUNARPUNK_API AHealingTurret : public AProjectileTurret
{
	GENERATED_BODY()
	
public:
  AHealingTurret();

  UPROPERTY(VisibleAnywhere, Category = "HealingTurret | Effects")
    USparkUpgradeComponent* SparksUpgradeComponent;

  UPROPERTY(EditAnywhere, Category = "HealingTurret | Healing")
    float HealingPercent;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HealingTurret | Healing")
    USphereComponent* HealingZone;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HealingTurret | Healing")
    UParticleSystemComponent* HealingEffect;

  // Called every frame
  virtual void Tick(float DeltaTime);

  //Funtion to health the adjacent turrets
  void HealAdjacentTurrets();

  void UpgradeTurret(FUpgradesDataStruct* Upgrade, int Quantity) override;

  float GetIncreaseHealing(FUpgradesDataStruct Upgrade, int AddNext);

  void RestartHealingEffect();

  /* Handle to manage the timer */
  UPROPERTY()
    FTimerHandle HealingEffectTimer;


protected:
  // Called when the game starts or when spawned
    virtual void BeginPlay();

private:

  

  virtual void StopTurret() override;

 
  void DeactivateHealingEffect();
};
