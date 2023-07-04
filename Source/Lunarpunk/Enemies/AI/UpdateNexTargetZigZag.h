// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "UpdateNexTargetZigZag.generated.h"

/**
 * 
 */
UCLASS()
class LUNARPUNK_API UUpdateNexTargetZigZag : public UBTTaskNode
{
	GENERATED_BODY()

		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

		virtual FString GetStaticDescription() const override;

public:
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;
	
};
