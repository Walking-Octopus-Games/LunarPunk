// Fill out your copyright notice in the Description page of Project Settings.

#include "WalkToDestinationComponent.h"
#include "Components/ActorComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Math/Vector.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HealthSystem/HealthComponent.h"
#include <Kismet/GameplayStatics.h>

#include "Player/PlayableCharacter.h"
#include "Turrets/AoETurret.h"
#include "Turrets/Turret.h"

// Sets default values for this component's properties

UWalkToDestinationComponent::UWalkToDestinationComponent()
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;

  // ...
}


// Called when the game starts
void UWalkToDestinationComponent::BeginPlay()
{
  Super::BeginPlay();

  // ...
  if (IsValid(ATargetToFollowFirst))
  {
    UHealthComponent* HealthComponent = Cast<UHealthComponent>(ATargetToFollowFirst->GetComponentByClass(UHealthComponent::StaticClass()));

    if (IsValid(HealthComponent))
    {
      if (HealthComponent->GetHealth() <= 0)
      {
        ATargetToFollowFirst = SearchClosestTurret();
      }
    }
  }
  else
  {
    ATargetToFollowFirst = SearchClosestTurret();
  }

  CharMoveComponent = Cast<UCharacterMovementComponent>(GetOwner()->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
}

void UWalkToDestinationComponent::MovementByPawn(float DeltaTime, FVector& VectorBetweenActorAndTarget)
{
  if (bCanRun)
  {
    GetOwner()->AddActorWorldOffset(Velocity * DeltaTime * VectorBetweenActorAndTarget);
  }

  LookAtTarget();

}

void UWalkToDestinationComponent::MovementByCharacter(float DeltaTime, FVector& VectorBetweenActorAndTarget)
{
  CharMoveComponent->AddInputVector(VectorBetweenActorAndTarget * Velocity);
  //CharMoveComponent->UpdateBasedMovement(DeltaTime);

  LookAtTarget();
}


AActor* UWalkToDestinationComponent::SearchClosestTurret()
{
  CheckValidTurretsActor();
  if (ActorTurrets.Num() > 0)
  {
    float Distance = FVector::Distance(ActorTurrets[0]->GetActorLocation(), GetOwner()->GetActorLocation());
    AActor* ActorToFollow = ActorTurrets[0];

    for (int i = 1; i < ActorTurrets.Num(); i++)
    {
      const float ActualDistance = FVector::Distance(ActorTurrets[i]->GetActorLocation(), GetOwner()->GetActorLocation());
      
      if (Distance > ActualDistance)
      {
          Distance = ActualDistance;
          ActorToFollow = ActorTurrets[i];
      }

    }
    return ActorToFollow;
  }

  return UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);


}

void UWalkToDestinationComponent::CheckValidTurretsActor()
{
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATurret::StaticClass(), ActorTurrets);
  TArray<AActor*> TurretsAlive = ActorTurrets;

  if (ActorTurrets.IsValidIndex(0))
  {
    const int NumTurrets = ActorTurrets.Num();
    for (int i = 0; i < NumTurrets; i++)
    {
      if (IsValid(ActorTurrets[i]))
      {
        const ATurret* Turret = Cast<ATurret>(ActorTurrets[i]);

        if (IsValid(Turret->HealthComponent))
        {
		  const float Health = Turret->HealthComponent->GetHealth();

          if (Health <= 0.0f)
          {
            TurretsAlive.Remove(ActorTurrets[i]);
          }
        }
      }
    }
    ActorTurrets = TurretsAlive;
    ActorTurrets.Shrink();
  }
}

void UWalkToDestinationComponent::LookAtTarget() const
{
  FRotator NewRotation = (GetOwner()->GetActorLocation() - ATargetToFollowFirst->GetActorLocation()).Rotation();
  NewRotation.Pitch = 0; //Prevent the actor from rotating in exe Z
  GetOwner()->SetActorRotation(NewRotation);
}

// Called every frame
void UWalkToDestinationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  if (bIsActive)
  {
    ATargetToFollowFirst = SearchClosestTurret();
    FollowTarget(DeltaTime);
  }
}

void UWalkToDestinationComponent::FollowTarget(float DeltaTime)
{
  const FVector CurrentLocation = GetOwner()->GetActorLocation();

  if (IsValid(ATargetToFollowFirst))
  {
    FVector VectorBetweenActorAndTarget = ATargetToFollowFirst->GetActorLocation() - CurrentLocation;
    const float DistanceBetweenActorAndTarget = VectorBetweenActorAndTarget.Size();
    VectorBetweenActorAndTarget.Z = 0; //Prevent the actor go up and down in exe Z
    VectorBetweenActorAndTarget.Normalize();

    if (DistanceBetweenActorAndTarget > ReachedDistance)
    {
      if (IsValid(CharMoveComponent))
      {
        MovementByCharacter(DeltaTime, VectorBetweenActorAndTarget);
      }
      else
      {
        MovementByPawn(DeltaTime, VectorBetweenActorAndTarget);
      }

    }
  }
}

