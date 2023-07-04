// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Projectile.h"
#include "AreaProjectile.generated.h"

/**
 *
 */
UCLASS()
class LUNARPUNK_API AAreaProjectile : public AProjectile
{
  GENERATED_BODY()

public:

  AAreaProjectile();

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UNiagaraComponent* ShootEffectComponent;



  virtual void BeginPlay() override;

  virtual void ShootProjectile(const FVector& ShootLocation, FRotator ShootRotation, const float& Power, const float& MaxShootDistance = 100, AProjectileTurret *Turret = nullptr, const AActor* Target = nullptr) override;

  virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) override;

  virtual void ReturnToPool() override;

  virtual void Reactivate() override;

 

  virtual void DoSparkleEffect(AActor* ActorToBeOrigin) {};

private:


};
