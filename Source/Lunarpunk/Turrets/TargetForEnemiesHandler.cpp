// Fill out your copyright notice in the Description page of Project Settings.


#include "Turrets/TargetForEnemiesHandler.h"

#include "AIController.h"
#include "Turret.h"
#include "AI/EnemyTargetPoints.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Enemies/BasicEnemy.h"
#include "Enemies/AI/EnemyAIController.h"
#include "HealthSystem/HealthComponent.h"

// Sets default values for this component's properties
UTargetForEnemiesHandler::UTargetForEnemiesHandler()
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;

  // ...

}

void UTargetForEnemiesHandler::TurretDeactivated()
{
  for (ABasicEnemy* BasicEnemy : EnemiesInsideTheArea)
  {
    if (IsValid(BasicEnemy->GetController()))
    {
      AEnemyAIController* BasicController = Cast<AEnemyAIController>(BasicEnemy->GetController());

      if (IsValid(OwnerTurret) && IsValid(BasicController))
        for (UEnemyTargetPoints* EnemyTargetPoints : OwnerTurret->EnemyTargetPoints)
        {
          if (IsValid(BasicController->ETP) && IsValid(EnemyTargetPoints))
          {
            if (BasicController->ETP == EnemyTargetPoints)
            {
              BasicController->RemoveETP();
            }
          }
        }
    }
  }
}

void UTargetForEnemiesHandler::TurretReactivated()
{
  GetETPAndAssignItToAFreeEnemy();
}

void UTargetForEnemiesHandler::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult)
{
  if (IsValid(OtherActor) && OtherActor->IsA(ABasicEnemy::StaticClass()) && OtherComp->IsA(UCapsuleComponent::StaticClass()))
  {
    EnemiesInsideTheArea.Add(Cast<ABasicEnemy>(OtherActor));
    GetETPAndAssignItToAFreeEnemy();
  }
}


void UTargetForEnemiesHandler::GetAFreeETP(float PointDistanceToEnemy, UEnemyTargetPoints*& ETPToEnemy)
{
  for (UEnemyTargetPoints* ETP : OwnerTurret->EnemyTargetPoints)
  {
    if (!ETP->IsOcuppied())
    {
      float ActualPointDistanceToEnemy = FVector::Distance(ETP->GetComponentLocation(), OwnerTurret->GetActorLocation());

      if (ActualPointDistanceToEnemy < PointDistanceToEnemy)
      {
        PointDistanceToEnemy = ActualPointDistanceToEnemy;
        ETPToEnemy = ETP;
      }

    }
  }
}

ABasicEnemy* UTargetForEnemiesHandler::GetFreeEnemy()
{

  for (ABasicEnemy* BasicEnemy : EnemiesInsideTheArea)
  {
    AEnemyAIController* EnemyController = Cast<AEnemyAIController>(BasicEnemy->AICharacterController);
    if (IsValid(EnemyController) && !EnemyController->IsETPAssigned())
    {
      return BasicEnemy;
    }
  }
  return nullptr;
}

void UTargetForEnemiesHandler::GetETPAndAssignItToAFreeEnemy()
{
  float PointDistanceToEnemy = MAX_FLT;
  UEnemyTargetPoints* ETPToEnemy = nullptr;
  GetAFreeETP(PointDistanceToEnemy, ETPToEnemy);
  if (ETPToEnemy)
  {
    ABasicEnemy* BasicEnemy = GetFreeEnemy();
    if (IsValid(BasicEnemy))
    {
      AEnemyAIController* EnemyController = Cast<AEnemyAIController>(BasicEnemy->AICharacterController);
      EnemyController->AssignETP(ETPToEnemy);
      if (EnemyController->PBlackboardComponent)
      {
        EnemyController->PBlackboardComponent->SetValueAsObject("TurretTarget", OwnerTurret);
      }
      ETPToEnemy->SetbIsOcuppied(true);
    }
  }
}

void UTargetForEnemiesHandler::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
  if (IsValid(OtherActor) && OtherActor->IsA(ABasicEnemy::StaticClass()) && OtherComp->IsA(UCapsuleComponent::StaticClass()))
  {
    if (EnemiesInsideTheArea.Contains(OtherActor))
    {
      // if the enemy was in the  target point
      // Search the closest enemy to the free point
      // Assign new target to enemy
      ABasicEnemy* Enemy = Cast<ABasicEnemy>(OtherActor);
      AEnemyAIController* EnemyController = Cast<AEnemyAIController>(Enemy->GetController());
      if (IsValid(EnemyController) && Enemy->HealthComponent->GetActualHealth() > 0.f)
      {
        EnemyController->RemoveETP();
      }
      EnemiesInsideTheArea.Remove(Enemy);
      GetETPAndAssignItToAFreeEnemy();
    }
  }

}

// Called when the game starts
void UTargetForEnemiesHandler::Initialize(ATurret* Turret)
{
  //Super::BeginPlay();
  if (IsValid(Turret))
  {
    if (IsValid(Turret->AreaForEnemies))
    {
      OwnerTurret = Turret;
      Turret->AreaForEnemies->OnComponentBeginOverlap.AddDynamic(this, &UTargetForEnemiesHandler::BeginOverlap);
      Turret->AreaForEnemies->OnComponentEndOverlap.AddDynamic(this, &UTargetForEnemiesHandler::EndOverlap);
      Turret->TurretDeath.AddDynamic(this, &UTargetForEnemiesHandler::TurretDeactivated);
      Turret->TurretReactivated.AddDynamic(this, &UTargetForEnemiesHandler::TurretReactivated);
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("TargetForEnemiesHandler: Area not being binded, enemies wont work"));
    }

  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("TargetForEnemiesHandler: Turret not valid, enemies wont work"));
  }
}

// Called every frame
void UTargetForEnemiesHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // ...
}

