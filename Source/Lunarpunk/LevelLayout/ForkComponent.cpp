// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelLayout/ForkComponent.h"

#include "ForkDoor.h"
#include "ZoneTrigger.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayableCharacter.h"
#include "Turrets/Turret.h"
#include "Turrets/AI/MovementTurretComponent.h"
#include "Turrets/AI/TurretWaypoint.h"

// Sets default values for this component's properties
UForkComponent::UForkComponent()
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;

  // ...
}


// Called when the game starts
void UForkComponent::BeginPlay()
{
  Super::BeginPlay();

  // ...
  UGameplayStatics::GetAllActorsWithTag(GetWorld(), "ChoosePath", ChoosePathDecals);
  for (AActor* Actor : ChoosePathDecals)
  {
    Actor->SetActorHiddenInGame(true);
  }
}


// Called every frame
void UForkComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // ...
}


void UForkComponent::MoveAllTurrets(bool AssignNewWaypoint)
{
  TArray<AActor*> TurretActors;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATurret::StaticClass(), TurretActors);
  for (AActor* Turret : TurretActors)
  {
    ATurret* T = Cast<ATurret>(Turret);
    if (IsValid(T))
    {
      if (AssignNewWaypoint)
      {
        T->TargetWaypoint = TargetWaypointToActivate;
      }
      T->TurretMovementComponent->ManageTurretMovement(false);
    }
  }
}

void UForkComponent::AssignTurretNewWaypoints()
{
  TargetWaypointToActivate->ActivateTurretWaypoint(0);
  MoveAllTurrets(true);
}

void UForkComponent::ActivateForkComponent()
{
  if (IsValid(OppositeZoneTrigger))
  {
    bWayChoose = true;

    CloseDoors();

    AssignTurretNewWaypoints();
  }
}

void UForkComponent::OpenDoors()
{
  //for (AForkDoor* WayForkDoor : WayForkDoors)
  //{
  //  WayForkDoor->Open();
  //}

  if (IsValid(OppositeZoneTrigger))
  {
    OppositeZoneTrigger->BlockCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    for (AActor* Actor : ChoosePathDecals)
    {
      Actor->SetActorHiddenInGame(false);
    }
    MoveAllTurrets(false);
  }
}

void UForkComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  if (OtherActor->IsA(APlayableCharacter::StaticClass()) && OtherComp->IsA(UCapsuleComponent::StaticClass()))
  {
    ActivateForkComponent();
  }
}

void UForkComponent::CloseDoors()
{
  if (IsValid(OppositeZoneTrigger))
  {

    OppositeZoneTrigger->BlockCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    OppositeZoneTrigger->BlockCollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
    if (IsValid(OppositeZoneTrigger->FogEffect))
    {
      OppositeZoneTrigger->FogEffect->SetVisibility(true);
    }

    //for (AForkDoor* WayForkDoor : OppositeZoneTrigger->ForkComponent->WayForkDoors)
    //{
    //  WayForkDoor->Close();
    //}
  }
}
