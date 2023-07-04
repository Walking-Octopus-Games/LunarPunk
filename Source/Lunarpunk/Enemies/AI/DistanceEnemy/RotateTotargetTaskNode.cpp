// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/DistanceEnemy/RotateTotargetTaskNode.h"
#include "DistanceEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

URotateTotargetTaskNode::URotateTotargetTaskNode()
{
	bNotifyTick = true;
}

EBTNodeResult::Type URotateTotargetTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ADistanceEnemyAIController* MyController = Cast<ADistanceEnemyAIController>(OwnerComp.GetAIOwner());
	if (MyController)
	{
		GoalActor = Cast<AActor>(MyController->MyBlackBoard->GetValueAsObject(TargetNameInBlackBoard));
	}
	

	return EBTNodeResult::InProgress;
}


//Se llama constantemente, y se usa generalmente para task que se quedan en progress.
void URotateTotargetTaskNode::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{

	ADistanceEnemyAIController* MyController = Cast<ADistanceEnemyAIController>(OwnerComp.GetAIOwner());
	
	if (GoalActor && MyController && MyController->RotateToTarget(GoalActor, DeltaSeconds))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

}

FString URotateTotargetTaskNode::GetStaticDescription() const
{
	return TEXT("Rotates to the current target");
}

void URotateTotargetTaskNode::OnGameplayTaskActivated(UGameplayTask& task)
{

}