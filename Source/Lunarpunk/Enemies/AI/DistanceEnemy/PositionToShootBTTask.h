// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "PositionToShootBTTask.generated.h"

/**
 * 
 */
UCLASS()
class LUNARPUNK_API UPositionToShootBTTask : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UPositionToShootBTTask();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//Se llama constantemente, y se usa generalmente para task que se quedan en progress.
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual FString GetStaticDescription() const override;

	virtual void OnGameplayTaskActivated(UGameplayTask& task) override;
	
};
