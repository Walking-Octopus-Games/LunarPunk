// Fill out your copyright notice in the Description page of Project Settings.


#include "Turrets/AI/MovementTurretComponent.h"

#include "Turrets/Turret.h"
#include "TurretWaypoint.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayableCharacter.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Player/Turrets/TurretInteractionComponent.h"
#include "Components/SphereComponent.h"

// Sets default values for this component's properties
UMovementTurretComponent::UMovementTurretComponent()
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UMovementTurretComponent::BeginPlay()
{
  Super::BeginPlay();

  //FloatingMovementComponent = Cast<UFloatingPawnMovement>(GetOwner()->GetComponentByClass(UFloatingPawnMovement::StaticClass()));

  TurretOwner = Cast<ATurret>(GetOwner());
  TurretOwner->FloatingMovementComponent->MaxSpeed = MaxTurretSpeed;

  AIController = Cast<AAIController>(TurretOwner->GetController());
  if (IsValid(AIController))
  {
    AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &UMovementTurretComponent::MoveCompleted);
  }


  SearchClosestTurretWaypoint();

  APlayableCharacter* PlayableCharacter = Cast<APlayableCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
  if (IsValid(PlayableCharacter))
  {
    PlayableCharacter->TurretInteractionComponent->AutoMoveTurretsDelegate.AddDynamic(this, &UMovementTurretComponent::ManageTurretMovement);
  }

  //Store de Z location at begin play.
  if (TurretOwner)
  {
    InitialHeight = TurretOwner->GetActorLocation().Z;
  }

  AvoidCollisionRange = Cast<USphereComponent>(GetOwner()->GetDefaultSubobjectByName(CollisionName));

  if (AvoidCollisionRange)
  {
    AvoidCollisionRange->OnComponentBeginOverlap.AddDynamic(this, &UMovementTurretComponent::OnBeginOverlapAvoidRange);
    AvoidCollisionRange->OnComponentEndOverlap.AddDynamic(this, &UMovementTurretComponent::OnEndOverlapAvoidRange);
    AvoidCollisionRange->SetSphereRadius(AvoidanceRadius);

    if (!bAvoidOtherTurrets)
    {
      AvoidCollisionRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
  }



}


void UMovementTurretComponent::MoveTurret()
{
  AIController->MoveToLocation(DestinationPosition, AcceptanceRadius, false, true, true, true, 0, false);
  MovementState = ETurretMovementState::GoingToDestination;
  TurretOwner->State = ETurretState::Moving;
  TurretMovingEvent.Broadcast();
}

// Called every frame
void UMovementTurretComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // ...
  //Manage the unlanding of the owner of the component.
  if (MovementState == ETurretMovementState::Unlanding)
  {
    float DistanceToIncrease = DeltaTime * UnlandingSpeed;
    TurretOwner->AddActorWorldOffset(FVector(0.0, 0.0, DistanceToIncrease));

    if (TurretOwner->GetActorLocation().Z >= InitialHeight + UnlandingHeight)
    {
      //Start to go to the destination position when unlanded.
      MoveTurret();
      //Adjust the landed location to avoid unexpected acumulative offsets.
      FVector Location = TurretOwner->GetActorLocation();
      Location.Z = InitialHeight + UnlandingHeight;
      TurretOwner->SetActorLocation(Location);
    }
  }

  //Manage the landing of the owner of the component.
  if (MovementState == ETurretMovementState::Landing)
  {
    float DistanceToDecrease = DeltaTime * UnlandingSpeed;
    TurretOwner->AddActorWorldOffset(FVector(0.0, 0.0, -DistanceToDecrease));

    if (TurretOwner->GetActorLocation().Z <= InitialHeight)
    {

      //Stop the movement
      MovementState = ETurretMovementState::Landed;
      TurretOwner->StopMovementEffect();

      //Adjust the landed location to avoid unexpected acumulative offsets,
      FVector Location = TurretOwner->GetActorLocation();
      Location.Z = InitialHeight;
      TurretOwner->SetActorLocation(Location);

      //If the turret is not deactivated or in conquest position, then Change its state to set (the player can interact with it)
      if (TurretOwner->State != ETurretState::Deactivated && TurretOwner->State != ETurretState::Conquest)
      {
        TurretOwner->State = ETurretState::Set;
      }
      TurretStopMovingEvent.Broadcast();
    }
  }


  //Manage the turret avoidance
  if (bAvoidOtherTurrets && TurretOwner->State == ETurretState::Moving && MovementState == ETurretMovementState::GoingToDestination && OverlappedTurrets.Num() > 0)
  {
    FVector ResultantOffset = FVector(0, 0, 0);

    for (ATurret* OtherTurret : OverlappedTurrets)
    {
      FVector Direction = (OtherTurret->GetActorLocation() - TurretOwner->GetActorLocation()).GetSafeNormal();
      Direction.Z = 0;
      FVector OwnVel = TurretOwner->GetVelocity().GetSafeNormal();
      OwnVel.Z = 0;
      FVector RotatedRightDirection = FRotator(0.0, 90.0f, 0.0).RotateVector(Direction);
      float AngleForward = FVector::DotProduct(OwnVel, Direction);
      float AngleRight = FVector::DotProduct(OwnVel, RotatedRightDirection);

      if (AngleForward > 0)
      {
        if (AngleRight > 0)
        {
          ResultantOffset += (RotatedRightDirection - Direction) * AvoidanceSpeed * DeltaTime;
        }
        else
        {
          ResultantOffset -= (RotatedRightDirection + Direction) * AvoidanceSpeed * DeltaTime;
        }
      }

    }
    TurretOwner->GetRootComponent()->AddWorldOffset(ResultantOffset, false, nullptr, ETeleportType::TeleportPhysics);

  }
}



void UMovementTurretComponent::SearchClosestTurretWaypoint()
{
  TArray<AActor*> TurretWaypoints;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATurretWaypoint::StaticClass(), TurretWaypoints);
  float BestDistance = MAX_FLT;
  AActor* ClosestTurretWaypoint = nullptr;

  for (AActor* TurretWaypoint : TurretWaypoints)
  {
    ATurretWaypoint* TWP = Cast<ATurretWaypoint>(TurretWaypoint);

    if (TWP->IsActive)
    {
      float DistanceToWaypoint = TurretOwner->GetDistanceTo(TurretWaypoint);
      if (DistanceToWaypoint < BestDistance)
      {
        ClosestTurretWaypoint = TurretWaypoint;
        BestDistance = DistanceToWaypoint;
      }
    }
  }
  if (IsValid(ClosestTurretWaypoint))
  {
    ATurretWaypoint* TWP = Cast<ATurretWaypoint>(ClosestTurretWaypoint);
    if (IsValid(TWP))
    {
      TurretOwner->TargetWaypoint = TWP;
    }
  }
}


void UMovementTurretComponent::ManageTurretMovement(bool IsMoving)
{
  if (IsValid(AIController))
  {
    if (IsMoving)
    {
      AIController->StopMovement();

      //If the turret is moving and his movement state is unlanding or going to destination, then it has to land.
      if (MovementState == ETurretMovementState::Unlanding || MovementState == ETurretMovementState::GoingToDestination)     //TurretOwner->State == ETurretState::Moving || MovementState == ETurretMovementState::Unlanding)
      {
        ForceLanding();
        //TurretOwner->State = ETurretState::Set;
      }
      else if (MovementState == ETurretMovementState::Landing)
      {
        //If the turret was landing, then start to unlanding.
        ForceUnLanding();
      }
      //TurretStopMovingEvent.Broadcast();

    }
    else if (TurretOwner->State == ETurretState::Set)
    {

      float Distance = CalculateDestinationPosition();

      if (Distance > AcceptanceRadius && TurretOwner->TargetWaypoint->IsActive)
      {
        TurretOwner->StartMovementEffect();
        MovementState = ETurretMovementState::Unlanding;
        TurretOwner->State = ETurretState::Moving;
        /*AIController->MoveToLocation(DestinationPosition, AcceptanceRadius, false, true, true, true, 0, false);*/
      }
    }
  }

}



float UMovementTurretComponent::CalculateDestinationPosition()
{
  if (TurretOwner->TargetWaypoint)
  {
    float Distance = TurretOwner->TargetWaypoint->DestinationComponent->GetClosestPointOnCollision(TurretOwner->GetActorLocation(), DestinationPosition);
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    const FNavAgentProperties& AgentProps = AIController->GetNavAgentPropertiesRef();
    if (NavSys != nullptr)
    {
      FNavLocation ProjectedLocation;
      NavSys->GetRandomReachablePointInRadius(DestinationPosition, 50.f, ProjectedLocation);
      DestinationPosition = ProjectedLocation.Location;
    }
    return Distance;
  }

  UE_LOG(LogTemp, Warning, TEXT("MovementTurretComponent.cpp: Turrets need waypoint to advance forward"));

  return 0.f;
}

void UMovementTurretComponent::MoveInCaseItsAlreadyMoving()
{
  if (AIController && AIController->IsFollowingAPath())
  {

    AIController->StopMovement();
    if (TurretOwner->TargetWaypoint->IsActive)
    {
      MoveTurret();
    }
    else
    {
      if (TurretOwner->State == ETurretState::Moving)
      {
        TurretStopMovingEvent.Broadcast();
        TurretOwner->State = ETurretState::Set;
      }
    }
  }
}

void UMovementTurretComponent::MoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& PathFollowingResult)
{
  float Distance = CalculateDestinationPosition();
  if (TurretOwner->State == ETurretState::Moving && Distance < AcceptanceRadius)
  {
    TurretOwner->State = ETurretState::Set;
  }

}


void UMovementTurretComponent::ForceLanding()
{
  if (AIController)
  {
    AIController->StopMovement();
  }
  TurretStopMovingEvent.Broadcast();
  MovementState = ETurretMovementState::Landing;
}

void UMovementTurretComponent::ForceUnLanding()
{
  MovementState = ETurretMovementState::Unlanding;
}

void UMovementTurretComponent::OnBeginOverlapAvoidRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  ATurret* OtherTurret = Cast<ATurret>(OtherActor);

  if (OtherTurret && OtherTurret != TurretOwner && !OverlappedTurrets.Contains(OtherTurret) && OtherComp->IsA(UCapsuleComponent::StaticClass()))
  {
    OverlappedTurrets.Add(OtherTurret);
  }
}


void UMovementTurretComponent::OnEndOverlapAvoidRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

  ATurret* OtherTurret = Cast<ATurret>(OtherActor);

  if (OtherTurret && OverlappedTurrets.Contains(OtherTurret) && OtherComp->IsA(UCapsuleComponent::StaticClass()))
  {
    OverlappedTurrets.Remove(OtherTurret);
  }

}