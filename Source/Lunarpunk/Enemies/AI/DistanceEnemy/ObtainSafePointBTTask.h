// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ObtainSafePointBTTask.generated.h"

class ADistanceEnemyAIController;
class UNavigationSystemV1;
class ATurret;

/**
 * 
 */
UCLASS()
class LUNARPUNK_API UObtainSafePointBTTask : public UBTTaskNode
{
	GENERATED_BODY()


public:


	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FString GetStaticDescription() const override;

	virtual void OnGameplayTaskActivated(UGameplayTask& task) override;

private:

	FVector ObtainSafePoint(
		const FVector& Center, 
		float Radius, 
		const int32 Rows, 
		const int32 Columns, 
		FVector Forward, 
		ADistanceEnemyAIController* Controller, 
		UNavigationSystemV1* NavSys, 
		TSubclassOf <UNavigationQueryFilter> FilterClass, 
		bool DrawSpheres = false, 
		float DrawDuration = 5.0f, 
		const FVector& Offset = FVector(0.0f, 0.0f, 20.0f) 
	) const;



	FVector ObtainSafePointWithMaxDistance(
		const FVector& Center,
		float Radius,
		const int32 Rows,
		const int32 Columns,
		FVector Forward,
		const TArray<ATurret*>& Targets,
		ADistanceEnemyAIController* Controller,
		UNavigationSystemV1* NavSys,
		bool DrawSpheres = false,
		float DrawDuration = 5.0f,
		const FVector& Offset = FVector(0.0f, 0.0f, 20.0f)
	) const;
	
};
