// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileTurret.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Projectiles/Projectile.h"
#include "Pool/Pool.h"
#include "Components/SphereComponent.h"
#include <Kismet/GameplayStatics.h>
#include "FMODBlueprintStatics.h"
#include "AI/EnemyTargetPoints.h"
#include "Enemies/BasicEnemy.h"
#include "Traps/RepeaterBeacon.h"
#include "RewardsAndUpgrades/Upgrade.h"
#include "HealthSystem/HealthComponent.h"
#include "Projectiles/AreaProjectile.h"
#include "RewardsAndUpgrades/UpgradesData.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "Managers/UpgradesManager.h"

AProjectileTurret::AProjectileTurret()
{
}

void AProjectileTurret::BeginPlay()
{
  TypeOfTurret = ETurretType::MachineGunTurret;
  WeaponMesh = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("Weapon")));
  BarrelMesh = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("Barrel")));

  //Note: BeginPlay executes the function StartTurret()
  Super::BeginPlay();

  //SphereShootRange->SetSphereRadius(Range);
  SphereShootRange->OnComponentBeginOverlap.AddDynamic(this, &AProjectileTurret::OnOverlapBegin);
  SphereShootRange->OnComponentEndOverlap.AddDynamic(this, &AProjectileTurret::OnOverlapEnd);
  SphereShootRange->SetSphereRadius(Range);

  RotationCorrection = WeaponMesh->GetRelativeRotation();

  //Obtain the pool reference.
  MyPool = Cast<APool>(UGameplayStatics::GetActorOfClass(GetWorld(), APool::StaticClass()));
  if (!IsValid(MyPool))
  {
    UE_LOG(LogTemp, Warning, TEXT("AProjectileTurret.cpp: No se encontro la Pool"));
  }
}

void AProjectileTurret::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
}

void AProjectileTurret::StopTurret()
{
  Super::StopTurret();

  GetWorld()->GetTimerManager().ClearTimer(FireTimerHandler);
}

void AProjectileTurret::StartTurret()
{
  Super::StartTurret();
  FindTarget();
  UpdateFireRate();
  RestartTimer();
}

void AProjectileTurret::RestartTimer()
{
  GetWorld()->GetTimerManager().ClearTimer(FireTimerHandler);
  //UpdateFireRate();
  GetWorld()->GetTimerManager().SetTimer(FireTimerHandler, this, &AProjectileTurret::Fire, FireRate);
}

void AProjectileTurret::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  IEnemy* Enemy = Cast<IEnemy>(OtherActor);

  if (Enemy != nullptr && Enemy->GetHealthComponent()->GetActualHealth() > 0)
  {
    PossibleTargets.Add(OtherActor);
  }

  if (State == ETurretState::Set || State == ETurretState::Moving || State == ETurretState::Conquest)
  {
    if (CheckIfTargetIsValid())
    {
      FindTarget();
    }
    else
    {
      FindTarget();
      Fire();
    }

  }
}

void AProjectileTurret::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
  if (PossibleTargets.Contains(OtherActor))
  {
    PossibleTargets.Remove(OtherActor);
  }
}

// Look for the nearest enemy to make it the target
void AProjectileTurret::FindTarget()
{
  AActor* BestTarget = nullptr;
  float BestDistance = MAX_FLT;
  TArray<AActor*> ActorsToRemove;

  for (AActor* Actor : PossibleTargets)
  {
    const FVector ActorL = Actor->GetActorLocation();
    const FVector ThisLocation = GetActorLocation();
    IEnemy* Enemy = Cast<IEnemy>(Actor);

    if (Enemy->GetHealthComponent()->GetActualHealth() > 0.0f)
    {

      if (!IsValid(BestTarget))
      {
        BestTarget = Actor;
        BestDistance = FVector::DistSquared(ActorL, ThisLocation);
      }
      else
      {
        const float CurrentDistance = FVector::DistSquared(ActorL, ThisLocation);

        if (CurrentDistance < BestDistance)
        {
          //DrawDebugLine(GetWorld(), BarrelMesh->GetComponentLocation(), Actor->GetActorLocation(), FColor(255, 0, 0), false, 5, 1, 3.f);
          BestDistance = CurrentDistance;
          BestTarget = Actor;
        }
      }
    }
    else
    {
      ActorsToRemove.Add(Actor);
    }
  }
  //clean array
  for (AActor* ToRemove : ActorsToRemove)
  {
    PossibleTargets.Remove(ToRemove);
  }

  Target = BestTarget;
}

bool AProjectileTurret::IsAnythingBetweenTheTurretAndTheTarget(const AActor* EnemyTarget)
{
  if (IsValid(EnemyTarget))
  {
    FHitResult Hit;
    FCollisionQueryParams TraceParams;

    TraceParams.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(Hit, WeaponMesh->GetComponentLocation(), EnemyTarget->GetActorLocation(), ECC_GameTraceChannel1, TraceParams))
    {
      if (Cast<IEnemy>(Hit.GetActor()) == nullptr)
      {
        return true;
      }
    }
  }

  return false;
}

void AProjectileTurret::RotateTurret()
{
  FVector TurretLocation = WeaponMesh->GetComponentLocation();
  FVector TargetLocation = Target->GetActorLocation();
  FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(TurretLocation, TargetLocation);
  Rotation.Pitch = 0;
  WeaponMesh->SetWorldRotation(Rotation + RotationCorrection);
}

void AProjectileTurret::Fire()
{
  if (!CheckIfTargetIsValid())
  {
    FindTarget();
  }

  if (IsValid(ProjectileClass) && IsValid(Target) && ((State == ETurretState::Set) || State == ETurretState::Moving || State == ETurretState::Conquest))
  {
    UActorComponent* HealthActorC = Target->GetComponentByClass(UHealthComponent::StaticClass());

    UHealthComponent* TargetHealthComponent = Cast<UHealthComponent>(HealthActorC);

    if (IsValid(TargetHealthComponent) && TargetHealthComponent->GetActualHealth() > 0.f || Target->IsA<ARepeaterBeacon>())
    {
      RotateTurret();

      FVector ShootLocation = BarrelMesh->GetComponentLocation();
      FRotator ShootRotation = WeaponMesh->GetComponentRotation() - RotationCorrection;
      ShootRotation.Normalize();

      //Gets the next projectile of the pool
      AActor* ProjectileActor = MyPool->GetNextActor(ProjectileClass);
      if (IsValid(ProjectileActor))
      {
        AProjectile* Projectile = Cast<AProjectile>(ProjectileActor);
        if (IsValid(Projectile))
        {
          Projectile->FiringTurret = this;
          Projectile->ShootProjectile(ShootLocation, ShootRotation, Power, SphereShootRange->GetScaledSphereRadius(), this, Target);

          ShootEvent.Broadcast();
        }
        else
        {
          UE_LOG(LogTemp, Warning, TEXT("ProjectileTurret.cpp: No se pudo spawnear proyectil! Probablemente no quedan proyectiles en la Pool."));
        }
      }

    }
  }
  RestartTimer();
  // After shooting a projectile, check again for nearest target
  FindTarget();

}

bool AProjectileTurret::CheckIfTargetIsValid()
{
  if (!IsValid(Target))
  {
    return false;
  }

  if (!Target->IsA<ARepeaterBeacon>())
  {
    IEnemy* Enemy = Cast<IEnemy>(Target);

    if (Enemy == nullptr || Enemy->GetHealthComponent()->GetActualHealth() <= 0)
    {
      PossibleTargets.Remove(Target);
      return false;
    }
  }

  return true;
}

void AProjectileTurret::UpgradeTurret(FUpgradesDataStruct* Upgrade, int Quantity)
{
  Super::UpgradeTurret(Upgrade, Quantity);
  if (Upgrade != nullptr)
  {
    float IncreaseOfUpgrade = GetIncreaseOfUpgrade(*Upgrade, 0);
    switch (Upgrade->Type)
    {
      case EUpgradeType::Range:
      {
        Range += (Range * IncreaseOfUpgrade * Quantity);
        SphereShootRange->SetSphereRadius(Range);
        break;
      }
      case EUpgradeType::Cadence:
      {
        MinShootTimeRange -= (MinShootTimeRange * IncreaseOfUpgrade * Quantity);
        if (MinShootTimeRange < 0.01f)
        { 
            MinShootTimeRange = 0.01f;
        }
        MaxShootTimeRange -= (MaxShootTimeRange * IncreaseOfUpgrade * Quantity);
        if (MaxShootTimeRange < 0.01f)
        {
            MaxShootTimeRange = 0.01f;
        }
        UpdateFireRate();
        //RestartTimer();
        break;
      }
      case EUpgradeType::Power:
      {
        Power += (Power * IncreaseOfUpgrade * Quantity);
        break;
      }
      case EUpgradeType::Health:
      {
        HealthComponent->IncreaseMaxHealthWithPercentage(IncreaseOfUpgrade * Quantity);
        break;
      }
    }

    //Update widget information
    UpdateUpgradesInfo(*Upgrade, Quantity);

    UpdateUpgradesInfoEvent.Broadcast(UpgradesInfo);
  }
}

float AProjectileTurret::GetIncreaseOfUpgrade(FUpgradesDataStruct Upgrade, int AddNext)
{
  int* NumberOfUpgradesApplied = AppliedUpgrades.Find(Upgrade.Type);
  int NumberApplied;
  if (NumberOfUpgradesApplied)
  {
    NumberApplied = *NumberOfUpgradesApplied + AddNext;

  }
  else
  {
    NumberApplied = AddNext;
  }

  float valueOfIncrease = 0.0f;


  for (int i = 0; i < Upgrade.IncreasesInfo.Num(); i++)
  {
    if (Upgrade.IncreasesInfo[i].Type == TypeOfTurret)
    {
      for (int j = 0; j < Upgrade.IncreasesInfo[i].MaxRanges.Num(); j++)
      {
        if (NumberApplied <= Upgrade.IncreasesInfo[i].MaxRanges[j])
        {
          valueOfIncrease = Upgrade.IncreasesInfo[i].IncreasesOfUpgrade[j];
          break;
        }
      }
      break;
    }
  }

  return valueOfIncrease;
}

void AProjectileTurret::InitializeUpgradeInfo()
{
  TMap<EUpgradeType, FUpgradesDataStruct> Upgrades = GameMode->UpgradesManager->UpgradesData->Upgrades;


  for (auto Upgrade : Upgrades)
  {
    switch (Upgrade.Key)
    {
      case EUpgradeType::Range:
        UpgradesInfo.Range = 1;
        UpgradesInfo.UpgradedRange = 2;
        break;
      case EUpgradeType::Cadence:
        UpgradesInfo.Cadence = 1;
        UpgradesInfo.UpgradedCadence = 2;
        break;
      case EUpgradeType::Power:
        UpgradesInfo.Power = 1;
        UpgradesInfo.UpgradedPower = 2;
        break;
      case EUpgradeType::Health:
        UpgradesInfo.Health = 1;
        UpgradesInfo.UpgradedHealth = 2;
        break;
    }
  }
  UpdateUpgradesInfoEvent.Broadcast(UpgradesInfo);
}

void AProjectileTurret::UpdateUpgradesInfo(FUpgradesDataStruct Upgrade, int Quantity)
{
  float IncreaseOfUpgrade = GetIncreaseOfUpgrade(Upgrade, 1);

  switch (Upgrade.Type)
  {
    case EUpgradeType::Range:
      UpgradesInfo.Range++;
      UpgradesInfo.UpgradedRange++;
      break;
    case EUpgradeType::Cadence:
      UpgradesInfo.Cadence++;
      UpgradesInfo.UpgradedCadence++;
      break;
    case EUpgradeType::Power:
      UpgradesInfo.Power++;
      UpgradesInfo.UpgradedPower++;
      break;
    case EUpgradeType::Health:
      UpgradesInfo.Health++;
      UpgradesInfo.UpgradedHealth++;
      break;
  }
}

void AProjectileTurret::SetRepeaterBeacon(AActor* RepeaterBeacon)
{
  Target = RepeaterBeacon;

  // Fire Projectile of type Area
  if (ProjectileClass->GetSuperClass() == AProjectile::StaticClass())
  {
    RotateTurret();

    FVector ShootLocation = BarrelMesh->GetComponentLocation();
    FRotator ShootRotation = WeaponMesh->GetComponentRotation() - RotationCorrection;
    ShootRotation.Normalize();

    UWorld* World = GetWorld();

    if (World)
    {
      FActorSpawnParameters SpawnParams;
      SpawnParams.Owner = this;
      SpawnParams.Instigator = GetInstigator();

      // Spawn the projectile at the muzzle.
      AProjectile* Projectile = World->SpawnActor<AProjectile>(BeaconProjectile, ShootLocation, ShootRotation, SpawnParams);
      if (Projectile)
      {
        // Set the projectile's initial trajectory.
        FVector LaunchDirection = ShootRotation.Vector();
        Projectile->FireInDirection(LaunchDirection);
      }
    }
    FindTarget();
  }
  else
  {
    Fire();
  }
}


void AProjectileTurret::UpdateFireRate()
{
  if (MinShootTimeRange > 0 && MaxShootTimeRange > 0)
  {
    FireRate = FMath::RandRange(MinShootTimeRange, MaxShootTimeRange);
  }
}