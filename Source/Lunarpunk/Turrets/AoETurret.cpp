// Fill out your copyright notice in the Description page of Project Settings.

#include "AoETurret.h"
#include "HealthSystem/HealthComponent.h"
#include "HealthSystem/DamageComponent.h"
#include "HealthSystem/VampirismComponent.h"
#include "Player/PlayableCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Enemies/IEnemy.h"

AAoETurret::AAoETurret()
{
  DamageComponent = CreateDefaultSubobject<UDamageComponent>(FName("Damage"));

  VampirismComponent = CreateDefaultSubobject<UVampirismComponent>(FName("Vampirism"));

  RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
}

void AAoETurret::BeginPlay()
{

  APlayableCharacter* Player = Cast<APlayableCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
  if (IsValid(Player) && IsValid(Player->GetHealthComponent()))
  {
    VampirismComponent->HealthComponentToRestore = Player->GetHealthComponent();
  }

  AoECollision = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("AoECollision")));
  if (IsValid(AoECollision))
  {
    AoECollision->OnComponentBeginOverlap.AddDynamic(this, &AAoETurret::OnOverlapBegin);
    AoECollision->OnComponentEndOverlap.AddDynamic(this, &AAoETurret::OnOverlapEnd);
  }

  FireComponent = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("Flames")));

  ATurret::BeginPlay();
}

void AAoETurret::Tick(float DeltaTime)
{
  ATurret::Tick(DeltaTime);
}

void AAoETurret::StartTurret()
{
  if (IsValid(RotatingMovement))
  {
    RotatingMovement->Activate();
  }

  if (IsValid(FireComponent))
  {
    FireComponent->SetVisibility(true);
  }
}

void AAoETurret::StopTurret()
{
  if (IsValid(RotatingMovement))
  {
    RotatingMovement->Deactivate();
  }

  if (IsValid(FireComponent))
  {
    FireComponent->SetVisibility(false);
  }

  if (IsValid(AoECollision))
  {
    TArray<AActor*> Actors;
    AoECollision->GetOverlappingActors(Actors);
    for (AActor* TargetActor : Actors)
    {
      IEnemy* Enemy = Cast<IEnemy>(TargetActor);

      if (Enemy && IsValid(Enemy->GetHealthComponent()) && IsValid(DamageComponent))
      {
        DamageComponent->StopDamagePerSecond(Enemy->GetHealthComponent());
      }
    }
  }
}

void AAoETurret::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent
  , AActor* OtherActor
  , UPrimitiveComponent* OtherComp
  , int32 OtherBodyIndex
  , bool bFromSweep
  , const FHitResult& SweepResult
)
{
  if (State != ETurretState::Set)
  {
    return;
  }

  IEnemy* Enemy = Cast<IEnemy>(OtherActor);
  if (!Enemy || !IsValid(DamageComponent) || !IsValid(Enemy->GetHealthComponent()))
  {
    return;
  }

  DamageComponent->MakeDamagePerSecond(Enemy->GetHealthComponent());
}

void AAoETurret::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent
  , AActor* OtherActor
  , UPrimitiveComponent* OtherComp
  , int32 OtherBodyIndex
)
{
  //if (State != ETurretState::Set)
  //{
  //	return;
  //}

  IEnemy* Enemy = Cast<IEnemy>(OtherActor);
  if (!Enemy || !IsValid(DamageComponent) || !IsValid(Enemy->GetHealthComponent()))
  {
    return;
  }

  DamageComponent->StopDamagePerSecond(Enemy->GetHealthComponent());
}