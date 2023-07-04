// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/BasicEnemy.h"
#include "DistanceEnemy.generated.h"

class USphereComponent;
class UNiagaraComponent;
class APool;
class UFMODEvent;
class UFMODAudioComponent;
/**
 * 
 */
UCLASS()
class LUNARPUNK_API ADistanceEnemy : public ABasicEnemy
{
	GENERATED_BODY()
	


public:


	ADistanceEnemy();
	
	void Initialize(APool* Pool) override;

	//Projectile type to shoot
	UPROPERTY(EditAnywhere, Category = "Config.")
	TSubclassOf<AActor> ProjectileClass;

	//Distance in which the enemy should shot the player instead of the turrets
	UPROPERTY(EditAnywhere, Category = "Config.")
		float SelfDefenseRange = 5.0f;

	//Quantity of damage of the projectile
	UPROPERTY(EditAnywhere, Category = "Config.")
		float ShootingPower = 50.0f;

	//Range of the shot
	UPROPERTY(EditAnywhere, Category = "Config.")
		float ShootingRange = 30.0f;

	//Time to charge the shot once the target is set
	UPROPERTY(EditAnywhere, Category = "Config.")
		float TimeToChargeShot = 3.0f;

	//Resting time of the Enemy after shooting
	UPROPERTY(EditAnywhere, Category = "Config.")
		float RestingTimeAfterShooting = 1.0f;

	//Movement Speed of the projectile
	UPROPERTY(EditAnywhere, Category = "Config.")
		float ProjectileSpeed = 3.0f;

	//Distance to perform the shoot. It should be lower than the shooting range.
	UPROPERTY(EditAnywhere, Category = "Config.")
		float DistanceToPositionToShoot = 18.0f;

	UPROPERTY(EditAnywhere, Category = "Config.", Meta = (Tooltip = "If false, the actor will try to avoid to shoot through the walls"))
		bool bShootThroughWalls = true;

	//Distance the enemy will move after shooting.
	UPROPERTY(EditAnywhere, Category = "Config.")
		float DistanceToMoveAfterShooting = 5.0f;

	//If the actor can't shoot, then it will move nearer to the target when perform the next shot.
	UPROPERTY(EditAnywhere, Category = "Config.")
		float PercentageModifierInDistanceToPositionToShot = 0.75f;

	UPROPERTY(VisibleAnywhere, Category = "Config.")
		float CurrentDistanceToShootPercentageModifier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Debug And Performance")
		bool bDrawDebugSpheresForSafePoingMovement = false;

	UPROPERTY(EditAnywhere, Category = "Debug And Performance")
		bool bDrawDebugSpheresForShootLocationReference = false;

	UPROPERTY(EditAnywhere, Category = "Debug And Performance")
		float DrawDuration = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Debug And Performance")
		int32 NumRowsSafePoints = 5;

	UPROPERTY(EditAnywhere, Category = "Debug And Performance")
		int32 NumColumnsSafePoints = 5;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Sound")
		USoundBase* ShotSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Sound")
		float VolumeShotSound = 0.5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Sound")
		USoundBase* ChargingShotSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Sound")
		float VolumeChargingShotSound = 0.5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Sound")
		UAudioComponent* AudioComponent;


  UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Enemy|Sound")
    UFMODEvent* Shot;

  UPROPERTY(VisibleAnywhere, Category = "Enemy|Sound")
    UFMODAudioComponent* ShotInstance;
	//USphereComponent* OwnArea;

	UNiagaraComponent* ShotChargeEffect;

	APool* MyPool;


	//class USphereComponent* SafePoint;
	//TArray<class ATurret> InTurretRange;

	//Handles the shoot
	void Shoot();


	void ActivateShotChargeEffect();

	void DeactivateShotChargeEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateAnimChargingState(bool IsCharging);

	UFUNCTION(BlueprintImplementableEvent)
	void MontageShotAnimation();

	virtual void BeginPlay() override;

	virtual void ReturnToPool() override;

	virtual void DeactivateForDeathAnim() override;

	void PlayShotSound();

	void PlayChargingShotSound();

	void StopChargingShotSound();

	FVector GetShootLocation();
private:

	//World point in which the projectile will be generated.
	USphereComponent* ShootLocation;


};
