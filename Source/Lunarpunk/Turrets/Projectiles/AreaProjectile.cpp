// Fill out your copyright notice in the Description page of Project Settings.


#include "AreaProjectile.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Enemies/IEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "HealthSystem/DamageComponent.h"
#include "HealthSystem/HealthComponent.h"
#include "Enemies/AI/EnemyAIController.h"
#include "Turrets/ProjectileTurret.h"
#include "Player/PlayableCharacter.h"
#include "Components/SphereComponent.h"

AAreaProjectile::AAreaProjectile()
{
  ShootEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ShootEffectComponent"));
  ShootEffectComponent->SetupAttachment(CollisionComponent);

  bDisappearAfterFirstHit = false;
}

void AAreaProjectile::BeginPlay()
{
  Super::BeginPlay();
  SetDistanceOfFire(DistanceOfFire);
}

void AAreaProjectile::ShootProjectile(const FVector& ShootLocation, FRotator ShootRotation, const float& Power, const float& MaxShootDistance, AProjectileTurret* Turret, const AActor* Target)
{
  //SetDistanceOfFire(MaxShootDistance);
  Super::ShootProjectile(ShootLocation, ShootRotation, Power, MaxShootDistance);
}

void AAreaProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
  //Do Nothing
}

void AAreaProjectile::ReturnToPool()
{
  ShootEffectComponent->Deactivate();
  Super::ReturnToPool();
}

void AAreaProjectile::Reactivate()
{

  ShootEffectComponent->Activate(true);

  Super::Reactivate();
}

//void AAreaProjectile::SetDistanceOfFire(const float& _DistanceOfFire)
//{
//
//}


