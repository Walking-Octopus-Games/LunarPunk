// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootBTTask.h"
#include "DistanceEnemyAIController.h"




EBTNodeResult::Type UShootBTTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	ADistanceEnemyAIController* MyController = Cast<ADistanceEnemyAIController>(OwnerComp.GetAIOwner());

	if (MyController)
	{
		MyController->Shoot();
	}

	return EBTNodeResult::Succeeded;
}


FString UShootBTTask::GetStaticDescription() const {

	return TEXT("Shoots the projectile");
}

void UShootBTTask::OnGameplayTaskActivated(UGameplayTask& task)
{

}