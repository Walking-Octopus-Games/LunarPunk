// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ForkDoor.generated.h"

class ATurretWaypoint;

UCLASS()
class LUNARPUNK_API AForkDoor : public AActor
{
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  AForkDoor();

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

public:

  // TARGET WAYPOINT TO ACTIVATE
  //UPROPERTY(EditAnywhere)
  //  ATurretWaypoint* TargetWaypointToActivate;

  UPROPERTY(EditAnywhere)
    float MinDistanceToTarget = 10.f;

  UPROPERTY(EditAnywhere)
    float Velocity = 50.f;

  void FollowTarget(float DeltaTime);

  // Called every frame
  virtual void Tick(float DeltaTime) override;

  UFUNCTION()
    void Close();

  UFUNCTION()
    void Open();


  UPROPERTY(EditAnywhere)
    USceneComponent* OpenedPositionComponent;

  UPROPERTY(EditAnywhere)
    USceneComponent* ClosedPositionComponent;

  UPROPERTY()
    FVector TargetPosition;

  UPROPERTY(EditAnywhere)
    bool bIsMoving;

  UPROPERTY(EditAnywhere)
    USceneComponent* MyRootComponent;
};
