// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Turrets/TurretsData.h"
#include "Turrets/ProjectileTurret.h"
#include "Traps/Trap.h"
#include "Delegates/Delegate.h"
#include "RepeaterBeacon.generated.h"


class USphereComponent;
class UCapsuleComponent;
class UStaticMeshComponent;

UENUM()
enum class ERepeaterBeaconState : uint8
{
	Disabled UMETA(DisplayName = "Disabled"), // Beacon is disabled
	Enabled UMETA(DisplayName = "Enabled"), // Beacon is activated
	WaitingSet UMETA(DisplayName = "WaitingSet"), // Beacon is waiting to be setted
};

UENUM()
enum class ERepeaterBeaconType : uint8
{
	None,
	MachineGunTurret, // Beacon is set as a MachineGunTurret
	AreaTurret, // Beacon is set as an AreaTurret
	HealingTurret // Beacon is set as a HealingTurret
};

UCLASS()
class LUNARPUNK_API ARepeaterBeacon : public ATrap
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly)
		TEnumAsByte<ERepeaterBeaconState> BeaconState;

	UPROPERTY(VisibleAnywhere)
		ERepeaterBeaconState State = ERepeaterBeaconState::Disabled;

	UPROPERTY(VisibleAnywhere)
		ERepeaterBeaconType StateType = ERepeaterBeaconType::None;

	// MachineGun Projectile class to spawn.
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AProjectile> MachineGunProjectileClass;
	// Area Projectile class to spawn.
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AProjectile> AreaProjectileClass;

	// Healing Projectile class to spawn.
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AProjectile> HealingProjectileClass;

	UPROPERTY(EditAnywhere, Category = Visual)
		UMaterial* DisabledMaterial;

	UPROPERTY(EditAnywhere, Category = Visual)
		UMaterial* EnabledMaterial;

	UPROPERTY(EditAnywhere, Category = Visual)
		UMaterial* SettedMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Power;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int MaxTimesSetted = 3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int TimesSetted = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int MaxNumShootsMachineGun = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int NumShootsMachineGun = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float TimeForAllMGShoots = 3; // Seconds

	float ShootCooldownTime = TimeForAllMGShoots / MaxNumShootsMachineGun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USphereComponent* SphereSetRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* BeaconMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCapsuleComponent* CapsuleCollision;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Range = 30.0f;

	virtual void BeginPlay() override;

	void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void DoTrapEffect() override;

	UFUNCTION(BlueprintCallable)
		virtual void LaunchTrap() override;

	//virtual void DeactivateTrap() override;

	//virtual void ReactivateTrap() override;

private:

	APool* MyPool;

	TArray<AActor*> Turrets;

	ETurretType CurrentPref;

	AActor* Target;

	TArray<AActor*> PossibleTargets;

	// Preference Queue
	TQueue<ETurretType> PreferenceQueue;

	bool PrefMachineGunTurretAdded = false;
	bool PrefAreaTurretAdded = false;
	bool PrefHealingTurretAdded = false;

	void SetAsTurret(const float& power);

	UPROPERTY(VisibleAnywhere)
		AProjectileTurret* FiringTurret = nullptr;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex
		);
		
	void FireAsMachineGunTurret();
	void FireAsAreaTurret();
	void FireAsHealingTurret();

	void FindTarget();
	bool IsAnythingBetweenTheTurretAndTheTarget(const AActor* EnemyTarget);


	void AskForSetUp();
	void SetBeaconType(AProjectileTurret* Turret, ERepeaterBeaconType Type);

	UFUNCTION()
	void FinishSetUp();
};
