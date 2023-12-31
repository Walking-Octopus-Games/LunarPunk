// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavigationSystem.h"
#include "AIController.h"

#include "Player/PlayableCharacter.h"
#include "AIControllerZigZag.generated.h"


class ALunarPunkGameMode;
/**
 * 
 */
UCLASS()
class LUNARPUNK_API AAIControllerZigZag : public AAIController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Enemy | AI")
		void UpdateNextTargetPoint();
	UPROPERTY()
		ALunarPunkGameMode* LPGameMode;

	UPROPERTY()
		UNavigationSystemV1* NavSystem;

	UPROPERTY()
		UBlackboardComponent* PBlackboardComponent;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "OutsideTarget")
		FVector OutsideDestination(FVector TargetVecto) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "OutsideTarget")
		void OutsideDestinationEvent();
		


	FNavLocation NavLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Location)
		FVector TLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Location)
		bool PlayerisNewTarget = false;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Location)
		FVector PLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Location)
		FVector NewTLocation;


	UFUNCTION()
		void SetValidTargets() ;

	void SetPlayerAsTarget(AActor*& Target, FVector& TLocation) const;

	void SetBlackboardTarget(UBlackboardComponent* PBlackboardComponent, AActor* Target, FVector& TLocation) ;

	void SetBlackBoardPlayerLocation(UBlackboardComponent* PBlackboardComponent) const;

	void OnPossess(APawn* InPawn) override;

	void SetPlayerAsTarget();

	void BeginPlay();




protected:
	
	UPROPERTY(EditDefaultsOnly, Category = BehaviorTree) UBehaviorTree* Behavior { nullptr };


};
