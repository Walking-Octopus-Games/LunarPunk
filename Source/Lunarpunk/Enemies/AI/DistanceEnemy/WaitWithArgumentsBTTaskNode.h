// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "WaitWithArgumentsBTTaskNode.generated.h"

/**
 * 
 */
UCLASS()
class LUNARPUNK_API UWaitWithArgumentsBTTaskNode : public UBTTaskNode
{
	GENERATED_BODY()
	
public:

	//Name in the Black Board of the time to wait
	UPROPERTY(EditAnywhere, Category = "Config.")
	FName TimeBind;

	UWaitWithArgumentsBTTaskNode();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//Se llama constantemente, y se usa generalmente para task que se quedan en progress.
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual FString GetStaticDescription() const override;

	virtual void OnGameplayTaskActivated(UGameplayTask& task) override;

private:

	float Time = 1.0f;
	float CurrentTime = 0.0f;
};
