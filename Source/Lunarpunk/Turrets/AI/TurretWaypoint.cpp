// Fill out your copyright notice in the Description page of Project Settings.


#include "Turrets/AI/TurretWaypoint.h"

#include "Turrets/Turret.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "LevelLayout/ConquestZone.h"
#include "MovementTurretComponent.h"
#include "Components/SphereComponent.h"

ATurretWaypoint::ATurretWaypoint()
{
  BoxCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollisionComponent"));
  BoxCollisionComponent->SetupAttachment(RootComponent);
}

void ATurretWaypoint::BeginPlay()
{
  Super::BeginPlay();

  BoxCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ATurretWaypoint::OnOverlapBegin);
  DestinationComponent = Cast<UBoxComponent>(GetDefaultSubobjectByName("DestinationWayPoint"));
  //ConquestZoneAssigned->ZoneCompleted.AddUObject(this, &ATurretWaypoint::DeactivateTurretWaypoint);

  if (IsValid(ConquestZoneAssigned))
  {
    ConquestZoneAssigned->ZoneStarted.AddUObject(this, &ATurretWaypoint::ActivateTurretWaypoint);
  }
}

void ATurretWaypoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

  if (OtherActor->IsA(ATurret::StaticClass()) && OtherComp->IsA(UCapsuleComponent::StaticClass()))
  {
    ATurret* Turret = Cast<ATurret>(OtherActor);

    if (IsValid(Turret) && IsValid(Turret->TargetWaypoint) && Turret->TargetWaypoint == this)
    {
      if (IsValid(NextWaypoint))
      {
        Turret->SetTargetWaypoint(NextWaypoint);

        if (Turret->State != ETurretState::PickedUp && !NextWaypoint->IsActive)
        {
          Turret->TurretMovementComponent->ForceLanding();
        }

      }
      else
      {
        if (!IsValid(NextWaypoint))
        {
          if (Turret->State != ETurretState::PickedUp)
          {
            Turret->TurretMovementComponent->ForceLanding();
          }
        }
      }

    }
  }
}

void ATurretWaypoint::ActivateTurretWaypoint(int32 ZoneIndex)
{
  IsActive = true;
}

void ATurretWaypoint::DeactivateTurretWaypoint(int32 ZoneIndex)
{
  IsActive = false;
}
