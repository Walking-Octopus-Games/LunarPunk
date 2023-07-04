// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/DistanceEnemy/CheckIsInTurretRangeService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DistanceEnemyAIController.h"
#include "Turrets/Turret.h"

void UCheckIsInTurretRangeService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
  Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);


  //Checks if the actor is in turrets range and write de max range in the black board.

  ADistanceEnemyAIController* MyController = Cast<ADistanceEnemyAIController>(OwnerComp.GetAIOwner());

  if (IsValid(MyController) && IsValid(MyController->GetPawn()))
  {
    FVector Location = MyController->GetPawn()->GetActorLocation();


    float MaxRange = -1.0f;
    float DistToTurret = 0.0f;
    float RangeTurret = 0.0f;
    float RangeTurretSquared = 0.0f;

    for (const ATurret* Turret : MyController->GetAvailableTargets())
    {

      RangeTurret = Turret->GetRange();
      RangeTurretSquared = RangeTurret * RangeTurret;

      if (RangeTurret > MaxRange)
      {
        DistToTurret = FVector::DistSquaredXY(Location, Turret->GetActorLocation());

        if (DistToTurret <= RangeTurretSquared)
        {
          MaxRange = RangeTurret;
        }

      }
    }

    MyController->GetBlackboardComponent()->SetValueAsFloat(FName("MaxRangeTurrets"), MaxRange);


  }


}


void UCheckIsInTurretRangeService::OnGameplayTaskActivated(UGameplayTask& task)
{

}


void UCheckIsInTurretRangeService::OnGameplayTaskDeactivated(UGameplayTask& task)
{

}