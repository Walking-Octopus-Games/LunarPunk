#include "Enemies/AI/UpdateNexTargetZigZag.h"

#include "AIControllerZigZag.h"

EBTNodeResult::Type UUpdateNexTargetZigZag::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerZigZag* AIEnemyController = Cast<AAIControllerZigZag>(OwnerComp.GetOwner());

	AIEnemyController->UpdateNextTargetPoint();

	return EBTNodeResult::Succeeded;
}

FString UUpdateNexTargetZigZag::GetStaticDescription() const
{
	return TEXT("Actualiza el siguiente punto en el recorrido");
}

void UUpdateNexTargetZigZag::OnGameplayTaskActivated(UGameplayTask& Task)
{
	//Super::OnGameplayTaskActivated(Task);
}
