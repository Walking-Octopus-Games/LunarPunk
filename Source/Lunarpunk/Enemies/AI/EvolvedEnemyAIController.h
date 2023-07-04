// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EvolvedEnemyAIController.generated.h"


/**
 * 
 */
UCLASS()
class LUNARPUNK_API AEvolvedEnemyAIController : public AAIController
{
	GENERATED_BODY()

		UFUNCTION(BlueprintCallable, Category = "Enemy | AI")
		void UpdateNextTargetPoint();

	UFUNCTION()
		void SetValidTargets() const;

	UPROPERTY(EditAnywhere, Category = "Enemy | AI")
		bool bGoOnlyToPlayer = false;

private:
	UFUNCTION()
		bool CheckIsTheActorValid(AActor* Actor) const;
		void UpdateActorBasicEnemy(TArray<AActor*>& ActorTurrets) const;
		void SetEnemyAsTarget(AActor*& Target, FVector& TLocation, bool& bTurretFound, TArray<AActor*>& ActorTurrets) const;
		void SetPlayerAsTarget(AActor*& Target, FVector& TLocation) const;
		void SetBlackboardTarget(UBlackboardComponent* PBlackboardComponent, AActor* Target, FVector TLocation) const;
		void SetBlackBoardPlayerLocation(UBlackboardComponent* PBlackboardComponent) const;
};
