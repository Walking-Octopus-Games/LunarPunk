// Fill out your copyright notice in the Description page of Project Settings.
#include "Turret.h"
#include "HealthSystem/HealthComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include <Kismet/GameplayStatics.h>
#include "AudioTurretComponent.h"
#include "RewardsAndUpgrades/UpgradesData.h"
#include "AI/MovementTurretComponent.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Managers/PlatesManager.h"
#include "RewardsAndUpgrades/UpgradesData.h"
#include "Managers/HUDManager.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "Managers/EntityManager.h"
#include "Managers/UpgradesManager.h"
#include "PositionHelperIcon.h"
#include "TargetForEnemiesHandler.h"
#include "AI/EnemyTargetPoints.h"
#include "Utilities/UtilitiesLunarpunk.h"

// Sets default values
ATurret::ATurret()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

  MyRootComponent = CreateDefaultSubobject<USceneComponent>("MyRootComponent");
  SetRootComponent(MyRootComponent);

  HealthComponent = CreateDefaultSubobject<UHealthComponent>(FName("Health"));
  HealthComponent->ActorDeath.AddDynamic(this, &ATurret::DeactivateTurret);

  AudioTurretComponent = CreateDefaultSubobject<UAudioTurretComponent>(FName("Audio Turret Component"));

  //Deactivated effect
  DeactivatedEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DeactivatedEffectComponent"));
  DeactivatedEffectComponent->SetupAttachment(RootComponent);

  UpgradeEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("UpgradeEffectComponent"));
  UpgradeEffectComponent->SetupAttachment(RootComponent);

  /*FloatingEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FloatingEffectComponent"));*/

  MeshShield = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMeshComponent"));
  MeshShield->SetupAttachment(RootComponent);
  MeshShield->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);



  TurretMovementComponent = CreateDefaultSubobject<UMovementTurretComponent>(TEXT("Turret Movement Component"));

  FloatingMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating Movement Component"));

  TargetForEnemiesHandlerComponent = CreateDefaultSubobject<UTargetForEnemiesHandler>(TEXT("Target For Enemies Handler"));




}

void ATurret::InitTurretState()
{
  if (State != ETurretState::Conquest)
  {
    State = ETurretState::Set;
  }

  if (IsValid(HealthComponent))
  {
    HealthComponent->SetCanBeDamaged(true);
  }

  StartTurret();
}

void ATurret::Initialize()
{
  //AreaForEnemies = GetDefaultSubobjectByName("AreaForEnemiesToWait");


  HealthComponent->DamageReceived.AddDynamic(this, &ATurret::DrawWidget);

  GameMode = Cast<ALunarPunkGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
  if (IsValid(GameMode))
  {
    GameMode->HUDManager->SetShowDamagePointsEvent.AddDynamic(this, &ATurret::SetShowDamagePoints);
    GameMode->EndGameModeInitializeEvent.AddDynamic(this, &ATurret::InitializeUpgradeInfo);
  }

  AudioTurretComponent->Initialize();

  RefreshInfoWidget(UUtilitiesLunarpunk::GetPlayerPlates(GetWorld()));

  //InitTurretState();

  //RemoveInfoWidget();
}
void ATurret::InitializeUpgradeInfo()
{
}

void ATurret::UpdateUpgradesInfo(FUpgradesDataStruct Upgrade, int Quantity)
{
}


// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
  Super::BeginPlay();

  USphereComponent* AvoidCollisionRange = Cast<USphereComponent>(GetOwner()->GetDefaultSubobjectByName(TurretMovementComponent->CollisionName));

  if (AvoidCollisionRange)
  {
    AvoidCollisionRange->SetupAttachment(GetRootComponent());
  }
  // Tuvimos que hacer esto porque hay un bug raro con el attachment :)
  AreaForEnemies = Cast<USphereComponent>(GetDefaultSubobjectByName("AreaForEnemiesToWait"));
  TargetPoint1 = Cast<UEnemyTargetPoints>(GetDefaultSubobjectByName(FName("EnemyTargetPoints" + FString::FromInt(1))));
  TargetPoint2 = Cast<UEnemyTargetPoints>(GetDefaultSubobjectByName(FName("EnemyTargetPoints" + FString::FromInt(2))));
  TargetPoint3 = Cast<UEnemyTargetPoints>(GetDefaultSubobjectByName(FName("EnemyTargetPoints" + FString::FromInt(3))));
  TargetPoint4 = Cast<UEnemyTargetPoints>(GetDefaultSubobjectByName(FName("EnemyTargetPoints" + FString::FromInt(4))));
  EnemyTargetPoints.Empty();
  //TargetPoint->SetupAttachment(MyRootComponent);UEnemyTargetPoints
  EnemyTargetPoints.Add(TargetPoint1);
  EnemyTargetPoints.Add(TargetPoint2);
  EnemyTargetPoints.Add(TargetPoint3);
  EnemyTargetPoints.Add(TargetPoint4);

  SphereShootRange = Cast<USphereComponent>(GetDefaultSubobjectByName("ShootRange"));

  MeshShield->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
  MeshShield->SetVisibility(false);

  TArray<UStaticMeshComponent*> Components;
  GetComponents<UStaticMeshComponent>(Components);
  for (int32 i = 0; i < Components.Num(); i++)
  {
    Components[i]->SetRenderCustomDepth(true);
    Components[i]->SetCustomDepthStencilValue(STENCIL_TURRET);
  }

  if (IsValid(DeactivatedEffectComponent))
  {
    DeactivatedEffectComponent->Deactivate();
  }

  if (IsValid(UpgradeEffectComponent))
  {
    UpgradeEffectComponent->DeactivateImmediate();
  }

  FloatingEffectComponent = Cast<UNiagaraComponent>(GetDefaultSubobjectByName("FloatingEffect"));

  if (IsValid(FloatingEffectComponent))
  {
    //FloatingEffectComponent->SetupAttachment(RootComponent);
    FloatingEffectComponent->DeactivateImmediate();
  }

  //PlaceTurret();
  InitTurretState();
  RemoveInfoWidget();

  GameMode = Cast<ALunarPunkGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

  PositionHelperIconComponent = FindComponentByClass<UPositionHelperIcon>();

  TargetForEnemiesHandlerComponent->Initialize(this);


}

void ATurret::DrawWidget(float DamageReceived)
{
  if (bShowDamagePoints)
  {
    DrawDamageWidget((int)DamageReceived);
  }
}

void ATurret::SetShowDamagePoints(bool _bShowDamagePoints)
{
  this->bShowDamagePoints = _bShowDamagePoints;
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (State == ETurretState::Repairing)
  {
    CurrentTimeLeftToHeal -= RecoverRate * DeltaTime;
    if (CurrentTimeLeftToHeal <= 0)
    {
      if (IsValid(GameMode->GetPlatesManager()) && IsValid(HealthComponent))
      {
        if (GameMode->GetPlatesManager()->TrySpendPlates(CostRepair()))
        {
          RestartTurret();
          StopRepairing.Broadcast();
        }
      }
      CurrentTimeLeftToHeal = 0;

    }
    Repairing.Broadcast();
  }
}

void ATurret::RestartTurret()
{
  if (HealthComponent->GetActualHealth() <= 0)
  {
    TurretReactivated.Broadcast();
  }

  HealthComponent->ResetHealth();
  HealthComponent->SetCanBeDamaged(true);

  if (PreviousState == ETurretState::Conquest)
  {
    State = PreviousState;
  }
  else
  {
    State = ETurretState::Set;
  }

  if (IsValid(DeactivatedEffectComponent))
  {
    DeactivatedEffectComponent->Deactivate();
  }

  StartTurret();
  RefreshInfoWidget(UUtilitiesLunarpunk::GetPlayerPlates(GetWorld()));
}

void ATurret::StartTurret()
{
  if (IsValid(GameMode) && IsValid(GameMode->EntityManager) && GameMode->EntityManager->IsAnyTurretMoving())
  {
    TurretMovementComponent->ManageTurretMovement(false);
  }
}

void ATurret::StopTurret()
{

}

void ATurret::UpgradeTurret(FUpgradesDataStruct* Upgrade, int Quantity)
{

  if (Upgrade != nullptr)
  {
    if (AppliedUpgrades.Contains(Upgrade->Type))
    {
      AppliedUpgrades.Add(Upgrade->Type, *AppliedUpgrades.Find(Upgrade->Type) + 1);
    }
    else
    {
      AppliedUpgrades.Add(Upgrade->Type, 1);
    }

    UpgradeAdded.Broadcast(Upgrade->Type);

    //Effect
    if (IsValid(UpgradeEffectComponent) && State != ETurretState::Moving)
    {
      UpgradeEffectComponent->DeactivateImmediate();
      UpgradeEffectComponent->SetNiagaraVariableLinearColor(FString("Color"), Upgrade->Tint);
      UpgradeEffectComponent->Activate(true);
    }
  }
}

void ATurret::ActivateShield()
{
  MeshShield->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Block);;
  MeshShield->SetVisibility(true);
  HealthComponent->SetCanBeDamaged(false);
  GetWorldTimerManager().ClearTimer(ShieldTimerHandle);
  GetWorldTimerManager().SetTimer(ShieldTimerHandle, this, &ATurret::DeactivateShield, 1.0f, false, TimeShield);
}

void ATurret::DeactivateShield()
{
  HealthComponent->SetCanBeDamaged(true);
  MeshShield->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
  MeshShield->SetVisibility(false);
}

void ATurret::PlaceTurret()
{
  InitTurretState();

  RefreshInfoWidget(UUtilitiesLunarpunk::GetPlayerPlates(GetWorld()));
  TurretLeft.Broadcast();
}

bool ATurret::PickUpTurret()
{
  if (State != ETurretState::Set && State != ETurretState::Conquest)
  {
    return false;
  }

  if (TurretMovementComponent && TurretMovementComponent->MovementState != ETurretMovementState::Landed)
  {
    return false;
  }

  State = ETurretState::PickedUp;

  if (IsValid(HealthComponent))
  {
    HealthComponent->SetCanBeDamaged(false);
  }

  UpgradeEffectComponent->DeactivateImmediate();

  StopTurret();

  if (IsValid(GameMode) && GameMode->HUDManager->bShowTurretsInfo)
  {
    RemoveInfoWidget();
  }

  TurretPickedUp.Broadcast();
  return true;
}

bool ATurret::StartRepairing()
{
  if (!IsValid(HealthComponent))
  {
    return false;
  }

  if (State == ETurretState::Deactivated)
  {
    State = ETurretState::Repairing;

    CurrentTimeLeftToHeal = MaxTimeLeftToRevive;

    if (IsValid(RepairEffectComponent))
    {
      UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), RepairEffectComponent, GetActorLocation(), FRotator::ZeroRotator);
    }

    return true;
  }
  else if (State == ETurretState::Set || State == ETurretState::Conquest)
  {

    if (HealthComponent->GetHealth() >= HealthComponent->GetMaxHealth())
    {
      return false;
    }

    PreviousState = State;
    State = ETurretState::Repairing;

    CurrentTimeLeftToHeal = MaxTimeLeftToRepair;
    HealthComponent->SetCanBeDamaged(false);
    StopTurret();

    if (IsValid(RepairEffectComponent))
    {
      UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), RepairEffectComponent, GetActorLocation(), FRotator::ZeroRotator);
    }

    return true;
  }
  else
  {
    return false;
  }
}

void ATurret::InterruptRepair()
{
  if (State != ETurretState::Repairing || !IsValid(HealthComponent))
  {
    return;
  }

  if (HealthComponent->GetHealth() > 0)
  {
    if (PreviousState == ETurretState::Conquest)
    {
      State = PreviousState;
    }
    else
    {
      State = ETurretState::Set;
    }
    HealthComponent->SetCanBeDamaged(true);
    StartTurret();
  }
  else
  {
    State = ETurretState::Deactivated;
  }

  CurrentTimeLeftToHeal = 0;

  StopRepairing.Broadcast();
}

void ATurret::DeactivateTurret()
{
  PreviousState = State;
  State = ETurretState::Deactivated;

  if (IsValid(TurretMovementComponent) && IsValid(TurretMovementComponent->AIController))
  {
    TurretMovementComponent->AIController->StopMovement();
    TurretMovementComponent->ForceLanding();
  }

  if (IsValid(HealthComponent))
  {
    HealthComponent->SetCanBeDamaged(false);
  }

  if (IsValid(DeactivatedEffectComponent->GetAsset()))
  {
    DeactivatedEffectComponent->Activate();
  }

  TurretDeath.Broadcast();
  StopTurret();
  RefreshInfoWidget(UUtilitiesLunarpunk::GetPlayerPlates(GetWorld()));
}


ETurretState ATurret::GetState()
{
  return State;
}

int32 ATurret::CostRepair()
{

  float LifePoints = HealthComponent->GetMaxHealth() - HealthComponent->GetActualHealth();

  if (LifePoints <= 0)
  {
    return 0;
  }
  else
  {
    int32 Cost;
    bFixedRepairCost ? Cost = FixedRepairCost * LifePoints / HealthComponent->GetMaxHealth() : RepairCostPerHealthPoint * (LifePoints);
    return FMath::Max(1, Cost);
  }

}

void ATurret::TurnOnOutline()
{
  TArray<UStaticMeshComponent*> Components;
  GetComponents<UStaticMeshComponent>(Components);
  for (int32 i = 0; i < Components.Num(); i++)
  {
    Components[i]->SetCustomDepthStencilValue(STENCIL_TURRET_WITH_OUTLINE);
  }
  if (IsValid(GameMode) && GameMode->HUDManager->bShowTurretsInfo)
  {
    DrawInfoWidget();
  }

}

void ATurret::TurnOffOutline()
{
  TArray<UStaticMeshComponent*> Components;
  GetComponents<UStaticMeshComponent>(Components);
  for (int32 i = 0; i < Components.Num(); i++)
  {
    Components[i]->SetCustomDepthStencilValue(STENCIL_TURRET);
  }
  //if (IsValid(GameMode) && GameMode->HUDManager->bShowTurretsInfo)
  //{
  RemoveInfoWidget();
  //}
}

UNiagaraComponent* ATurret::GetDeactivatedEffectComponent()
{
  return DeactivatedEffectComponent;
}

void ATurret::SetDeactivatedEffectComponent(UNiagaraComponent* NewDeactivatedEffectComponent)
{
  this->DeactivatedEffectComponent = NewDeactivatedEffectComponent;
}

void ATurret::SetTargetWaypoint(ATurretWaypoint* NextWaypoint)
{
  this->TargetWaypoint = NextWaypoint;
  TurretMovementComponent->CalculateDestinationPosition();
  TurretMovementComponent->MoveInCaseItsAlreadyMoving();
}


void ATurret::StartMovementEffect()
{
  if (FloatingEffectComponent)
  {
    FloatingEffectComponent->ActivateSystem();
  }
}


void ATurret::StopMovementEffect()
{

  if (FloatingEffectComponent)
  {
    FloatingEffectComponent->DeactivateImmediate();
  }
}

void ATurret::SetTurretType(ETurretType Type)
{
  TypeOfTurret = Type;
}

