// Fill out your copyright notice in the Description page of Project Settings.


#include "DistanceEnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EngineUtils.h"
#include "Enemies/DistanceEnemy.h"
#include "Turrets/Turret.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "Enemies/DistanceEnemy.h"
#include "Components/SphereComponent.h"
#include "HealthSystem/HealthComponent.h"
#include "Portal/Portal.h"
#include "Kismet/KismetSystemLibrary.h"



void ADistanceEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(MyBt);

	MyBlackBoard = GetBrainComponent()->GetBlackboardComponent();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TargetClass, Targets);

	//Add the player to the possibles targets
	Targets.Add(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

}

void ADistanceEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (MyBlackBoard)
	{
		MyBlackBoard->SetValueAsFloat(FName("MaxRangeTurrets"), -1.0f);


		ADistanceEnemy* MyPawn = Cast<ADistanceEnemy>(InPawn);
		if (MyPawn)
		{
			MyBlackBoard->SetValueAsFloat(FName("TimeToChargeShot"), MyPawn->TimeToChargeShot);
			MyBlackBoard->SetValueAsFloat(FName("RestingTimeAfterShooting"), MyPawn->RestingTimeAfterShooting);
		}
	}

}

void ADistanceEnemyAIController::Tick(float DeltaTime)
{
	
}

void ADistanceEnemyAIController::Shoot()
{

	ADistanceEnemy* MyPawn = Cast<ADistanceEnemy>(GetPawn());
	if (MyPawn)
	{
		MyPawn->DeactivateShotChargeEffect();
		MyPawn->Shoot();
		//MyPawn->OwnArea->SetCanEverAffectNavigation(false);
	}

}



EPathFollowingRequestResult::Type ADistanceEnemyAIController::MoveToShootPosition()
{
	//Query filter class of the actor:
	TSubclassOf <UNavigationQueryFilter> FilterClass = GetDefaultNavigationFilterClass();
	ADistanceEnemy* MyPawn = Cast<ADistanceEnemy>(GetPawn());
	
	EPathFollowingRequestResult::Type Result = MoveToLocation(
		MyBlackBoard->GetValueAsVector(FName("ShootLocation")), 
		100*MyPawn->DistanceToPositionToShoot*MyPawn->CurrentDistanceToShootPercentageModifier,
		true, 
		true, 
		false, 
		true, 
		FilterClass
	);

	return Result;
}



bool ADistanceEnemyAIController::RotateToTarget(const AActor* Target, float DeltaTime)
{
	APawn* MyPawn = GetPawn();

	if (!MyPawn || !Target)
	{
		return false;
	}

	if (!bPawnIsRotatingToShoot)
	{
		bPawnIsRotatingToShoot = true;
		GoalRotation = UKismetMathLibrary::FindLookAtRotation(MyPawn->GetActorLocation(), Target->GetActorLocation());
		GoalRotation.Pitch = 0;
		GoalRotation.Roll = 0;
	}
	else
	{
		if (FMath::Abs(MyPawn->GetActorRotation().Yaw - GoalRotation.Yaw) >= 1.0f)
		{
			MyPawn->SetActorRotation(FMath::Lerp(MyPawn->GetActorRotation(), GoalRotation, DeltaTime * RotationSpeed));
			return false;
		}
		else
		{
			bPawnIsRotatingToShoot = false;
			return true;
		}
	}
	
	return false;

}


void ADistanceEnemyAIController::SetShootLocation(const FVector& ShootLocation)
{
	if (MyBlackBoard)
	{
		MyBlackBoard->SetValueAsVector(FName("ShootLocation"), ShootLocation);
	}
}



void ADistanceEnemyAIController::SetNearestTarget(AActor* NearestTarget)
{
	if (MyBlackBoard)
	{
		MyBlackBoard->SetValueAsObject(FName("NearestTarget"), NearestTarget);
	}
}


AActor* ADistanceEnemyAIController::GetNearestTarget() const
{
	AActor* NearestActor = nullptr;
	float DistanceSquared = -1.0f;
	float DistanceSquaredToTarget = 0.0f;

	APawn* MyPawn = GetPawn();
	if (MyPawn)
	{
		for (AActor* Target : Targets)
		{
			//Other option: store only the available targets in an array and sort it by squared distance using a lambda.
			ATurret* TurretCasted = Cast<ATurret>(Target);
			if (TurretCasted && TurretCasted->State != ETurretState::Deactivated && TurretCasted->State != ETurretState::PickedUp)
			{
				UHealthComponent* HealthComponent = Target->FindComponentByClass<UHealthComponent>();
				if (HealthComponent && HealthComponent->GetActualHealth() > 0)
				{
					DistanceSquaredToTarget = (MyPawn->GetActorLocation() - Target->GetActorLocation()).SizeSquared();
					if (DistanceSquared < 0)
					{
						DistanceSquared = DistanceSquaredToTarget;
						NearestActor = Target;
					}
					else
					{
						if (DistanceSquaredToTarget < DistanceSquared)
						{
							DistanceSquared = DistanceSquaredToTarget;
							NearestActor = Target;
						}
					}
				}
			}
		}
	}

	return NearestActor;
}


TArray<ATurret*> ADistanceEnemyAIController::GetAvailableTargets()
{
	TArray<ATurret*> AvailableTargets;
	ATurret* TargetCasted;
	for (AActor* Target : Targets)
	{
		TargetCasted = Cast<ATurret>(Target);
		if (TargetCasted && TargetCasted->State != ETurretState::Deactivated && TargetCasted->State != ETurretState::PickedUp)
		{
			AvailableTargets.Add(TargetCasted);
		}
	}

	return AvailableTargets;
}


FVector ADistanceEnemyAIController::GetRandomPointAroundTarget(const AActor* Target, float Radius)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	FVector Location;

	FRotator Rot = FRotator(0.0, FMath::FRandRange(0.0, 360.0), 0.0);
	float Dist = FMath::FRandRange(0.0, Radius);
	FVector NewLocation = Target->GetActorForwardVector();
	NewLocation = Rot.RotateVector(NewLocation);
	NewLocation = Target->GetActorLocation() + 100 * Dist * NewLocation;

	FNavLocation LocInNavMesh;

	if (NavSys->ProjectPointToNavigation(NewLocation, LocInNavMesh))
	{
		Location = LocInNavMesh.Location;
	}
	else
	{
		Location = NavSys->ProjectPointToNavigation(GetWorld(), Target->GetActorLocation());
	}

	return Location;
}


bool ADistanceEnemyAIController::IsCorneredByPortal(const FVector& Location, FVector& GoalLocation)
{

	TArray<FHitResult> OutHits;

	GetWorld()->LineTraceMultiByChannel(
		OutHits,
		Location,
		GoalLocation,
		ECC_WorldDynamic
	);
	
	AActor* ActorHitted;
	for (const FHitResult& Hit : OutHits)
	{
		ActorHitted = Hit.GetActor();
		if (Cast<APortal>(ActorHitted))
		{
			GoalLocation = ActorHitted->GetActorLocation();
			return true;
		}
	}

	return false;
}


float ADistanceEnemyAIController::GetDistanceToNearestPortal()
{

	TArray<AActor*> Portals;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APortal::StaticClass(), Portals);
	float MinDistance = -1.0f;
	float DistanceToPortal = 0.0f;
	if (GetPawn())
	{
		for (AActor* Portal : Portals)
		{
			if (IsValid(Portal))
			{
				DistanceToPortal = FVector::DistSquaredXY(GetPawn()->GetActorLocation(), Portal->GetActorLocation());
				if (MinDistance < 0)
				{
					MinDistance = DistanceToPortal;
				}

				if (MinDistance > DistanceToPortal)
				{
					MinDistance = DistanceToPortal;
				}
			}
			
		}
	}
	

	return FMath::Sqrt(MinDistance);
}