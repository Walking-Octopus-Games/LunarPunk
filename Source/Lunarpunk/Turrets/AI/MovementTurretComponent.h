// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Components/ActorComponent.h"
#include "MovementTurretComponent.generated.h"

class USphereComponent;

UENUM()
enum class ETurretMovementState : uint8
{
  Unlanding, //Turret is unlanding before start moving
  Landing, //Turret is landing after stop moving.
  GoingToDestination, //Turret is moving in the destionation direction
  Landed //Turret is stopped.
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurretMoving);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurretStop);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUNARPUNK_API UMovementTurretComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UMovementTurretComponent();

  UFUNCTION(BlueprintCallable)
    void ManageTurretMovement(bool IsMoving);

protected:
  UFUNCTION(BlueprintCallable)
    void SearchClosestTurretWaypoint();

  // Called when the game starts
  virtual void BeginPlay() override;
  //UFUNCTION(BlueprintCallable)
    void MoveTurret();

public:
  // Called every frame
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  float CalculateDestinationPosition();

  UFUNCTION(BlueprintCallable)
    void MoveInCaseItsAlreadyMoving();

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AcceptanceRadius = 10.f;


  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bAvoidOtherTurrets = true;

  UPROPERTY(EditAnywhere, meta = (EditCondition = "bAvoidOtherTurrets", EditConditionHides))
    float AvoidanceSpeed = 150.0f;

  UPROPERTY(EditAnywhere, meta = (EditCondition = "bAvoidOtherTurrets", EditConditionHides))
    float AvoidanceRadius = 150.0f;

  UPROPERTY(EditAnywhere, meta = (EditCondition = "bAvoidOtherTurrets", EditConditionHides))
    bool bAvoidWalls = true;

  UPROPERTY(EditAnywhere, meta = (EditCondition = "bAvoidOtherTurrets && bAvoidWalls", EditConditionHides))
    float AvoidanceWallsDistance = 40.0f;

  UPROPERTY(EditAnywhere, meta = (EditCondition = "bAvoidOtherTurrets", EditConditionHides))
    FName CollisionName = FName("AvoidCollision");

  UPROPERTY(EditAnywhere, meta = (EditCondition = "bAvoidOtherTurrets", EditConditionHides))
    bool bShowDebugLines = false;

  UPROPERTY(VisibleAnywhere, meta = (EditCondition = "bAvoidOtherTurrets", EditConditionHides))
    USphereComponent* AvoidCollisionRange;

  UPROPERTY()
    class ATurret* TurretOwner;

  UPROPERTY(VisibleAnywhere)
    ETurretMovementState MovementState = ETurretMovementState::Landed;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    AAIController* AIController;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Turret|Speed")
    float MaxTurretSpeed = 100.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Turret|Unlanding")
    float UnlandingHeight = 40.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Turret|Unlanding")
    float UnlandingSpeed = 90.0f;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FVector DestinationPosition;

  void MoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& PathFollowingResult);

  //Force the landing state of the movement component;
  UFUNCTION(BlueprintCallable)
    void ForceLanding();

  //Force the landing state of the movement component;
  UFUNCTION(BlueprintCallable)
    void ForceUnLanding();

  UFUNCTION()
    void OnBeginOverlapAvoidRange(UPrimitiveComponent* OverlappedComponent,
      AActor* OtherActor,
      UPrimitiveComponent* OtherComp,
      int32 OtherBodyIndex,
      bool bFromSweep,
      const FHitResult& SweepResult);

  UFUNCTION()
    void OnEndOverlapAvoidRange(UPrimitiveComponent* OverlappedComponent,
      AActor* OtherActor,
      UPrimitiveComponent* OtherComp,
      int32 OtherBodyIndex
    );

  FTurretMoving TurretMovingEvent;
  FTurretStop TurretStopMovingEvent;
private:

  float InitialHeight = 0.0f;

  TArray<ATurret*> OverlappedTurrets;
};
