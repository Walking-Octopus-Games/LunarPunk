// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/AIControllerZigZag.h"
#include <Perception/AIPerceptionTypes.h>
#include <BehaviorTree/BlackboardComponent.h>
#include "BrainComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/EntityManager.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "NavigationSystem.h"
#include "Components/SphereComponent.h"
#include <Enemies/BasicEnemy.h>


void AAIControllerZigZag::UpdateNextTargetPoint()
{

	SetValidTargets();


}



void AAIControllerZigZag::SetValidTargets() 
{

	if (!BrainComponent)
	{
		return;
	}

	PBlackboardComponent = BrainComponent->GetBlackboardComponent();
	AActor* Target = nullptr;
	
	if (!PBlackboardComponent)
	{
		return;
	}

	NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem) 
	{
		return;
	}

	const float SearchRadius = 100.0f;
	//FNavLocation NavLocation;
	
	


	//UWorld* WorldP = GetWorld();
	//APlayerController* ControllerP = WorldP->GetFirstPlayerController();
	//APawn* PawnP = ControllerP->GetPawn();
	//Target = PawnP;
	// 
	APawn* PawnControlled = AController::GetPawn();

	if (!PawnControlled)
	{
		return;
	}
	/*
	if (PawnControlled != nullptr) {
		ABasicEnemy* BasicEnemy = Cast<ABasicEnemy>(PawnControlled);
		if (BasicEnemy != nullptr) {
			USphereComponent* Sphere = BasicEnemy->GetSphereToFollow();
			if (Sphere != nullptr) {
				ABasicEnemy* Sphereee = Cast<ABasicEnemy>(Sphere);
				
				Target = Sphereee;
				
			}
		}
	}
	*/

	ABasicEnemy* BasicEnemy = Cast<ABasicEnemy>(PawnControlled);
	USphereComponent* Sphere = nullptr;
	if (BasicEnemy)
	{
		Sphere = BasicEnemy->GetSphereToFollow();
	}
	
	
	if (Sphere == nullptr ) {

		PlayerisNewTarget = true;
		PBlackboardComponent->SetValueAsBool("PlayerisNewTarget", PlayerisNewTarget);

		UWorld* WorldP = GetWorld();
		APlayerController* ControllerP = WorldP->GetFirstPlayerController();
		APawn* PawnP = ControllerP->GetPawn();
		Target = PawnP;
		TLocation = PawnP->GetActorLocation();

		PBlackboardComponent->SetValueAsVector("PlayerLocation", TLocation);


		SetPlayerAsTarget();
		
		
		

		//SetBlackboardTarget(PBlackboardComponent, Target, TLocation);
	

	}
	else {

		PlayerisNewTarget = false;
		PBlackboardComponent->SetValueAsBool("PlayerisNewTarget", PlayerisNewTarget);

		TLocation = Sphere->GetComponentLocation();
		APawn* SphereToFollow = Cast<APawn>(Sphere);
		Target = SphereToFollow;

		
		if (!NavSystem->ProjectPointToNavigation(TLocation, NavLocation)) {

		
			OutsideDestinationEvent();
		
			SetBlackboardTarget(PBlackboardComponent, Target, NewTLocation);
			
		}
		else {

			SetBlackboardTarget(PBlackboardComponent, Target, TLocation);

		}

	}

	
	//SetBlackboardTarget(PBlackboardComponent, Target, TLocation);


}

void AAIControllerZigZag::SetBlackboardTarget(UBlackboardComponent* InPBlackboardComponent, AActor* Target, FVector& TLocationBB) 
{
	


	if (TLocationBB != InPBlackboardComponent->GetValueAsVector("TargetLocation"))
	{
		InPBlackboardComponent->SetValueAsVector("TargetLocation", TLocationBB);
		InPBlackboardComponent->SetValueAsObject("Target", Target);
		
	}
}


void AAIControllerZigZag::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!Behavior) return;
	RunBehaviorTree(Behavior);
}


void AAIControllerZigZag::SetPlayerAsTarget()
{
	if (IsValid(LPGameMode->EntityManager) && IsValid(PBlackboardComponent))
	{

		APlayableCharacter* Target = LPGameMode->EntityManager->PlayerCharacter;
		if (IsValid(Target))
		{
			PBlackboardComponent->SetValueAsObject(FName("Player"), Target);
		}
	}
}




void AAIControllerZigZag::BeginPlay()
{
	Super::BeginPlay();
	LPGameMode = Cast<ALunarPunkGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (IsValid(GetWorld()))
	{
		NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	}
}
