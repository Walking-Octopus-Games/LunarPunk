// Fill out your copyright notice in the Description page of Project Settings.


#include "UpdateTargetTaskNode.h"
#include "DistanceEnemyAIController.h"
#include "HealthSystem/HealthComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Turrets/Turret.h"
#include "Enemies/DistanceEnemy.h"
#include "Player/PlayableCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Utilities/UtilitiesLunarpunk.h"

EBTNodeResult::Type UUpdateTargetTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	ADistanceEnemyAIController* MyController = Cast<ADistanceEnemyAIController>(OwnerComp.GetAIOwner());

	if (MyController)
	{
		ADistanceEnemy* MyPawn = Cast<ADistanceEnemy>(MyController->GetPawn());

		if (MyPawn)
		{
			FVector PawnLocation = MyPawn->GetActorLocation();
			FVector ShootLocation = MyPawn->GetShootLocation();


			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
			TArray<ATurret*> Turrets;

			UUtilitiesLunarpunk::ObtainActorsInRange<ATurret>(
				GetWorld(),
				PawnLocation,
				MyPawn->ShootingRange,
				Turrets,
				ObjectTypes,
				MyPawn->bDrawDebugSpheresForShootLocationReference,
				MyPawn->DrawDuration
				);



			/*TArray<ATurret*> Turrets = ObtainTurretsInRange(
				PawnLocation, 
				MyPawn->ShootingRange, 
				MyPawn->bDrawDebugSpheresForShootLocationReference, 
				MyPawn->DrawDuration
			);*/

			MyController->CurrentTarget = ChooseBestTarget(MyPawn, Turrets);

			//If there's not current turret target then set up the player.
			if (!MyController->CurrentTarget)
			{
				APlayableCharacter* Player = Cast<APlayableCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
				bool bCanShootThePlayer = true;

				if (!MyPawn->bShootThroughWalls && Player && UUtilitiesLunarpunk::CheckIsAnythingBetweenActorAndTarget(GetWorld(), ShootLocation, Player))
				{
					bCanShootThePlayer = false;
				}

				if (bCanShootThePlayer)
				{
					MyController->CurrentTarget = Player;
				}
			}

			MyController->GetBlackboardComponent()->SetValueAsObject(FName("Target"), MyController->CurrentTarget);

			if (MyController->CurrentTarget)
			{
				MyController->GetBlackboardComponent()->SetValueAsVector(FName("TargetLocation"), MyController->CurrentTarget->GetActorLocation());
			}
		}
	}

	return EBTNodeResult::Succeeded;

}

FString UUpdateTargetTaskNode::GetStaticDescription() const
{
	return TEXT("Updates the target to shoot.");
}

void UUpdateTargetTaskNode::OnGameplayTaskActivated(UGameplayTask& task)
{

}


ATurret* UUpdateTargetTaskNode::ChooseBestTarget(const ADistanceEnemy* OriginActor, const TArray<ATurret*>& AllTargets)
{
	ATurret* BestTarget = nullptr;
	bool bCanShootTheTarget = true;
	const FVector Origin = OriginActor->GetActorLocation();
	const UWorld* WorldContext = GetWorld();

	if (!WorldContext)
	{
		return BestTarget;
	}

	for (ATurret* Target : AllTargets)
	{
		if (Target->State != ETurretState::Deactivated && Target->State != ETurretState::PickedUp)
		{

			//Checks if the enemy can see the target (and if this matters or not).
			bCanShootTheTarget = true;
			if (!OriginActor->bShootThroughWalls && UUtilitiesLunarpunk::CheckIsAnythingBetweenActorAndTarget(WorldContext, Origin, Target))
			{
				bCanShootTheTarget = false;
			}

			//Choose the target by its health.
			if (bCanShootTheTarget)
			{
				UHealthComponent* HealthComponent = Target->FindComponentByClass<UHealthComponent>();
				if (HealthComponent && HealthComponent->GetActualHealth() > 0)
				{
					if (!BestTarget)
					{
						BestTarget = Target;
					}
					else
					{
						UHealthComponent* HealthComponentCurrentTarget = BestTarget->FindComponentByClass<UHealthComponent>();
						if (HealthComponentCurrentTarget && HealthComponentCurrentTarget->GetActualHealth() < HealthComponent->GetActualHealth())
						{
							BestTarget = Target;
						}
					}
				}
			}
			
		}
	}

	return BestTarget;
	
}