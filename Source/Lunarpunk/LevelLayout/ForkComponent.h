// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DecalActor.h"
#include "Components/ActorComponent.h"
#include "ForkComponent.generated.h"


class AForkDoor;
class ATurretWaypoint;
class AZoneTrigger;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUNARPUNK_API UForkComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UForkComponent();

protected:
  // Called when the game starts
  virtual void BeginPlay() override;

public:
  // Called every frame
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  void MoveAllTurrets(bool AssignNewWaypoint);

  void AssignTurretNewWaypoints();

  void ActivateForkComponent();


  UFUNCTION()
    void OpenDoors();

  UFUNCTION()
    virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
      int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


  UFUNCTION()
    void CloseDoors();



  UPROPERTY(VisibleAnywhere)
    bool bWayChoose = false;

  UPROPERTY(EditAnywhere)
    AZoneTrigger* OppositeZoneTrigger;

  UPROPERTY(EditAnywhere)
    ATurretWaypoint* TargetWaypointToActivate;

  UPROPERTY(EditAnywhere)
    TArray<AForkDoor*> WayForkDoors;

  /*UPROPERTY(EditAnywhere)
    TArray<ADecalActor*> ChoosePathDecals;*/
  UPROPERTY(EditAnywhere)
    TArray<AActor*> ChoosePathDecals;





};
