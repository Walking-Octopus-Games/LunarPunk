// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AI/Navigation/NavQueryFilter.h"
#include "DistanceEnemyAIController.generated.h"


class ATurret;

/**
 * 
 */
UCLASS()
class LUNARPUNK_API ADistanceEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBehaviorTree* MyBt;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBlackboardComponent* MyBlackBoard;

	//Target class to look for and shoot
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> TargetClass;

	//Possible targets of the enemy
	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> Targets;

	AActor* CurrentTarget = nullptr;
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	//Handles the shoot action
	void Shoot();

	//Rotates to the given target
	bool RotateToTarget(const AActor* Target, float DeltaTime);

	//Moves to a shoot position
	EPathFollowingRequestResult::Type MoveToShootPosition();

	//Sets the shoot location in the BlackBoard
	void SetShootLocation(const FVector& ShootLocation);

	//Set the nearest target in the BlackBoard
	void SetNearestTarget(AActor* NearestTarget);

	//Obtains the nearest available target
	AActor* GetNearestTarget() const;

	//Return the available targets (that is, targets which are alive)
	TArray<ATurret*> GetAvailableTargets();

	//Obtains a nav mesh point around the target in the given radious.
	FVector GetRandomPointAroundTarget(const AActor* Target, float Radius);

	//Checks if the actor is cornered between the portal and the player.
	bool IsCorneredByPortal(const FVector& Location, FVector& GoalLocation);

	//Obtains the distance to the nearest portal
	float GetDistanceToNearestPortal();

protected:

	virtual void OnPossess(APawn* InPawn) override;

private:

	bool bPawnIsRotatingToShoot = false;
	FRotator GoalRotation;

	UPROPERTY(EditAnywhere)
	float RotationSpeed = 10.0f;

};
