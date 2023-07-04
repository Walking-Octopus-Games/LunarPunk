// Fill out your copyright notice in the Description page of Project Settings.


#include "Traps/RepeaterBeacon.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pool/Pool.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Turrets/Projectiles/Projectile.h"
#include "Turrets/HealingTurret.h"
#include "Turrets/Projectiles/AreaProjectile.h"
#include "Turrets/Projectiles/HealingProjectile.h"
#include "Turrets/Projectiles/BeaconAreaProjectile.h"
#include "HealthSystem/HealthComponent.h"
#include "HealthSystem/DamageComponent.h"


/* ----------- How it works --------------
  - Starts as State Inactive and StateType None
  - Collects all the turrets and enemies overlapping in a range on 2 TArray, creates a priorityQueue for the differents type of turrets
  - Upon receiving 1 shoot from the player or the ActiveTrapAbility -> LaunchTrap() -> This set the State to Enabled
  - Then AskForSetUp() to the turret type which has the higher priority -> this informs the turret to set the beacon and sets -> State::WaitingSet and StateType::(TurretType)
  - On Projectile overlap Checks if is State is waiting for set and projectileType matches StateType::(TurretType) on True -> State::Enabled
  - When State::Enabled and StateType != None Attacks as the StateType::(TurretType) indicated
  - After finish action +1 to the X times can be setted(TimesSetted) before destroy StateType::None -> Repeat from 4.
*/

void ARepeaterBeacon::BeginPlay()
{
  Super::BeginPlay();

  SphereSetRange = Cast<USphereComponent>(GetDefaultSubobjectByName("BeaconDetectionRange"));

  SphereSetRange->SetSphereRadius(Range);
  SphereSetRange->OnComponentBeginOverlap.AddDynamic(this, &ARepeaterBeacon::OnOverlapBegin);
  SphereSetRange->OnComponentEndOverlap.AddDynamic(this, &ARepeaterBeacon::OnOverlapEnd);

  CapsuleCollision = Cast<UCapsuleComponent>(GetDefaultSubobjectByName("CapsuleComponent"));
  CapsuleCollision->OnComponentBeginOverlap.AddDynamic(this, &ARepeaterBeacon::OnOverlapBegin);
  CapsuleCollision->OnComponentEndOverlap.AddDynamic(this, &ARepeaterBeacon::OnOverlapEnd);

  BeaconMesh = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName("Body"));
  BeaconMesh->SetMaterial(0, DisabledMaterial);

  //BeaconMesh->OnComponentBeginOverlap.AddDynamic(this, &ARepeaterBeacon::OnOverlapBegin);

  //Obtain the pool reference.
  MyPool = Cast<APool>(UGameplayStatics::GetActorOfClass(GetWorld(), APool::StaticClass()));

  SetActorTickEnabled(false);
  if (!IsValid(MyPool))
  {
    UE_LOG(LogTemp, Warning, TEXT("RepeaterBeacon.cpp: No se encontro la Pool"));
  }
}

// Called every frame
void ARepeaterBeacon::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (State == ERepeaterBeaconState::Enabled)
  {
    if (StateType == ERepeaterBeaconType::None && !PreferenceQueue.IsEmpty())
    {
      AskForSetUp();
    }
    else if (StateType == ERepeaterBeaconType::MachineGunTurret)
    {
      if (ShootCooldownTime > 0)
      {
        ShootCooldownTime -= DeltaTime;
      }
      else if (PossibleTargets.Num() != 0)
      {
        ShootCooldownTime = TimeForAllMGShoots / MaxNumShootsMachineGun;
        DoTrapEffect();
      }
    }
    else if (StateType == ERepeaterBeaconType::AreaTurret)
    {
      DoTrapEffect();
    }
    else if (StateType == ERepeaterBeaconType::HealingTurret)
    {
      if (PossibleTargets.Num() != 0)
      {
        DoTrapEffect();
      }

    }
  }
}

void ARepeaterBeacon::SetAsTurret(const float& power)
{
  if (State == ERepeaterBeaconState::WaitingSet)
  {
    BeaconMesh->SetMaterial(0, SettedMaterial);
    this->Power = power;
    State = ERepeaterBeaconState::Enabled;
    CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  }
}

void ARepeaterBeacon::LaunchTrap()
{
  SetActorTickEnabled(true);
  State = ERepeaterBeaconState::Enabled;
  BeaconMesh->SetMaterial(0, EnabledMaterial);
  DeactivateTrap();
}

void ARepeaterBeacon::DoTrapEffect()
{
  if (StateType == ERepeaterBeaconType::MachineGunTurret)
  {
    FireAsMachineGunTurret();
  }
  else if (StateType == ERepeaterBeaconType::AreaTurret)
  {
    FireAsAreaTurret();
  }
  else if (StateType == ERepeaterBeaconType::HealingTurret)
  {
    FireAsHealingTurret();
  }
}

void ARepeaterBeacon::FireAsMachineGunTurret()
{
  if (IsValid(MachineGunProjectileClass) && IsValid(Target))
  {
    FVector ShootLocation = this->GetActorLocation() +5.0f;

    FRotator ShootRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
    ShootRotation.Normalize();

    //Gets the next projectile of the pool
    AActor* Projectile = MyPool->GetNextActor(MachineGunProjectileClass);

    if (IsValid(Projectile))
    {
      //Sets location and rotation.
      Projectile->SetActorLocation(ShootLocation);
      Projectile->SetActorRotation(ShootRotation);

      //Reactivate the projectile
      if (Projectile->Implements<UIsPoolable>())
      {
        Cast<IIsPoolable>(Projectile)->Reactivate();
      }

      //Shoot the projectile
      AProjectile* ProjectileCasted = Cast<AProjectile>(Projectile);
      if (IsValid(ProjectileCasted))
      {
        // Set the projectile's initial trajectory.
        ProjectileCasted->CollisionComponent->MoveIgnoreActors.Add(this);
        ProjectileCasted->DamageComponent->SetDamage(Power, Power);
        FVector LaunchDirection = ShootRotation.Vector();
        ProjectileCasted->FireInDirection(LaunchDirection);
        //ShootEvent.Broadcast();
      }
    }
    else
    {
      UE_LOG(LogTemp, Warning, TEXT("ProjectileTurret.cpp: No se pudo spawnear proyectil! Probablemente no quedan proyectiles en la Pool."));
    }

    NumShootsMachineGun += 1;
    if (NumShootsMachineGun == MaxNumShootsMachineGun)
    {
      NumShootsMachineGun = 0;
      FinishSetUp();
    }
  }
  FindTarget();
}

void ARepeaterBeacon::FireAsAreaTurret()
{
  // ReEnabled when Projectile finish
  State = ERepeaterBeaconState::Disabled;

  FVector ShootLocation = this->GetActorLocation();
  FRotator SpawnRotation(0.0f, 0.0f, 0.0f);

  FActorSpawnParameters SpawnParams;
  SpawnParams.Owner = this;
  SpawnParams.Instigator = GetInstigator();

  //AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ShootLocation, SpawnRotation, SpawnParams);
  ABeaconAreaProjectile* Projectile = GetWorld()->SpawnActor<ABeaconAreaProjectile>(AreaProjectileClass, ShootLocation, SpawnRotation, SpawnParams);
  Projectile->FinishEvent.AddDynamic(this, &ARepeaterBeacon::FinishSetUp);

}

void ARepeaterBeacon::FireAsHealingTurret()
{
  if (IsValid(Target))
  {
    FVector ShootLocation = this->GetActorLocation() + 5.0f;

    FRotator ShootRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
    ShootRotation.Normalize();

    //Gets the next projectile of the pool
    AActor* ProjectileActor = MyPool->GetNextActor(HealingProjectileClass);

    if (IsValid(ProjectileActor))
    {
      AProjectile* Projectile = Cast<AProjectile>(ProjectileActor);
      if (IsValid(Projectile))
      {
        Projectile->ShootProjectile(ShootLocation, ShootRotation, Power, SphereSetRange->GetScaledSphereRadius(), FiringTurret, Target);
        State = ERepeaterBeaconState::Disabled;
        //ShootEvent.Broadcast();
        Projectile->ReturnPoolEvent.AddDynamic(this, &ARepeaterBeacon::FinishSetUp);
      }
      else
      {
        UE_LOG(LogTemp, Warning, TEXT("RepeaterBeacon.cpp: No se pudo spawnear proyectil Healing! Probablemente no quedan proyectiles en la Pool."));
      }
    }
  }
  FindTarget();
}

//Pasar a componente
// Look for the nearest enemy to make it the target
void ARepeaterBeacon::FindTarget()
{
  AActor* BestTarget = nullptr;
  float BestDistance = MAX_FLT;

  for (AActor* Actor : PossibleTargets)
  {
    const FVector ActorL = Actor->GetActorLocation();
    const FVector ThisLocation = GetActorLocation();
    IEnemy* Enemy = Cast<IEnemy>(Actor);

    if (Enemy->GetHealthComponent()->GetActualHealth() > 0.0f) //&& !IsAnythingBetweenTheTurretAndTheTarget(Actor))
    {

      if (!IsValid(BestTarget))
      {

        BestTarget = Actor;
        BestDistance = (ActorL.X - ThisLocation.X) * (ActorL.X - ThisLocation.X)
          + (ActorL.Y - ThisLocation.Y) * (ActorL.Y - ThisLocation.Y)
          + (ActorL.Z - ThisLocation.Z) * (ActorL.Z - ThisLocation.Z);

      }
      else
      {
        const float CurrentDistance = (ActorL.X - ThisLocation.X) * (ActorL.X - ThisLocation.X)
          + (ActorL.Y - ThisLocation.Y) * (ActorL.Y - ThisLocation.Y)
          + (ActorL.Z - ThisLocation.Z) * (ActorL.Z - ThisLocation.Z);

        if (CurrentDistance < BestDistance)
        {
          BestDistance = CurrentDistance;
          BestTarget = Actor;
        }
      }
    }
  }
  Target = BestTarget;
}

// Pasar a componente
bool ARepeaterBeacon::IsAnythingBetweenTheTurretAndTheTarget(const AActor* EnemyTarget)
{
  if (IsValid(EnemyTarget))
  {

    FHitResult Hit;
    FCollisionQueryParams TraceParams;

    TraceParams.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), EnemyTarget->GetActorLocation(), ECC_GameTraceChannel1, TraceParams))
    {
      //DrawDebugLine(GetWorld(), BarrelMesh->GetComponentLocation(), EnemyTarget->GetActorLocation(), FColor(1, 1, 1), false, 10, 1, 3.f);
      if (Cast<IEnemy>(Hit.GetActor()) == nullptr)
      {
        return true;
      }
    }
  }
  return false;
}

void ARepeaterBeacon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  if (OverlappedComponent->IsA<UCapsuleComponent>())
  {
    if (State == ERepeaterBeaconState::WaitingSet)
    {
      // The order matters because an AreaProjectile is a Projectile
      if (StateType == ERepeaterBeaconType::AreaTurret)
      {
        if (OtherActor->IsA<AAreaProjectile>())
        {
          SetAsTurret(Cast<AAreaProjectile>(OtherActor)->GetDamageStrength());
          UE_LOG(LogTemp, Warning, TEXT("Setted as AreaTurret"));
        }
      }
      else if (StateType == ERepeaterBeaconType::HealingTurret)
      {
        if (OtherActor->IsA<AHealingProjectile>())
        {
          SetAsTurret(Cast<AHealingProjectile>(OtherActor)->GetDamageStrength());
          UE_LOG(LogTemp, Warning, TEXT("Beacon Setted as HealingTurret"));
        }
      }
      else if (StateType == ERepeaterBeaconType::MachineGunTurret)
      {
        if (OtherActor->IsA<AProjectile>())
        {
          SetAsTurret(Cast<AProjectile>(OtherActor)->GetDamageStrength());
          UE_LOG(LogTemp, Warning, TEXT("Setted as MachineGunTurret"));
        }
      }
    }
    if (OtherActor->IsA<AProjectileTurret>())
    {

      AProjectileTurret* Turret = Cast<AProjectileTurret>(OtherActor);
      if (Turret->ProjectileClass->GetSuperClass() == AAreaProjectile::StaticClass())
      {
        if (!PrefAreaTurretAdded)
        {
          PrefAreaTurretAdded = true;
          PreferenceQueue.Enqueue(ETurretType::AoETurret);
        }
      }
      else if (Turret->ProjectileClass->GetSuperClass() == AProjectile::StaticClass())
      {
        if (!PrefMachineGunTurretAdded)
        {
          PrefMachineGunTurretAdded = true;
          PreferenceQueue.Enqueue(ETurretType::MachineGunTurret);
        }
      }
      else if (OtherActor->IsA<AHealingTurret>())
      {
        if (!PrefHealingTurretAdded)
        {
          PrefHealingTurretAdded = true;
          PreferenceQueue.Enqueue(ETurretType::HealingTurret);
        }
      }
      Turrets.Add(OtherActor);
    }
  }
  else
  {
    IEnemy* Enemy = Cast<IEnemy>(OtherActor);
    //UE_LOG(LogTemp, Warning, TEXT("Vida: %f"), Enemy->GetHealthComponent()->GetActualHealth());
    if (Enemy != nullptr && Enemy->GetHealthComponent()->GetActualHealth() > 0)
    {
      PossibleTargets.Add(OtherActor);
    }
  }
}

void ARepeaterBeacon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
  if (OtherActor->IsA<AProjectileTurret>())
  {
    if (Turrets.Contains(OtherActor))
    {
      Turrets.Remove(OtherActor);
    }
  }
  else if (PossibleTargets.Contains(OtherActor))
  {
    PossibleTargets.Remove(OtherActor);
  }
}

void ARepeaterBeacon::AskForSetUp()
{

  PreferenceQueue.Dequeue(CurrentPref);

  for (auto TurretElement : Turrets)
  {
    AProjectileTurret* Turret = Cast<AProjectileTurret>(TurretElement);
    if (CurrentPref == ETurretType::MachineGunTurret)
    {
      if (Turret->ProjectileClass->GetSuperClass() == AProjectile::StaticClass())
      {
        SetBeaconType(Turret, ERepeaterBeaconType::MachineGunTurret);
        break;
      }
    }
    else if (CurrentPref == ETurretType::AoETurret)
    {
      if (Turret->ProjectileClass->GetSuperClass() == AAreaProjectile::StaticClass())
      {
        SetBeaconType(Turret, ERepeaterBeaconType::AreaTurret);
        break;
      }
    }
    else if (CurrentPref == ETurretType::HealingTurret)
    {
      if (Turret->ProjectileClass->GetSuperClass() == AHealingProjectile::StaticClass())
      {
        //UE_LOG(LogTemp, Warning, TEXT("Trying to set as HealingTurret %s"), *UEnum::GetValueAsString(State));
        SetBeaconType(Turret, ERepeaterBeaconType::HealingTurret);
        break;
      }
    }
  }
  //PreferenceQueue.Enqueue(CurrentPref);
}

void ARepeaterBeacon::SetBeaconType(AProjectileTurret* Turret, ERepeaterBeaconType Type)
{
  if (Turret->State == ETurretState::Set || Turret->State == ETurretState::Moving || Turret->State == ETurretState::Conquest)
  {
    StateType = Type;
    State = ERepeaterBeaconState::WaitingSet;
    FiringTurret = Turret;
    Turret->SetRepeaterBeacon(this);
  }
}

void ARepeaterBeacon::FinishSetUp()
{
  UE_LOG(LogTemp, Warning, TEXT("Beacon: Finish SetUp"));
  TimesSetted += 1;
  if (TimesSetted == MaxTimesSetted)
  {
    DestroyTrap();
  }
  else
  {
    PreferenceQueue.Enqueue(CurrentPref);
    BeaconMesh->SetMaterial(0, EnabledMaterial);

    CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    State = ERepeaterBeaconState::Enabled;
    StateType = ERepeaterBeaconType::None;
  }
}