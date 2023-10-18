// Fill out your copyright notice in the Description page of Project Settings.


#include "Turrets/Projectiles/HealingProjectile.h"

#include "FMODBlueprintStatics.h"
#include "Pool/Pool.h"
#include "Enemies/IEnemy.h"
#include "Enemies/BasicEnemy.h"
#include "Enemies/DistanceEnemy.h"
#include "Components/SphereComponent.h"
#include "HealthSystem/DamageComponent.h"
#include "HealthSystem/HealthComponent.h"
#include "Turrets/HealingTurret.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

AHealingProjectile::AHealingProjectile()
{
  DamageZone = CreateDefaultSubobject<USphereComponent>(TEXT("DamageZone"));
  DamageZone->InitSphereRadius(400.0f);
  DamageZone->OnComponentHit.AddDynamic(this, &AHealingProjectile::OnHit);
  DamageZone->SetupAttachment(CollisionComponent);

  DamageZone->SetSimulatePhysics(false);

  //Simulate generate hit events
  DamageZone->SetNotifyRigidBodyCollision(false);
  DamageZone->SetGenerateOverlapEvents(false);

  ShootEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ShootEffectComponent"));
  ShootEffectComponent->SetupAttachment(DamageZone);
  DamageZone->SetupAttachment(CollisionComponent);
  ShootEffectComponent->SetupAttachment(DamageZone);
}

void AHealingProjectile::BeginPlay()
{
  Super::BeginPlay();

  DamageZone->SetRelativeLocation(CollisionComponent->GetRelativeLocation());
  ShootEffectComponent->SetRelativeLocation(CollisionComponent->GetRelativeLocation());

  DamageZone->Deactivate();
}

void AHealingProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
  //ShootEffectComponent->Activate();
  ShowShootEffect();
  GetOverlappingActors(OverlapedActors, ABasicEnemy::StaticClass());

  if (OverlapedActors.Num() > 0)
  {
    //Damage Enemies
    Cast<AHealingTurret>(FiringTurret)->RestartHealingEffect();
    for (AActor* Actor : OverlapedActors)
    {
      ABasicEnemy* Enemy = Cast<ABasicEnemy>(Actor);
      if (Actor->IsA(ADistanceEnemy::StaticClass()))
      {
        float currentDamage = DamageComponent->DamageStrength;
        float* DamageValue = DamageToEnemies.Find(ADistanceEnemy::StaticClass());
        if (DamageValue != nullptr)
        {
          DamageComponent->SetDamage(*DamageValue, *DamageValue);
        }
        DamageComponent->MakeDamage(Enemy->GetHealthComponent());
        if (IsValid(Super::FiringTurret))
        {
          Super::FiringTurret->TurretMakeDamage.Broadcast(FiringTurret->TypeOfTurret, DamageComponent->DamageStrength);
        }
        
        DamageComponent->SetDamage(currentDamage, currentDamage);
        

      }
      else
      {
        float currentDamage = DamageComponent->DamageStrength;
        float* DamageValue = DamageToEnemies.Find(ABasicEnemy::StaticClass());
        if (DamageValue != nullptr)
        {
          DamageComponent->SetDamage(*DamageValue, *DamageValue);
        }
        DamageComponent->MakeDamage(Enemy->GetHealthComponent());
        if (IsValid(Super::FiringTurret))
        {
          Super::FiringTurret->TurretMakeDamage.Broadcast(FiringTurret->TypeOfTurret, DamageComponent->DamageStrength);
        }
        DamageComponent->SetDamage(currentDamage, currentDamage);
      }
    }

    //Heal Turrets
    AHealingTurret* Turret = Cast<AHealingTurret>(FiringTurret);
    if (IsValid(Turret))
    {
      Turret->HealAdjacentTurrets();
    }
  }

  HittedInstance = UFMODBlueprintStatics::PlayEventAttached(HittedAudioEvent, GetRootComponent(), NAME_None, GetActorLocation(), EAttachLocation::KeepWorldPosition, true, true, true);

  // Return projectile to the pool
  ReturnToPool();
}

void AHealingProjectile::ReturnToPool()
{
  ShootEffectComponent->Deactivate();

  DamageZone->SetNotifyRigidBodyCollision(false);
  DamageZone->SetGenerateOverlapEvents(false);
  DamageZone->SetCollisionEnabled(ECollisionEnabled::NoCollision);

  Super::ReturnToPool();
}

void AHealingProjectile::Reactivate()
{
  Super::Reactivate();

  DamageZone->SetRelativeLocation(ProjectileMeshComponent->GetRelativeLocation());
  DamageZone->SetNotifyRigidBodyCollision(true);
  DamageZone->SetGenerateOverlapEvents(true);
  DamageZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

  ShootEffectComponent->SetRelativeLocation(ProjectileMeshComponent->GetRelativeLocation());
  DamageZone->Activate();
}

void AHealingProjectile::SetDistanceOfFire(const float& _DistanceOfFire)
{
  //Super::SetDistanceOfFire(DistanceOfFire);

  // 2 * velocidad * sen angulodelanzamiento / g

  this->DistanceOfFire = _DistanceOfFire;
  float angleRad = (LaunchingAngle * PI / 180);

  TimeToReturnProjectileToPool = 2 * ProjectileMovementComponent->MaxSpeed * FMath::Sin(angleRad) / (-1 * GetWorld()->GetGravityZ());

}

void AHealingProjectile::BeginDestroy()
{
  if (IsValid(HittedInstance) && HittedInstance->IsPlaying())
  {
    HittedInstance->Stop();
    HittedInstance->Release();
  }

  Super::BeginDestroy();
}

void AHealingProjectile::ShowShootEffect()
{
  if (IsValid(ShootEffectComponent) && ShootEffectComponent->GetAsset())
  {
    UNiagaraComponent* ShootEffectComponentCopy = ShootEffectComponent;
    ShootEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ShootEffectComponent->GetAsset(), GetActorLocation(), FRotator(0,0,0));
    ShootEffectComponent = ShootEffectComponentCopy;
  }
}
