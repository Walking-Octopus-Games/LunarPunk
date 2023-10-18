// Fill out your copyright notice in the Description page of Project Settings.


#include "Turrets/AI/EnemyTargetPoints.h"

#include "Turrets/Turret.h"

// Sets default values for this component's properties
UEnemyTargetPoints::UEnemyTargetPoints()
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;

  // ...
}


// Called when the game starts
void UEnemyTargetPoints::BeginPlay()
{
  Super::BeginPlay();

  // ...
  OwnerTurret = Cast<ATurret>(GetOwner());
}


// Called every frame
void UEnemyTargetPoints::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // ...
}

bool UEnemyTargetPoints::IsOcuppied()
{
  if (OwnerTurret->State == ETurretState::Deactivated || OwnerTurret->State == ETurretState::PickedUp || OwnerTurret->State == ETurretState::Repairing)
  {
    return true;
  }

  return bIsOcuppied;
}

void UEnemyTargetPoints::SetbIsOcuppied(bool bCond)
{
  bIsOcuppied = bCond;
}

