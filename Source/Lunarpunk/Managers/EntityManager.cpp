// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/EntityManager.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "NiagaraComponent.h"
#include "Enemies/BasicEnemy.h"
#include "Enemies/IEnemy.h"
#include "Pool/IsPoolable.h"
#include "Enemies/DistanceEnemy.h"
#include "EnemySpawner/EnemySpawner.h"
#include "HealthSystem/HealthComponent.h"
#include "GameFrameworks/LunarPunkGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "LevelLayout/ConquestZone.h"
#include "LevelLayout/ZoneTrigger.h"
#include "Player/PlayableCharacter.h"
#include "Pool/Pool.h"
#include "Portal/Portal.h"
#include "Turrets/Turret.h"
#include "Turrets/AI/MovementTurretComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Player/LunarPunkPlayerController.h"

UEntityManager::UEntityManager()
{

}



void UEntityManager::Initialize(UWorld* _World, APlayableCharacter* PlayableCharacter, ALunarPunkPlayerController* PlayerController)
{
  World = _World;

  if (IsValid(World))
  {
    PlayerCharacter = PlayableCharacter;
    // Get reference to pool and add it to the entity manager 
    LevelPool = Cast<APool>(UGameplayStatics::GetActorOfClass(World, APool::StaticClass()));


    if (IsValid(LevelPool))
    {
      LevelPool->InitializePool();
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("Pool not created"));
    }

    InitEnemySpawners();

    InitPortals();

    InitTurrets();

    InitEnemies();

    InitZoneTriggers();

    InitializeConquestZones();


  #if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT || UE_EDITOR
    InitializeProfilling();
  #endif

  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("EntityManager.cpp: World Not Ready"));
  }

  PlayerController->KillEnemiesEvent.AddDynamic(this, &UEntityManager::KillAllEnemiesInMap);

}

void UEntityManager::InitEnemySpawners()
{
  TArray<AActor*> ActorEnemySpawners;
  UGameplayStatics::GetAllActorsOfClass(World, AEnemySpawner::StaticClass(), ActorEnemySpawners);

  for (AActor* EnemySpawner : ActorEnemySpawners)
  {
    EnemySpawners.Add(Cast<AEnemySpawner>(EnemySpawner));
  }
}

bool UEntityManager::IsAnyTurretMoving()
{
  //TArray<AActor*> OutTurrets;
  //UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATurret::StaticClass(), OutTurrets);
  bool IsMoving = false;
  for (ATurret* ActorTurret : Turrets)
  {
    if (ActorTurret->State == ETurretState::Moving)//TurretMovementComponent->MovementState != ETurretMovementState::Landed)
    {
      IsMoving = true;
      break;
    }
    //APawn* pawn = Cast<APawn>(ActorTurret);
    //if (pawn->GetController())
    //{
    //    bool IsFollowingAPath = pawn->GetController()->IsFollowingAPath();

    //    if (IsFollowingAPath)
    //    {
    //        IsMoving = true;
    //        //return IsMoving;
    //        break;
    //    }
    //}
  }

  return IsMoving;
}

void UEntityManager::InitializeConquestZones()
{
  UGameplayStatics::GetAllActorsOfClass(World, AConquestZone::StaticClass(), ConquestZones);

  //Sorts the sector by their index.
  ConquestZones.Sort([](const AActor& ip1, const AActor& ip2)
    {
      const AConquestZone* auxA = Cast<AConquestZone>(&ip1);
      const AConquestZone* auxB = Cast<AConquestZone>(&ip2);

      if (IsValid(auxA) && IsValid(auxB))
      {
        return auxA->Index < auxB->Index;
      }
      else
      {
        return false;
      }
    });
}


TArray<APortal*>& UEntityManager::GetPortals()
{
  return Portals;
}

void UEntityManager::InitTurrets()
{
  TArray<AActor*> TurretActors;
  UGameplayStatics::GetAllActorsOfClass(World, ATurret::StaticClass(), TurretActors);

  for (AActor* TActor : TurretActors)
  {
    ATurret* T = Cast<ATurret>(TActor);
    if (IsValid(T))
    {
      T->Initialize();
      Turrets.Add(T);
      TurretsDefaultSpeed = T->FloatingMovementComponent->MaxSpeed;
    }
  }

}

void UEntityManager::InitEnemies()
{
  TArray<AActor*> EnemyActors;
  UGameplayStatics::GetAllActorsOfClass(World, ABasicEnemy::StaticClass(), EnemyActors);

  for (AActor* TActor : EnemyActors)
  {
    ABasicEnemy* E = Cast<ABasicEnemy>(TActor);
    if (IsValid(E))
    {
      E->Initialize(LevelPool);
      Enemies.Add(E);
    }
  }

}

void UEntityManager::InitPortals()
{
  TArray<AActor*> PortalActors;
  UGameplayStatics::GetAllActorsOfClass(World, APortal::StaticClass(), PortalActors);
  for (auto Portal : PortalActors)
  {
    Portals.Add(Cast<APortal>(Portal));
  }

}

void UEntityManager::InitZoneTriggers()
{
  TArray<AActor*> ZoneTriggerActors;
  UGameplayStatics::GetAllActorsOfClass(World, AZoneTrigger::StaticClass(), ZoneTriggerActors);
  for (AActor* ZoneTrigger : ZoneTriggerActors)
  {
    ZoneTriggers.Add(Cast<AZoneTrigger>(ZoneTrigger));
  }
}

void UEntityManager::InitNewTurret(ATurret* NewTurret)
{
  if (IsValid(NewTurret))
  {
    NewTurret->Initialize();
    NewTurret->InitializeUpgradeInfo();
    Turrets.Add(NewTurret);
  }
}

void UEntityManager::KillAllEnemiesInMap()
{
  for (ABasicEnemy* Enemy : Enemies)
  {
    IIsPoolable* EnemyCastedPoolable = Cast<IIsPoolable>(Enemy);
    IEnemy* EnemyCasted = Cast<IEnemy>(Enemy);
    if (EnemyCastedPoolable && EnemyCasted && EnemyCastedPoolable->bIsActive)
    {
      EnemyCasted->GetHealthComponent()->InstantKill();
    }
  }
}


void UEntityManager::ChangeTurretsMovementSpeed(float NewSpeed)
{
  for (ATurret* Turret : Turrets)
  {
    Turret->FloatingMovementComponent->MaxSpeed = NewSpeed;
  }
}


float UEntityManager::GetTurretsSpeed()
{
  if (Turrets.Num() > 0 && Turrets[0])
  {
    return Turrets[0]->FloatingMovementComponent->MaxSpeed;
  }

  return 0;
}



float UEntityManager::GetTurretsDefaultSpeed()
{
    return TurretsDefaultSpeed;
}


void UEntityManager::RestoreTurretsSpeed()
{
    ChangeTurretsMovementSpeed(TurretsDefaultSpeed);
}

// !------------------------ PROFILLING --------------------------------------!

void UEntityManager::DeactivateComponents(AActor* Actor)
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT || UE_EDITOR
  if (bDeactivateVFX)
  {
    DeactivateNiagara(Actor);
  }
  if (bDeactivateAI)
  {
    DeactivateAI(Actor);
  }
#endif

}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT || UE_EDITOR

void UEntityManager::InitializeProfilling()
{
  // PROFILLING
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT || UE_EDITOR

  SuscribeToReactivateDelegate();

  if (bDeactivateAI || bDeactivateVFX)
  {
    for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    {
      if (bDeactivateAI)
      {
        DeactivateAI(*ActorItr);
      }

      if (bDeactivateVFX)
      {
        DeactivateNiagara(*ActorItr);
      }
    }
  }
#endif
}





void UEntityManager::DeactivateAI(AActor* Actor)
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT|| UE_EDITOR
  if (Actor->GetInstigatorController())
  {
    AAIController* AIComponent = Cast<AAIController>(Actor->GetInstigatorController());
    if (IsValid(AIComponent))
    {
      //AAIController* AIController = Cast<AAIController>(AIComponent);
      AIComponent->StopMovement();
      if (AIComponent->BrainComponent)
      {
        AIComponent->BrainComponent->Deactivate();
      }
      AIComponent->UnPossess();
    }
  }
#endif

}

void UEntityManager::DeactivateNiagara(AActor* Actor)
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT|| UE_EDITOR
  UActorComponent* NSComponent = Actor->GetComponentByClass(UNiagaraComponent::StaticClass());
  UNiagaraSystem* NSS = Cast<UNiagaraSystem>(Actor);

  if (IsValid(NSComponent))
  {
    Cast<UNiagaraComponent>(NSComponent)->Deactivate();
  }
#endif
}

void UEntityManager::DeactivateAllActiveAI()
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT|| UE_EDITOR
  for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
  {
    DeactivateAI(*ActorItr);
  }
#endif
}

void UEntityManager::DeactivateAllActiveNiagara()
{
  for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
  {
    DeactivateNiagara(*ActorItr);
  }
}


void UEntityManager::SuscribeToReactivateDelegate()
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT || UE_EDITOR
  if (IsValid(LevelPool))
  {
    for (auto& Actor : LevelPool->ClassesToSpawn)
    {
      TArray<AActor*> Actors;
      LevelPool->Spawns.MultiFind(Actor.Key, Actors, true);
      for (AActor* Actorr : Actors)
      {
        Cast<IIsPoolable>(Actorr)->FReactivate.AddDynamic(this, &UEntityManager::DeactivateComponents);
      }
    }
  }
#endif
}

#endif


