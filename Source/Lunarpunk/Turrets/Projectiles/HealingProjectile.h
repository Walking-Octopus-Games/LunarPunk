// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Turrets/Projectiles/Projectile.h"
#include "HealingProjectile.generated.h"




class USphereComponent;
class UNiagaraComponent;
class UFMODAudioComponent;

UCLASS()
class LUNARPUNK_API AHealingProjectile : public AProjectile
{
  GENERATED_BODY()

public:
  AHealingProjectile();

  //Enemy Damage Array 
  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile | Attack")
    TMap<TSubclassOf<class ABasicEnemy>, float> DamageToEnemies;

  UPROPERTY(EditAnywhere)
    USphereComponent* DamageZone;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UNiagaraComponent* ShootEffectComponent;

  virtual void BeginPlay() override;

  virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) override;

  virtual void ReturnToPool() override;

  virtual void Reactivate() override;

  virtual void SetDistanceOfFire(const float& DistanceOfFire) override;
  virtual void BeginDestroy() override;

  UPROPERTY(EditAnywhere, Category = "Turret|Sound")
    class UFMODEvent* HittedAudioEvent;

  UPROPERTY()
    UFMODAudioComponent* HittedInstance;
private:

  TArray<AActor*> OverlapedActors;
};
