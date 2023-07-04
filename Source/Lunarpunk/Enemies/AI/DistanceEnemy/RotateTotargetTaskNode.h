// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "RotateTotargetTaskNode.generated.h"

/**
 * 
 */
UCLASS()
class LUNARPUNK_API URotateTotargetTaskNode : public UBTTaskNode
{
	GENERATED_BODY()

public:


	URotateTotargetTaskNode();


	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual FString GetStaticDescription() const override;

	virtual void OnGameplayTaskActivated(UGameplayTask& task) override;

	UPROPERTY(EditAnywhere, Category = "Config")
	FName TargetNameInBlackBoard;

private:

	AActor* GoalActor;
	
};
