// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/DistanceEnemy/ObtainNearPointTaskNode.h"
#include "DistanceEnemyAIController.h"
#include "Enemies/DistanceEnemy.h"
#include "NavigationSystem.h"
#include "HealthSystem/HealthComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

EBTNodeResult::Type UObtainNearPointTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ADistanceEnemyAIController* MyController = Cast<ADistanceEnemyAIController>(OwnerComp.GetAIOwner());

	if (MyController)
	{

		ADistanceEnemy* MyPawn = Cast<ADistanceEnemy>(MyController->GetPawn());

		float Distance = MyPawn->DistanceToMoveAfterShooting;
		float DistanceToPortal = 0.01*MyController->GetDistanceToNearestPortal();
		FVector NewLocation;

		//Checks if the portal is near.
		if (Distance > DistanceToPortal)
		{
			NewLocation = MyPawn->GetActorLocation();
		}
		else
		{
			NewLocation = MyController->GetRandomPointAroundTarget(MyPawn, MyPawn->DistanceToMoveAfterShooting);
		}

		MyController->SetShootLocation(NewLocation);

#if !UE_BUILD_SHIPPING
		if (MyPawn && MyPawn->bDrawDebugSpheresForShootLocationReference)
		{
			DrawDebugSphere(GetWorld(), NewLocation, 50, 3, FColor(180, 0, 0), false, MyPawn->DrawDuration, 0, 2);
		}
#endif //!UE_BUILD_SHIPPING

		return EBTNodeResult::Succeeded;

	}

	return EBTNodeResult::Failed;
}

FString UObtainNearPointTaskNode::GetStaticDescription() const
{
	return FString("Obtain a near random point in the nav mesh.");
}

void UObtainNearPointTaskNode::OnGameplayTaskActivated(UGameplayTask& task)
{

}



