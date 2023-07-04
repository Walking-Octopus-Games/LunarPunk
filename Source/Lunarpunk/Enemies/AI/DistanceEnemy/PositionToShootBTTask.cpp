// Fill out your copyright notice in the Description page of Project Settings.


#include "PositionToShootBTTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DistanceEnemyAIController.h"
#include "Enemies/DistanceEnemy.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"



UPositionToShootBTTask::UPositionToShootBTTask()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UPositionToShootBTTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	return EBTNodeResult::InProgress;
}


void UPositionToShootBTTask::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{

	ADistanceEnemyAIController* MyController = Cast<ADistanceEnemyAIController>(OwnerComp.GetAIOwner());

	if (MyController)
	{
		EPathFollowingRequestResult::Type Result = MyController->MoveToShootPosition();


		if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			//Start to shooting
			ADistanceEnemy* MyPawn = Cast<ADistanceEnemy>(MyController->GetPawn());
			if (MyPawn)
			{
				MyPawn->ActivateShotChargeEffect();
				//MyPawn->OwnArea->SetCanEverAffectNavigation(true);
			}

			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}

	
}


FString UPositionToShootBTTask::GetStaticDescription() const
{
	return FString("Moves to the shoot position.");
}

void UPositionToShootBTTask::OnGameplayTaskActivated(UGameplayTask& task)
{

}