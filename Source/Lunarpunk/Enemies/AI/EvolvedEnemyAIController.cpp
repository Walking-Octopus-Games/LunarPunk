// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/EvolvedEnemyAIController.h"
#include <Enemies/BasicEnemy.h>
#include "HealthSystem/HealthComponent.h"
#include "BrainComponent.h"
#include "EngineUtils.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayableCharacter.h"

void AEvolvedEnemyAIController::UpdateNextTargetPoint()
{
    SetValidTargets();
}

void AEvolvedEnemyAIController::SetValidTargets() const
{
    UBlackboardComponent* PBlackboardComponent = BrainComponent->GetBlackboardComponent();
    AActor* Target = nullptr;
    FVector TLocation;

    UObject* TargetInBlackBoard = PBlackboardComponent->GetValueAsObject("Target");
    AActor* TargetActorInBlackBoard = Cast<AActor>(TargetInBlackBoard);

    //PlayerLocation
    UWorld* WorldP = GetWorld();
    APlayerController* ControllerP = WorldP->GetFirstPlayerController();
    APawn* PawnP = ControllerP->GetPawn();
    FVector PlayerLocation = PawnP->GetActorLocation();

    //EvolvedEnemyLocation
    FVector EvolvedEnemyLocation = GetPawn()->GetActorLocation();

    //Distance between PlayerLocation and EvolvedEnemyLocation
    float Distance = FVector::Dist(PlayerLocation, EvolvedEnemyLocation);
 

    if (!CheckIsTheActorValid(TargetActorInBlackBoard) || Cast<APlayableCharacter>(TargetActorInBlackBoard) != nullptr)
    {
        if (!bGoOnlyToPlayer)
        {
            bool bEnemyFound = false;
            TArray<AActor*> ActorEnemy;

            UpdateActorBasicEnemy(ActorEnemy);

            SetEnemyAsTarget(Target, TLocation, bEnemyFound, ActorEnemy);

            if (!bEnemyFound || Distance<1000.0f)
            {
                SetPlayerAsTarget(Target, TLocation);
            }
        }
        else
        {
            SetPlayerAsTarget(Target, TLocation);
        }
    }
    else
    {
        TLocation = TargetActorInBlackBoard->GetActorLocation();
    }


    SetBlackboardTarget(PBlackboardComponent, Target, TLocation);

    SetBlackBoardPlayerLocation(PBlackboardComponent);
}





void AEvolvedEnemyAIController::UpdateActorBasicEnemy(TArray<AActor*>& ActorBasicEnemy) const
{
    for (TActorIterator<ABasicEnemy> It(GetWorld()); It; ++It)
    {
        ABasicEnemy* TargetPoint = *It;

        if (CheckIsTheActorValid(TargetPoint))
        {
            ActorBasicEnemy.Add(TargetPoint);
        }
    }
}
//¿Get pawn está bien?
void AEvolvedEnemyAIController::SetEnemyAsTarget(AActor*& Target, FVector& TLocation, bool& bTurretFound, TArray<AActor*>& ActorEnemy) const
{
   
    if (ActorEnemy.Num() > 0)
    {
        bTurretFound = true;

        AActor* ClosestEnemy = nullptr;
        float ClosestDistance = FLT_MAX;
        for (AActor* Enemy : ActorEnemy)
        {
            if (Enemy)
            {
                float Distance = FVector::Dist(Enemy->GetActorLocation(), GetPawn()->GetActorLocation());
                if (Distance < ClosestDistance)
                {
                    ClosestDistance = Distance;
                    ClosestEnemy = Enemy;
                }
            }
        }

        Target = ClosestEnemy;
        TLocation = ClosestEnemy->GetActorLocation();
    }



}




//Me falta comprobar que es de tipo básico ya que el evolucionado hereda del básico
bool AEvolvedEnemyAIController::CheckIsTheActorValid(AActor* Actor) const
{
    if (IsValid(Actor) && Cast<UHealthComponent>(Actor->GetComponentByClass(UHealthComponent::StaticClass()))->GetActualHealth() > 0.0f
         )
       //&& Actor->GetClass()->IsA(BP_EvolvedEnemy::StaticClass()

       
    {
        return true;
    }

    return false;
}


void AEvolvedEnemyAIController::SetPlayerAsTarget(AActor*& Target, FVector& TLocation) const
{
    UWorld* WorldP = GetWorld();
    if (IsValid(WorldP))
    {
        APlayerController* ControllerP = WorldP->GetFirstPlayerController();
        if (IsValid(ControllerP))
        {
            APawn* PawnP = ControllerP->GetPawn();
            if (IsValid(PawnP))
            {
                Target = PawnP;
                TLocation = Target->GetActorLocation();
            }
        }
    }

}



void AEvolvedEnemyAIController::SetBlackboardTarget(UBlackboardComponent* PBlackboardComponent, AActor* Target, FVector TLocation) const
{
    if (TLocation != PBlackboardComponent->GetValueAsVector("TargetLocation"))
    {
        PBlackboardComponent->SetValueAsVector("TargetLocation", TLocation);
        //PBlackboardComponent->SetValueAsObject("Target", Cast<UObject>(Target));
        PBlackboardComponent->SetValueAsObject("Target",Target);
    }
}

void AEvolvedEnemyAIController::SetBlackBoardPlayerLocation(UBlackboardComponent* PBlackboardComponent) const
{
    UWorld* WorldP = GetWorld();
    if (IsValid(WorldP))
    {
        APlayerController* ControllerP = WorldP->GetFirstPlayerController();
        if (IsValid(ControllerP))
        {
            APawn* PawnP = ControllerP->GetPawn();
            if (IsValid(PawnP))
            {
                PBlackboardComponent->SetValueAsVector("PlayerLocation", PawnP->GetActorLocation());
            }
        }

    }
}

