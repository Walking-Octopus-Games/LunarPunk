// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelLayout/ForkDoor.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AForkDoor::AForkDoor()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

  MyRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("My Root Component"));
  SetRootComponent(MyRootComponent);

  OpenedPositionComponent = CreateDefaultSubobject<USceneComponent>(TEXT("OpenedPositionComponent"));
  OpenedPositionComponent->SetMobility(EComponentMobility::Static);

  ClosedPositionComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ClosedPositionComponent"));
  ClosedPositionComponent->SetMobility(EComponentMobility::Static);
  SetActorTickEnabled(false);
}

// Called when the game starts or when spawned
void AForkDoor::BeginPlay()
{
  Super::BeginPlay();

  //SetActorTickEnabled(false);

}

// Called every frame
void AForkDoor::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
  if (bIsMoving)
  {
    FollowTarget(DeltaTime);
  }

}

void AForkDoor::Close()
{
  bIsMoving = true;
  SetActorTickEnabled(true);
  TargetPosition = ClosedPositionComponent->GetComponentLocation();
}

void AForkDoor::Open()
{
  bIsMoving = true;
  SetActorTickEnabled(true);
  TargetPosition = OpenedPositionComponent->GetComponentLocation();
}


void AForkDoor::FollowTarget(float DeltaTime)
{
  const FVector CurrentLocation = GetActorLocation();
  FVector VectorBetweenActorAndTarget = TargetPosition - CurrentLocation;
  const float DistanceBetweenActorAndTarget = VectorBetweenActorAndTarget.Size();
  VectorBetweenActorAndTarget.Normalize();

  if (DistanceBetweenActorAndTarget > MinDistanceToTarget)
  {
    AddActorWorldOffset(Velocity * DeltaTime * VectorBetweenActorAndTarget);
  }
  else
  {
    bIsMoving = false;
    SetActorTickEnabled(false);
  }
}