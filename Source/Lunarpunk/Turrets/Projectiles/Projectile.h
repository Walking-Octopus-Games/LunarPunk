// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/IEnemy.h"
#include "Delegates/Delegate.h"
#include "GameFramework/Actor.h"
#include "Pool/IsPoolable.h"
#include "Projectile.generated.h"

class UDamageComponent;
class UHealthComponent;
class USphereComponent;
class UProjectileMovementComponent;
class APool;
class USoundBase;
class AProjectileTurret;


UENUM()
enum class EProjectileTargetMode : uint8
{
  Default //Projectile affects to Enemies
  , Enemy //Projectile affects to Player and Turrets
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FProjectileDelegate);

UCLASS()
class LUNARPUNK_API AProjectile : public AActor, public IIsPoolable
{
  GENERATED_BODY()

public:

  AProjectile();
  UPROPERTY()
    float DistanceOfFire;

  UPROPERTY()
    float TimeToReturnProjectileToPool;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trajectory")
    bool bIsParabolicProjectile = false;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Trajectory")
    float LaunchingAngle = 0.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    bool bDisappearAfterFirstHit = true;

  // Force that will apply to the enemy when hit
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    float ThrustImpactForce = 1000.0f;

  UPROPERTY(VisibleAnywhere)
    TArray<AActor*> ActorsHitted;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EProjectileTargetMode TargetMode = EProjectileTargetMode::Default;

  // Sphere collision component
  UPROPERTY(EditAnywhere)
    USphereComponent* CollisionComponent;

  // Projectile movement component
  UPROPERTY(EditAnywhere)
    UProjectileMovementComponent* ProjectileMovementComponent;

  // Projectile mesh
  UPROPERTY(EditAnywhere)
    UStaticMeshComponent* ProjectileMeshComponent;

  // Projectile material
  UPROPERTY(EditAnywhere)
    UMaterialInstanceDynamic* ProjectileMaterialInstance;

  // Projectile material
  UPROPERTY(EditAnywhere)
    UDamageComponent* DamageComponent;

  //Reference to the pool of the game
  UPROPERTY(VisibleAnywhere)
    APool* MyPool;

  UPROPERTY(VisibleAnywhere)
    AProjectileTurret* FiringTurret = nullptr;

  UPROPERTY()
    FProjectileDelegate ReturnPoolEvent;

  // Called every frame
  virtual void Tick(float DeltaTime) override;

  // Function that initializes the projectile's velocity in the shoot direction.
  void FireInDirection(const FVector& ShootDirection);

  UFUNCTION()
    virtual void ShootProjectile(const FVector& ShootLocation, FRotator ShootRotation, const float& Power, const float& MaxShootDistance = 100, AProjectileTurret* Turret = nullptr, const AActor* Target = nullptr);

  UFUNCTION()
    virtual FVector CalculateVelocity(const FVector& ShootLocation, const AActor* Target);

  // Function that is called when the projectile hits something.
  UFUNCTION()
    virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

  UFUNCTION()
    virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

  UFUNCTION()
    virtual void ApplyThrustImpact(AActor* EnemyActor);

  float GetDamageStrength();

  //Functions to handle the in-out of the pool
  virtual void ReturnToPool() override;

  virtual void Deactivate() override;

  virtual void Reactivate() override;

  virtual void SetDistanceOfFire(const float& DistanceOfFire);

  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
  void SpawnImpactEffect(FVector Location);

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;
private:
  /* Handle to manage the timer */
  UPROPERTY()
    FTimerHandle FireTimerHandler;
};
