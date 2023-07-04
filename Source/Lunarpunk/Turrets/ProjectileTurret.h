// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Turrets/Turret.h"
#include "RewardsAndUpgrades/UpgradesData.h"
#include "ProjectileTurret.generated.h"

class UBoxComponent;
class APool;
class USphereComponent;
class USparkUpgradeComponent;

/**
 *
 */
UCLASS()
class LUNARPUNK_API AProjectileTurret : public ATurret
{
  GENERATED_BODY()

public:
  AProjectileTurret();

  // Projectile class to spawn.
  UPROPERTY(EditDefaultsOnly, Category = "Projectile | Type")
    TSubclassOf<class AProjectile> ProjectileClass;

  // Projectile class to spawn.
  UPROPERTY(EditDefaultsOnly, Category = "Projectile | Type")
    TSubclassOf<class AProjectile> BeaconProjectile;

  UPROPERTY(BlueprintReadWrite, EditAnyWhere, Category = "Projectile | Attack")
    float FireRate = 1.0f;

  UPROPERTY(EditAnywhere, category = "Projectile | Attack")
    float MinShootTimeRange = 2.0f;

  UPROPERTY(EditAnywhere, category = "Projectile | Attack")
    float  MaxShootTimeRange = 2.5f;

  //UPROPERTY(VisibleAnywhere, category = "Projectile | Attack")
  //  float ActualShootTime = 0.0f;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile | Attack")
    float Power;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Projectile | Attack")
    float Range = 30.0f;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
    AActor* Target;

  // Reference to the pool of the game
  UPROPERTY(VisibleAnywhere)
    APool* MyPool;

  // This variable should be removed once the meshes are fixed and in the correct rotation
  UPROPERTY()
    FRotator RotationCorrection;




  // Called every frame
  virtual void Tick(float DeltaTime);

  UFUNCTION(BlueprintCallable, Category = "ProjectileTurret")
    void FindTarget();

  UFUNCTION(BlueprintCallable, Category = "ProjectileTurret")
    void RotateTurret();

  // Attempt to fire a projectile
  UFUNCTION(BlueprintCallable, Category = "ProjectileTurret")
    void Fire();

  // Attempt to fire a projectile
  UFUNCTION(BlueprintCallable, Category = "ProjectileTurret")
    bool CheckIfTargetIsValid();

  UFUNCTION(BlueprintCallable, Category = "ProjectileTurret")
    void SetProjectile(TSubclassOf<class AProjectile> NewProjectileClass)
  {
    ProjectileClass = NewProjectileClass;
  }

  UFUNCTION(BlueprintCallable, Category = "ProjectileTurret")
    void SetRepeaterBeacon(AActor* RepeaterBeacon);

  void UpgradeTurret(FUpgradesDataStruct* Upgrade, int Quantity) override;

  float GetIncreaseOfUpgrade(FUpgradesDataStruct Upgrade, int AddNext);

  void InitializeUpgradeInfo() override;

  void UpdateUpgradesInfo(FUpgradesDataStruct Upgrade, int Quantity) override;

  virtual float GetRange() const override
  {
    return Range;
  }

protected:

  UPROPERTY()
    TArray<AActor*> PossibleTargets;

  UPROPERTY(EditDefaultsOnly, Category = "ProjectileTurret")
    class UStaticMeshComponent* WeaponMesh;

  UPROPERTY(EditDefaultsOnly, Category = "ProjectileTurret")
    class UStaticMeshComponent* BarrelMesh;

  // Handle to manage the timer
  UPROPERTY()
    FTimerHandle FireTimerHandler;

  //Variable to store the class of the range nav area turret.
  TSubclassOf<UNavAreaBase> NavAreaRangeTurret;


  // Called when the game starts or when spawned
  UFUNCTION()
    virtual void BeginPlay();

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

  virtual void StopTurret() override;

  virtual void StartTurret() override;

  void RestartTimer();

  bool IsAnythingBetweenTheTurretAndTheTarget(const AActor* Target);

  void UpdateFireRate();
};


