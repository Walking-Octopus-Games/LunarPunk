// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/DistanceEnemy.h"

#include "FMODBlueprintStatics.h"
#include "Kismet/GameplayStatics.h"
#include "Pool/Pool.h"
#include "Turrets/Projectiles/AreaProjectile.h"
#include "Turrets/Projectiles/Projectile.h"
#include "Turrets/Projectiles/AreaProjectile.h"
#include "Components/SphereComponent.h"
#include "HealthSystem/DamageComponent.h"
#include "AI/DistanceEnemy/DistanceEnemyAIController.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"



ADistanceEnemy::ADistanceEnemy()
{

  PrimaryActorTick.bCanEverTick = true;
  //Create audio component.
  AudioComponent = CreateDefaultSubobject<UAudioComponent>(FName("Audio Component"));

}


void ADistanceEnemy::BeginPlay()
{
  Super::BeginPlay();


  //AICharacterController = Cast<ADistanceEnemyAIController>(GetController());

  ShootLocation = Cast<USphereComponent>(GetDefaultSubobjectByName(TEXT("ShootLocation")));

  /*OwnArea = Cast<USphereComponent>(GetDefaultSubobjectByName(TEXT("OwnArea")));
  if (OwnArea)
  {
      OwnArea->SetCanEverAffectNavigation(false);
  }*/

  ShotChargeEffect = Cast<UNiagaraComponent>(GetDefaultSubobjectByName(TEXT("ShotChargeEffect")));

  if (ShotChargeEffect)
  {
    ShotChargeEffect->DeactivateImmediate();
  }

}



void ADistanceEnemy::Initialize(APool* Pool)
{
  Super::Initialize(Pool);

  MyPool = Pool;
  if (!IsValid(MyPool))
  {
    UE_LOG(LogTemp, Warning, TEXT("DistanceEnemy.cpp: No se encontro la Pool"));
  }

}


void ADistanceEnemy::Shoot()
{
  //Gets the next projectile of the pool
  if (!MyPool)
  {
    return;
  }

  AActor* Projectile = MyPool->GetNextActor(ProjectileClass);

  if (IsValid(Projectile))
  {
    //Sets location and rotation.
    if (ShootLocation)
    {
      AProjectile* ProjectileCasted = Cast<AProjectile>(Projectile);
      if (IsValid(ProjectileCasted))
      {

        ProjectileCasted->TargetMode = EProjectileTargetMode::Enemy;

        Projectile->SetActorLocation(GetShootLocation());
        Projectile->SetActorRotation(ShootLocation->GetComponentRotation());

        if (Projectile->IsA(AAreaProjectile::StaticClass()))
        {
          AAreaProjectile* Pr = Cast<AAreaProjectile>(Projectile);
          ProjectileCasted->ProjectileMovementComponent->MaxSpeed = 1000 * ProjectileSpeed;
          ProjectileCasted->ProjectileMovementComponent->InitialSpeed = 1000 * ProjectileSpeed;
          Pr->SetDistanceOfFire(100 * ShootingRange);
        }

        //Reactivate the projectile
        if (Projectile->Implements<UIsPoolable>())
        {
          Cast<IIsPoolable>(Projectile)->Reactivate();
        }

        //Shoot the projectile
        // Set the projectile's initial trajectory.
        ProjectileCasted->TargetMode = EProjectileTargetMode::Enemy;
        ProjectileCasted->DamageComponent->SetDamage(ShootingPower, ShootingPower);
        FVector LaunchDirection = ShootLocation->GetComponentRotation().Vector();
        ProjectileCasted->FireInDirection(LaunchDirection);

        StopChargingShotSound();
        PlayShotSound();
        MontageShotAnimation();

      }
    }
  }
}


void ADistanceEnemy::ActivateShotChargeEffect()
{
  if (ShotChargeEffect)
  {
    ShotChargeEffect->ActivateSystem();
  }

  UpdateAnimChargingState(true);
  PlayChargingShotSound();

}

void ADistanceEnemy::DeactivateShotChargeEffect()
{

  if (ShotChargeEffect)
  {
    ShotChargeEffect->DeactivateImmediate();
  }
  UpdateAnimChargingState(false);
  StopChargingShotSound();
}


void ADistanceEnemy::ReturnToPool()
{
  Super::ReturnToPool();

  DeactivateShotChargeEffect();
  //OwnArea->SetCanEverAffectNavigation(false);
}

void ADistanceEnemy::DeactivateForDeathAnim()
{

  DeactivateShotChargeEffect();
  Super::DeactivateForDeathAnim();

}


void ADistanceEnemy::PlayShotSound()
{
  ShotInstance = UFMODBlueprintStatics::PlayEventAttached(Shot, GetRootComponent(), NAME_None, GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
}


void ADistanceEnemy::PlayChargingShotSound()
{
  if (AudioComponent && ChargingShotSound)
  {
    AudioComponent->SetSound(ChargingShotSound);
    AudioComponent->SetVolumeMultiplier(VolumeChargingShotSound);
    AudioComponent->Play();
  }
}


void ADistanceEnemy::StopChargingShotSound()
{
  if (AudioComponent)
  {
    AudioComponent->Stop();
  }
}


FVector ADistanceEnemy::GetShootLocation()
{
  return ShootLocation->GetComponentLocation();
}