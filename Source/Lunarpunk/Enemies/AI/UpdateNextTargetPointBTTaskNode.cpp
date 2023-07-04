// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/UpdateNextTargetPointBTTaskNode.h"

#include "EnemyAIController.h"

EBTNodeResult::Type UUpdateNextTargetPointBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
  AEnemyAIController* AIEnemyController = Cast<AEnemyAIController>(OwnerComp.GetOwner());

  AIEnemyController->UpdateNextTargetPoint();

  return EBTNodeResult::Succeeded;
}

FString UUpdateNextTargetPointBTTaskNode::GetStaticDescription() const
{
  return TEXT("Actualiza el siguiente punto en el recorrido");
}

void UUpdateNextTargetPointBTTaskNode::OnGameplayTaskActivated(UGameplayTask& Task)
{
  //Super::OnGameplayTaskActivated(Task);
}
