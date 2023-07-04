// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "CheckIsInTurretRangeService.generated.h"

/**
 * 
 */
UCLASS()
class LUNARPUNK_API UCheckIsInTurretRangeService : public UBTService
{
	GENERATED_BODY()
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:

	virtual void OnGameplayTaskActivated(UGameplayTask& task) override;

	virtual void OnGameplayTaskDeactivated(UGameplayTask& task) override;
};
