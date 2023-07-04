// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/DistanceEnemy/ObtainSafePointBTTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DistanceEnemyAIController.h"
#include "Turrets/Turret.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "NavigationData.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Enemies/DistanceEnemy.h"



EBTNodeResult::Type UObtainSafePointBTTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	//Obtain the controller and pawn location
	ADistanceEnemyAIController* MyController = Cast<ADistanceEnemyAIController>(OwnerComp.GetAIOwner());
	ADistanceEnemy* MyPawn = Cast<ADistanceEnemy>(MyController->GetPawn());

	if (MyController && MyPawn)
	{
		FVector Location = MyController->GetPawn()->GetActorLocation();

		//Obtain the navigation system.
		UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

		//Obtain the query filter class of the controller:
		TSubclassOf <UNavigationQueryFilter> FilterClass = MyController->GetDefaultNavigationFilterClass();

		float MaxRange = MyController->GetBlackboardComponent()->GetValueAsFloat(FName("MaxRangeTurrets"));
		float DistToPortal = MyController->GetDistanceToNearestPortal();
		FVector LocationBestPoint;
		if (MaxRange > DistToPortal)
		{
			LocationBestPoint = MyPawn->GetActorLocation();
		}
		else
		{
			LocationBestPoint = ObtainSafePointWithMaxDistance(
				Location,
				MaxRange,
				MyPawn->NumRowsSafePoints,
				MyPawn->NumColumnsSafePoints,
				MyController->GetPawn()->GetActorForwardVector(),
				MyController->GetAvailableTargets(),
				MyController,
				NavSys,
				MyPawn->bDrawDebugSpheresForSafePoingMovement,
				MyPawn->DrawDuration
			);

			//Nav Mesh Method.
			/*LocationBestPoint = ObtainSafePoint(
				Location,
				MaxRange,
				MyPawn->NumRowsSafePoints,
				MyPawn->NumColumnsSafePoints,
				MyController->GetPawn()->GetActorForwardVector(),
				MyController,
				NavSys,
				FilterClass,
				MyPawn->bDrawDebugSpheresForSafePoingMovement,
				MyPawn->DrawDuration
			);*/
		}

		MyController->GetBlackboardComponent()->SetValueAsVector(FName("SafeLocation"), LocationBestPoint);
		MyController->GetBlackboardComponent()->SetValueAsFloat(FName("MaxRangeTurrets"), -1.0f);
	}

	return EBTNodeResult::Succeeded;

}

FString UObtainSafePointBTTask::GetStaticDescription() const
{
	return TEXT("Obtains a location far away of the turrets");
}

void UObtainSafePointBTTask::OnGameplayTaskActivated(UGameplayTask& task)
{

}


FVector UObtainSafePointBTTask::ObtainSafePoint(const FVector& Center, float Radius, const int32 Rows, const int32 Columns, FVector Forward, ADistanceEnemyAIController* Controller, UNavigationSystemV1* NavSys, TSubclassOf <UNavigationQueryFilter> FilterClass, bool DrawSpheres, float DrawDuration, const FVector& Offset) const
{
	

	FVector LocationBestPoint = Center;

	if (Rows == 0 || Columns == 0 || Radius == 0)
	{
		return LocationBestPoint;
	}

	if (!Controller || !NavSys)
	{
		return LocationBestPoint;
	}

	//Obtain the navigation data of the controller.
	const ANavigationData* NavData = NavSys->GetNavDataForProps(Controller->GetNavAgentPropertiesRef());
	FSharedConstNavQueryFilter QueryFilter;
	if (NavData)
	{
		//Get an instance of the query filter:
		QueryFilter = UNavigationQueryFilter::GetQueryFilter(*NavData, Controller, FilterClass);
	}
	else
	{
		return LocationBestPoint;
	}


	//Angle and distance between points
	float Angle = 360.0 / Columns;
	float DistRows = Radius / Rows;
	FRotator Rot = FRotator(0.0, Angle, 0.0);

	//Cost of the points in the nav mesh
	float Cost;
	bool bFirstCost = true;
	float MinCost = 0;


	FVector LocationPoint = Center;
	FVector NavMeshLocationPoint;
	FNavLocation LocNavMesh;

	for (int i = 0; i < Columns; ++i)
	{
		//Starting in the center
		LocationPoint = Center;
		//Rotate the forward the required angle
		Forward = Rot.RotateVector(Forward);

		for (int j = 0; j < Rows; ++j)
		{
			LocationPoint += DistRows * Forward;

			//Project the point in the nav mesh and calculate its cost if it's actually into the nav mesh.
			if (NavSys->ProjectPointToNavigation(LocationPoint, LocNavMesh))
			{
				NavMeshLocationPoint = LocNavMesh.Location + Offset;

				if (DrawSpheres)
				{
#if !UE_BUILD_SHIPPING
					DrawDebugSphere(GetWorld(), NavMeshLocationPoint, 50, 3, FColor(180, 0, 0), false, DrawDuration, 0, 2);


					FColor drawColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f).ToFColor(true);
					bool drawShadow = true;
					DrawDebugString(GEngine->GetWorldFromContextObject(this), NavMeshLocationPoint + FVector(0.0, 0.0, 10.0), *FString::Printf(TEXT("%s[%f]"), TEXT("Cost:"), Cost), NULL, drawColor, DrawDuration, drawShadow);
#endif //!UE_BUILD_SHIPPING
				}
				
				//Gets the location cost in the nav mesh.
				NavSys->GetPathCost(NavMeshLocationPoint, NavMeshLocationPoint + FVector(1.0, 0.0, 0.0), Cost, NavData, QueryFilter);

				//Adds the distance cost. The farther the better.
				Cost -= 10 * j;

				if (bFirstCost)
				{
					MinCost = Cost;
					LocationBestPoint = NavMeshLocationPoint;
					bFirstCost = false;
				}
				if (Cost < MinCost)
				{
					MinCost = Cost;
					LocationBestPoint = NavMeshLocationPoint;
				}
				
			}


		}
	}
#if !UE_BUILD_SHIPPING
	if (DrawSpheres)
	{
		DrawDebugSphere(GetWorld(), LocationBestPoint, 50, 3, FColor(0, 180, 0), false, DrawDuration, 0, 2);
	}
#endif //!UE_BUILD_SHIPPING

	return LocationBestPoint;
}



FVector UObtainSafePointBTTask::ObtainSafePointWithMaxDistance(const FVector& Center, float Radius, const int32 Rows, const int32 Columns, FVector Forward, const TArray<ATurret*>& Targets, ADistanceEnemyAIController* Controller, UNavigationSystemV1* NavSys, bool DrawSpheres, float DrawDuration, const FVector& Offset) const
{


	FVector LocationBestPoint = Center;

	if (Rows == 0 || Columns == 0 || Radius == 0)
	{
		return LocationBestPoint;
	}

	if (!Controller || !NavSys)
	{
		return LocationBestPoint;
	}

	//Angle and distance between points
	float Angle = 360.0 / Columns;
	float DistRows = Radius / Rows;
	FRotator Rot = FRotator(0.0, Angle, 0.0);

	int NumTargets = Targets.Num();

	if (NumTargets == 0)
	{
		return LocationBestPoint;
	}
	TArray<float> SquaredRanges;
	for (int i = 0; i < NumTargets; ++i)
	{
		SquaredRanges.Add((Targets[i]->GetRange()) * (Targets[i]->GetRange()));
	}

	FVector LocationPoint = Center;
	FVector NavMeshLocationPoint;
	FNavLocation LocNavMesh;

	bool bIsOutsideRange = false;
	float SquaredDistance = 0;

	for (int i = 0; i < Rows; ++i)
	{
		if (bIsOutsideRange)
		{
			break;
		}
	
		for (int j = 0; j < Columns; ++j)
		{
			//Starting in farthest point in forward direction.
			LocationPoint = Center + (Radius - i * DistRows) * Forward;
			bIsOutsideRange = true;
			for (int n = 0; n < NumTargets; ++n)
			{
				SquaredDistance = FVector::DistSquaredXY(LocationPoint, Targets[n]->GetActorLocation());
				if (SquaredDistance < SquaredRanges[n])
				{
					bIsOutsideRange = false;
					break;
				}
			}

			//Project the point in the nav mesh and draw it.
			if (NavSys->ProjectPointToNavigation(LocationPoint, LocNavMesh))
			{
				NavMeshLocationPoint = LocNavMesh.Location + Offset;

				if (DrawSpheres)
				{
#if !UE_BUILD_SHIPPING
					DrawDebugSphere(GetWorld(), NavMeshLocationPoint, 50, 3, FColor(180, 0, 0), false, DrawDuration, 0, 2);
#endif //!UE_BUILD_SHIPPING
				}

				if (bIsOutsideRange)
				{
					LocationBestPoint = NavMeshLocationPoint;
					break;
				}

			}
			//Rotate the forward the required angle
			Forward = Rot.RotateVector(Forward);

		}
		
	}
#if !UE_BUILD_SHIPPING
	if (DrawSpheres)
	{
		DrawDebugSphere(GetWorld(), LocationBestPoint, 50, 3, FColor(0, 180, 0), false, DrawDuration, 0, 2);
	}
#endif //!UE_BUILD_SHIPPING

	return LocationBestPoint;
}