// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/DistanceEnemy/WaitWithArgumentsBTTaskNode.h"
#include "Enemies/AI/DistanceEnemy/DistanceEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UWaitWithArgumentsBTTaskNode::UWaitWithArgumentsBTTaskNode()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UWaitWithArgumentsBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	ADistanceEnemyAIController* MyController = Cast<ADistanceEnemyAIController>(OwnerComp.GetAIOwner());

	if (MyController && TimeBind.IsValid())
	{
		Time = MyController->GetBlackboardComponent()->GetValueAsFloat(TimeBind);
		CurrentTime = 0.0f;
	}

	return EBTNodeResult::InProgress;
}


void UWaitWithArgumentsBTTaskNode::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{

	CurrentTime += DeltaSeconds;

	if (CurrentTime >= Time)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}



FString UWaitWithArgumentsBTTaskNode::GetStaticDescription() const
{
	return FString("The AI waits the specified time.");
}

void UWaitWithArgumentsBTTaskNode::OnGameplayTaskActivated(UGameplayTask& task)
{

}