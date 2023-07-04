// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/DistanceEnemy/CheckIfPlayerIsNearBTTaskNode.h"
#include "DistanceEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayableCharacter.h"
#include "Enemies/DistanceEnemy.h"
#include "Utilities/UtilitiesLunarpunk.h"




EBTNodeResult::Type UCheckIfPlayerIsNearBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//Checks if the player is in the self defense radius.
	ADistanceEnemyAIController* MyController = Cast<ADistanceEnemyAIController>(OwnerComp.GetAIOwner());
	if (MyController)
	{
		ADistanceEnemy* MyPawn = Cast<ADistanceEnemy>(MyController->GetPawn());
		APlayableCharacter* Player = Cast<APlayableCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

		if (Player && MyPawn)
		{
			FVector ShootLocation = MyPawn->GetShootLocation();
			if (!UUtilitiesLunarpunk::CheckIsAnythingBetweenActorAndTarget(GetWorld(), ShootLocation, Player))
			{
				FVector DistToPlayer = Player->GetActorLocation() - MyController->GetPawn()->GetActorLocation();
				float SelfDefenseRangeSquared = 10000 * (MyPawn->SelfDefenseRange * MyPawn->SelfDefenseRange);
				//If the player is in the self defense radius, then set up him as target
				if (DistToPlayer.SizeSquared() <= SelfDefenseRangeSquared)
				{
					MyController->CurrentTarget = Player;

					//Sets the player in current target in the black board
					MyController->GetBlackboardComponent()->SetValueAsObject(FName("Target"), MyController->CurrentTarget);
					MyController->GetBlackboardComponent()->SetValueAsVector(FName("TargetLocation"), MyController->CurrentTarget->GetActorLocation());
				}
			}
		}
	}

	return EBTNodeResult::Succeeded;
}



FString UCheckIfPlayerIsNearBTTaskNode::GetStaticDescription() const
{
	return TEXT("Checks if the player is into the selfdefense range and shoots him instead of the current target.");
}

void UCheckIfPlayerIsNearBTTaskNode::OnGameplayTaskActivated(UGameplayTask& task)
{

}