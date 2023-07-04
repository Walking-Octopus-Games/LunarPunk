// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Turrets/TurretInteractionComponent.h"

#include "NavigationSystem.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "HealthSystem/HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/EntityManager.h"
#include "Managers/PlatesManager.h"
#include "Managers/UpgradesManager.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "Player/LunarPunkPlayerController.h"
#include "Player/PlayableCharacter.h"
#include "RewardsAndUpgrades/GetRewardsComponent.h"
#include "Turrets/Turret.h"

// Sets default values for this component's properties
UTurretInteractionComponent::UTurretInteractionComponent()
{
  PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UTurretInteractionComponent::BeginPlay()
{
  Super::BeginPlay();
}


// Called every frame
void UTurretInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  if (bUpdateNearestTurret)
  {
    GetNearestTurret();
  }
}

void UTurretInteractionComponent::OnOverlapBegin(
  UPrimitiveComponent* OverlappedComponent
  , AActor* OtherActor
  , UPrimitiveComponent* OtherComp
  , int32 OtherBodyIndex
  , bool bBFromSweep
  , const FHitResult& SweepResult)
{
  if (!bUpdateNearestTurret && !InteractedTurret && OtherActor->IsA(ATurret::StaticClass()))
  {
    bUpdateNearestTurret = true;
  }
}

void UTurretInteractionComponent::OnOverlapEnd(
  UPrimitiveComponent* OverlappedComponent
  , AActor* OtherActor
  , UPrimitiveComponent* OtherComp
  , int32 OtherBodyIndex)
{
  if (OtherActor->IsA(ATurret::StaticClass()))
  {
    TArray<AActor*> NearTurrets;
    PlayableCharacter->InteractionCollider->GetOverlappingActors(NearTurrets, ATurret::StaticClass());

    ATurret* Turret = Cast<ATurret>(OtherActor);
    Turret->TurnOffOutline();

    if (IsValid(InteractedTurret))
    {
      if (OtherActor == InteractedTurret && PlayableCharacter->ActualPlayerStatus == EPlayerState::TurretInteraction)
      {
        StopTreatingTurret();
      }
    }

    if (NearTurrets.Num() == 0)
    {
      bUpdateNearestTurret = false;
      NearestTurret = nullptr;
    }
  }
}

void UTurretInteractionComponent::Initialize()
{
  GameMode = Cast<ALunarPunkGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
  PlayableCharacter = Cast<APlayableCharacter>(GetOwner());

  PlayableCharacter->LPPlayerController->AutoMoveTurretsEvent.AddDynamic(this, &UTurretInteractionComponent::AutoMoveTurrets);
  PlayableCharacter->LPPlayerController->PositionTurretEvent.AddDynamic(this, &UTurretInteractionComponent::PositionTurret);
  PlayableCharacter->LPPlayerController->TreatTurretEvent.AddDynamic(this, &UTurretInteractionComponent::TreatTurret);
  PlayableCharacter->LPPlayerController->StopTreatingTurretEvent.AddDynamic(this, &UTurretInteractionComponent::StopTreatingTurret);
  PlayableCharacter->LPPlayerController->UpgradeTurretEvent.AddDynamic(this, &UTurretInteractionComponent::UpgradeTurret);

  PlayableCharacter->InteractionCollider->OnComponentBeginOverlap.AddDynamic(this, &UTurretInteractionComponent::OnOverlapBegin);
  PlayableCharacter->InteractionCollider->OnComponentEndOverlap.AddDynamic(this, &UTurretInteractionComponent::OnOverlapEnd);

  PositioningCollider = Cast<UBoxComponent>(PlayableCharacter->GetDefaultSubobjectByName(TEXT("PositioningCollider")));
}

void UTurretInteractionComponent::GetNearestTurret()
{
  TArray<AActor*> NearTurrets;
  PlayableCharacter->InteractionCollider->GetOverlappingActors(NearTurrets, ATurret::StaticClass());
  if (NearTurrets.Num() > 0)
  {
    float BestDistance;
    if (IsValid(NearestTurret))
    {
      BestDistance = GetOwner()->GetSquaredDistanceTo(NearestTurret);
    }
    else
    {
      BestDistance = MAX_FLT;
    }

    for (AActor* Turret : NearTurrets)
    {
      ATurret* InspectedTurret = Cast<ATurret>(Turret);
      if (IsValid(InspectedTurret))
      {
        if (IsValid(NearestTurret))
        {
          const float NewDistance = GetOwner()->GetSquaredDistanceTo(InspectedTurret);

          if (NewDistance < BestDistance)
          {
            NearestTurret->TurnOffOutline();
            NearestTurret = InspectedTurret;
            BestDistance = NewDistance;
          }
        }
        else
        {
          NearestTurret = InspectedTurret;
          BestDistance = GetOwner()->GetSquaredDistanceTo(NearestTurret);
        }
      }
    }

    if (IsValid(NearestTurret))
    {
      NearestTurret->TurnOnOutline();
    }
  }
  else
  {
    if (IsValid(NearestTurret))
    {
      NearestTurret->TurnOffOutline();
    }
    NearestTurret = nullptr;
  }
}

void UTurretInteractionComponent::AutoMoveTurrets()
{
  bool IsMoving = GameMode->EntityManager->IsAnyTurretMoving();

  AutoMoveTurretsDelegate.Broadcast(IsMoving);
}

void UTurretInteractionComponent::PositionTurret()
{
  if (PlayableCharacter->ActualPlayerStatus == EPlayerState::Default)
  {
    TakeTurret();
  }
  else if (PlayableCharacter->ActualPlayerStatus == EPlayerState::PositioningTurret)
  {
    SetTurret();
  }
}

void UTurretInteractionComponent::TakeTurret()
{
  if (Interact(true) && InteractedTurret->PickUpTurret())
  {
    InteractedTurret->TurnOffOutline();
    InteractedTurret->AttachToActor(GetOwner(), FAttachmentTransformRules::SnapToTargetIncludingScale);
    InteractedTurret->SetActorRelativeLocation(FVector(0, 0, 200));

    if (IsValid(PlayableCharacter) && IsValid(PlayableCharacter->TurretPositionIndicator))
    {
      PlayableCharacter->TurretPositionIndicator->SetVisibility(true);

    }

    PlayableCharacter->ActualPlayerStatus = EPlayerState::PositioningTurret;

  }
  else
  {
    PlayableCharacter->ActualPlayerStatus = EPlayerState::Default;
    Interact(false);
  }
}

void UTurretInteractionComponent::SetTurret()
{
  TArray<AActor*> Obstacles;
  PositioningCollider->GetOverlappingActors(Obstacles);
  
  if (Obstacles.Num() > 0)
  {
    return;
  }

  FVector NewPositionTurret = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * OffsetTurretNewPosition;

  FNavLocation Navmeshlocation;
  UNavigationSystemV1* NavSystem = nullptr;
  UWorld* World = GetWorld();

  if (IsValid(World))
  {
    NavSystem = UNavigationSystemV1::GetCurrent(World);
  }

  // In order to facilitate testing, we check the new position for a turret only when there is a navmesh in the world.
  if (IsValid(NavSystem) && NavSystem->GetMainNavData() && !NavSystem->ProjectPointToNavigation(NewPositionTurret, Navmeshlocation))
  {
    return;
  }

  NewPositionTurret.Z = Navmeshlocation.Location.Z;

  InteractedTurret->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
  InteractedTurret->PlaceTurret();
  InteractedTurret->SetActorLocation(NewPositionTurret);
  InteractedTurret->SetActorRotation(FQuat(0, 0, 0, 0));

  if (IsValid(PlayableCharacter) && IsValid(PlayableCharacter->TurretPositionIndicator))
  {
    PlayableCharacter->TurretPositionIndicator->SetVisibility(false);
  }
  PlayableCharacter->ActualPlayerStatus = EPlayerState::Default;
  Interact(false);
}


void UTurretInteractionComponent::TreatTurret()
{
  
  if (Interact(true) && IsValid(PlayableCharacter->RewardsComponent) && (InteractedTurret->State == ETurretState::Set || InteractedTurret->State == ETurretState::Conquest || InteractedTurret->State == ETurretState::Deactivated))
  {
    if (GameMode->GetPlatesManager()->GetCurrentPlates() >= InteractedTurret->CostRepair())
    {
      InteractedTurret->StartRepairing();
    }
    else
    {
      GameMode->GetPlatesManager()->NotEnoughPlates.Broadcast();
    }
  }
}

void UTurretInteractionComponent::StopTreatingTurret()
{
  if (IsValid(InteractedTurret))
  {
    InteractedTurret->InterruptRepair();
  }
  Interact(false);
}

void UTurretInteractionComponent::UpgradeTurret()
{
  if (Interact(true) && IsValid(PlayableCharacter->RewardsComponent) && (InteractedTurret->State == ETurretState::Conquest || InteractedTurret->State == ETurretState::Set))
  {
    if (InteractedTurret->HealthComponent->GetActualHealth() > 0)
    {
      EUpgradeType Upgrade = GameMode->UpgradesManager->GetCurrentUpgrade();
      FUpgradesDataStruct* UpgradeData = GameMode->UpgradesManager->UpgradesData->Upgrades.Find(Upgrade);
      bool hasUpgrade = GameMode->UpgradesManager->SpendUpgrade(Upgrade, 1);
      if (hasUpgrade) 
      {
          InteractedTurret->UpgradeTurret(UpgradeData, 1);
      }
    }
  }
  Interact(false);
}

bool UTurretInteractionComponent::Interact(bool bStartInteraction)
{
  bool Succesful = false;
  if (bStartInteraction)
  {
    bUpdateNearestTurret = false;
    if (IsValid(NearestTurret))
    {
      InteractedTurret = NearestTurret;
      NearestTurret = nullptr;
      Succesful = true;
    }
    else
    {
      bUpdateNearestTurret = true;
    }
  }
  else
  {
    InteractedTurret = nullptr;
    bUpdateNearestTurret = true;
  }
  return Succesful;
}