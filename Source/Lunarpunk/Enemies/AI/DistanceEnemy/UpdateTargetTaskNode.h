// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "UpdateTargetTaskNode.generated.h"

class ADistanceEnemy;

/**
 * 
 */

class ATurret;

UCLASS()
class LUNARPUNK_API UUpdateTargetTaskNode : public UBTTaskNode
{
	GENERATED_BODY()

public:


	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FString GetStaticDescription() const override;

	virtual void OnGameplayTaskActivated(UGameplayTask& task) override;

private:

	ATurret* ChooseBestTarget(const ADistanceEnemy* OriginActor, const TArray<ATurret*>& AllTargets);
};
