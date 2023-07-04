// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/DistanceEnemy/SelectShootLocationTaskNode.h"
#include "DistanceEnemyAIController.h"
#include "HealthSystem/HealthComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/DistanceEnemy.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

EBTNodeResult::Type USelectShootLocationTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ADistanceEnemyAIController* MyController = Cast<ADistanceEnemyAIController>(OwnerComp.GetAIOwner());

	if (MyController)
	{
		AActor* NearestTarget = MyController->GetNearestTarget();
		FVector ShootLocation;

		//If there is no valid turret target, then select the player.
		if (!NearestTarget)
		{
			NearestTarget = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		}

		MyController->SetNearestTarget(NearestTarget);

		if (NearestTarget)
		{
			ShootLocation = MyController->GetRandomPointAroundTarget(NearestTarget, 6.0f);
			MyController->SetShootLocation(ShootLocation);
			ADistanceEnemy* MyPawn = Cast<ADistanceEnemy>(MyController->GetPawn());

#if !UE_BUILD_SHIPPING
			if (MyPawn && MyPawn->bDrawDebugSpheresForShootLocationReference)
			{
				DrawDebugSphere(GetWorld(), ShootLocation, 50, 3, FColor(180, 0, 0), false, MyPawn->DrawDuration, 0, 2);
			}
#endif //!UE_BUILD_SHIPPING
			
		}
		
		return EBTNodeResult::Succeeded;
		
	}
	
	return EBTNodeResult::Failed;
}

FString USelectShootLocationTaskNode::GetStaticDescription() const
{
	return FString("Select a position in the level to shoot.");
}

void USelectShootLocationTaskNode::OnGameplayTaskActivated(UGameplayTask& task)
{

}



