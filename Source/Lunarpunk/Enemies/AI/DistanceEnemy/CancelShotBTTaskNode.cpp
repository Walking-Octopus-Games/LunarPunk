// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/DistanceEnemy/CancelShotBTTaskNode.h"
#include "Enemies/DistanceEnemy.h"
#include "DistanceEnemyAIController.h"
#include "HealthSystem/HealthComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UCancelShotBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	ADistanceEnemyAIController* MyController = Cast<ADistanceEnemyAIController>(OwnerComp.GetAIOwner());
	
	if (MyController)
	{
		ADistanceEnemy* MyPawn = Cast<ADistanceEnemy> (MyController->GetPawn());

		if (MyPawn)
		{
			MyPawn->DeactivateShotChargeEffect();
			MyPawn->CurrentDistanceToShootPercentageModifier *= MyPawn->PercentageModifierInDistanceToPositionToShot;
		}
	}

	

	return EBTNodeResult::Succeeded;

}

FString UCancelShotBTTaskNode::GetStaticDescription() const
{
	return TEXT("Cancels the shot and the shot effects.");
}

void UCancelShotBTTaskNode::OnGameplayTaskActivated(UGameplayTask& task)
{

}