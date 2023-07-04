// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WalkToDestinationComponent.generated.h"

class ActorComponent;
class UCharacterMovementComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUNARPUNK_API UWalkToDestinationComponent : public UActorComponent
{
  GENERATED_BODY()
private:
  /*
  * Movement without physics just position tranform
  */
  void MovementByPawn(float DeltaTime, FVector& VectorBetweenActorAndTarget);

  /*
  * Movement using character movement component provided by Unreal
  */
  void MovementByCharacter(float DeltaTime, FVector& VectorBetweenActorAndTarget);

  /*Searchs and returns the closest AActor in the array*/
  AActor* SearchClosestTurret();

  /*Check if the actors in the array*/
  void CheckValidTurretsActor();

  void LookAtTarget() const;

  void FollowTarget(float DeltaTime);



public:
  // Sets default values for this component's properties
  UWalkToDestinationComponent();

protected:
  // Called when the game starts
  virtual void BeginPlay() override;


public:
  // Called every frame
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  /* If true the component will search and go to the closest target position*/
  bool bIsActive = true;

  /*Target that the actor will follow first. When the value is empty it will follow the closest one to the actor*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Walk to Destination")
    AActor* ATargetToFollowFirst = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Walk to Destination")
    TArray<AActor*> ActorTurrets;

  UPROPERTY(EditAnywhere, Category = "Walk to Destination", meta = (UIMin = "0.0", UIMax = "1.0"))
    float Velocity = 0.5f;

  UPROPERTY(EditAnywhere, Category = "Walk to Destination")
    float ReachedDistance = 10;

  /*If true it will follow the closest AActor when the first target is destroyed*/
  UPROPERTY(EditAnywhere, Category = "Walk to Destination")
    bool bFollowClosestTargets = true;

  UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Walk to Destination")
    bool bCanRun = true;

  /*Component that stores the character movement component*/
  UCharacterMovementComponent* CharMoveComponent = NULL;


};
