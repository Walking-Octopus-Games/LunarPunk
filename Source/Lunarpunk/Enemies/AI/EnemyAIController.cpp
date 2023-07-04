// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "BrainComponent.h"
#include "EngineUtils.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "HealthSystem/HealthComponent.h"
#include "Turrets/Turret.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/EntityManager.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Navigation/CrowdManager.h"
#include "Player/PlayableCharacter.h"
#include "Turrets/AI/EnemyTargetPoints.h"
#include "Navigation/CrowdFollowingComponent.h"
//
AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
  CrowdFollowingComponent = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent());
  //CrowdFollowingComponent->SetCrowdSeparationWeight(3, true);
  //CrowdFollowingComponent->SetCrowdSeparation(true, true);
  //CrowdFollowingComponent->SetAvoidanceGroup(1);
  //CrowdFollowingComponent->SetCrowdCollisionQueryRange(10.f);
  //CrowdFollowingComponent->SetCrowdObstacleAvoidance(true);
  //CrowdFollowingComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);
}


void AEnemyAIController::UpdateNextTargetPoint()
{
  if (IsValid(BrainComponent))
  {
    if (!IsValid(PBlackboardComponent))
    {
      PBlackboardComponent = BrainComponent->GetBlackboardComponent();
    }
    SetValidTargets();
  }
}

void AEnemyAIController::RemovePlayerAsTarget()
{
  if (IsValid(PBlackboardComponent))
  {

    PBlackboardComponent->SetValueAsObject(FName("Player"), nullptr);
  }
}

void AEnemyAIController::SetTurretBlackboardTarget(AActor* Actor)
{
  if (IsValid(PBlackboardComponent))
  {
    PBlackboardComponent->SetValueAsObject(FName("TurretTarget"), Actor);
  }
}

void AEnemyAIController::SetValidTargets()
{
  if (IsValid(PBlackboardComponent))
  {
    AActor* TurretTarget = nullptr;

    UObject* TurretTargetInBlackBoard = PBlackboardComponent->GetValueAsObject("TurretTarget");
    AActor* TurretTargetActorInBlackBoard = Cast<AActor>(TurretTargetInBlackBoard);
    if (!CheckIsTheActorValid(TurretTargetActorInBlackBoard))
    {
      bool bTurretsFound = false;
      SetTurretAsTarget(TurretTarget, bTurretsFound);
      if (!bTurretsFound)
      {
        SetPlayerAsTarget();
      }
      else
      {
        RemovePlayerAsTarget();
        SetTurretBlackboardTarget(TurretTarget);
        GetRandomPointInTurretArea(TurretTargetActorInBlackBoard);
      }
    }
    else
    {
      RemovePlayerAsTarget();
      if (!ETP)
      {
        UpdateRelativeAreaPosition(TurretTargetActorInBlackBoard);
      }
      else
      {
        FNavLocation ResultLocation;
        FVector Extent = FVector(220, 220, 220);
        bool bs = NavSystem->ProjectPointToNavigation(ETP->GetComponentLocation(), ResultLocation, Extent);
        if (bs)
        {
          PBlackboardComponent->SetValueAsVector("TTPPosition", ResultLocation);
        }
        else
        {
          NavSystem->GetRandomReachablePointInRadius(ETP->GetComponentLocation(), 400, ResultLocation);
          PBlackboardComponent->SetValueAsVector("TTPPosition", ResultLocation);
        }
      }
      //SetTurretBlackboardTarget(TurretTarget);
    }
  }
}

void AEnemyAIController::UpdateActorTurrets(TArray<AActor*>& ActorTurrets) const
{
  for (TActorIterator<ATurret> It(GetWorld()); It; ++It)
  {
    ATurret* TargetPoint = *It;
    ActorTurrets.Add(TargetPoint);
  }
}

void AEnemyAIController::UpdateRelativeAreaPosition(AActor* Turret)
{

  TargetAreaPosition = Turret->GetActorLocation() - RelativePosition;

  ATurret* TurretT = Cast<ATurret>(Turret);
  if (IsValid(Turret))
  {
    USphereComponent* AreaTarget = TurretT->AreaForEnemies;
    FNavLocation ResultLocation;
    FVector Extent = FVector(AreaTarget->GetScaledSphereRadius(), AreaTarget->GetScaledSphereRadius(), AreaTarget->GetScaledSphereRadius());
    bool bs = NavSystem->ProjectPointToNavigation(TargetAreaPosition, ResultLocation, Extent);
    if (bs)
    {
      PBlackboardComponent->SetValueAsVector("TargetAreaPosition", ResultLocation);
    }
    else
    {
      NavSystem->GetRandomReachablePointInRadius(TargetAreaPosition, AreaTarget->GetScaledSphereRadius(), ResultLocation);
      PBlackboardComponent->SetValueAsVector("TargetAreaPosition", ResultLocation);
    }
  }
}

void AEnemyAIController::GetRandomPointInTurretArea(AActor* TurretTarget)
{
  if (IsValid(PBlackboardComponent))
  {

    ATurret* Turret = Cast<ATurret>(TurretTarget);
    if (IsValid(Turret))
    {
      USphereComponent* AreaTarget = Turret->AreaForEnemies;
      float Radius = AreaTarget->GetScaledSphereRadius();

      FNavLocation TurretNavLocation;
      NavSystem->GetRandomReachablePointInRadius(Turret->GetActorLocation(), Radius, TurretNavLocation);

      float Distance = FVector::Distance(Turret->TargetPoint1->GetComponentLocation(), Turret->GetActorLocation());
      int32 Tries = 0;
      while (FVector::Distance(TurretNavLocation.Location, Turret->GetActorLocation()) < Distance && Tries < 10)
      {
        NavSystem->GetRandomReachablePointInRadius(Turret->GetActorLocation(), Radius, TurretNavLocation);
        Tries++;
      }

      TargetAreaPosition = TurretNavLocation.Location;

      RelativePosition = Turret->GetActorLocation() - TargetAreaPosition;

      //UpdateRelativeAreaPosition(Turret);
      PBlackboardComponent->SetValueAsVector("TargetAreaPosition", TargetAreaPosition);
    }
  }

}

void AEnemyAIController::SetTurretAsTarget(AActor*& Target, bool& bTurretFound)
{
  TArray<ATurret*> Turrets = LPGameMode->EntityManager->Turrets;
  if (Turrets.Num() > 0)
  {
    TArray<int32> IndexesChecked;
    int32 Index = FMath::RandRange(0, Turrets.Num() - 1);
    IndexesChecked.Add(Index);
    while (IndexesChecked.Num() < Turrets.Num() && !CheckIsTheActorValid(Turrets[Index]))
    {
      Index = FMath::RandRange(0, Turrets.Num() - 1);

      if (!IndexesChecked.Contains(Index))
      {
        IndexesChecked.Add(Index);
      }
    }

    if (CheckIsTheActorValid(Turrets[Index]))
    {
      Target = Turrets[Index];
      GetRandomPointInTurretArea(Target);
      Turrets[Index]->TargetForEnemiesHandlerComponent->GetETPAndAssignItToAFreeEnemy();
      bTurretFound = true;
    }

  }

}

void AEnemyAIController::BeginPlay()
{
  Super::BeginPlay();
  LPGameMode = Cast<ALunarPunkGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
  if (IsValid(GetWorld()))
  {
    NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
  }

}

void AEnemyAIController::SetPlayerAsTarget()
{
  if (IsValid(LPGameMode->EntityManager) && IsValid(PBlackboardComponent))
  {

    APlayableCharacter* Target = LPGameMode->EntityManager->PlayerCharacter;
    if (IsValid(Target))
    {
      PBlackboardComponent->SetValueAsObject(FName("Player"), Target);
    }
  }
}

bool AEnemyAIController::IsAnyTurretAvailable(bool& bValue1)
{
  for (ATurret* Turret : LPGameMode->EntityManager->Turrets)
  {
    if (CheckIsTheActorValid(Turret))
    {
      bValue1 = true;
      return true;
    }
  }
  return false;
}

bool AEnemyAIController::CheckIsTheActorValid(AActor* Actor)
{
  FNavLocation Navmeshlocation;
  //UNavigationPath* Generatedpath = NavSystem->FindPathToActorSynchronously(GetWorld(), GetInstigator()->GetNavAgentLocation(), Actor);
  if (IsValid(Actor) && Cast<UHealthComponent>(Actor->GetComponentByClass(UHealthComponent::StaticClass()))->GetActualHealth() > 0.0f/* && IsValid(Generatedpath) && Generatedpath->PathPoints.Num() > 0*/)
  {
    if (Actor->IsA<APlayableCharacter>())
    {
      if (IsValid(LPGameMode->EntityManager))
      {
        bool bValue1;
        return !IsAnyTurretAvailable(bValue1);
      }
    }

    ATurret* TurretCasted = Cast<ATurret>(Actor);
    if (IsValid(TurretCasted) && (TurretCasted->State == ETurretState::Set || TurretCasted->State == ETurretState::Conquest || TurretCasted->State == ETurretState::Moving))
    {
      return true;
    }

  }

  return false;
}

bool AEnemyAIController::IsETPAssigned()
{
  if (IsValid(ETP))
  {
    return true;
  }
  return false;
}

void AEnemyAIController::AssignETP(UEnemyTargetPoints* EtpToEnemy)
{
  if (IsValid(EtpToEnemy) && IsValid(PBlackboardComponent))
  {
    ETP = EtpToEnemy;

    PBlackboardComponent->SetValueAsObject("TurretTargetPoint", ETP);
    PBlackboardComponent->SetValueAsVector("TTPPosition", ETP->GetComponentLocation());
  }
}

void AEnemyAIController::RemoveETP()
{
  if (IsValid(ETP))
  {
    ETP->SetbIsOcuppied(false);
    ETP = nullptr;
    //PBlackboardComponent->SetValueAsObject("TurretTargetPoint", nullptr);
  }
  DeleteBBValues();
}

void AEnemyAIController::DeleteBBValues()
{
  if (IsValid(PBlackboardComponent))
  {
    PBlackboardComponent->ClearValue("TurretTargetPoint");
    PBlackboardComponent->ClearValue("Player");
    PBlackboardComponent->ClearValue("TargetAreaPosition");
    PBlackboardComponent->ClearValue("TTPPosition");
    PBlackboardComponent->ClearValue("TurretTarget");
  }

}

