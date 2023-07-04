// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "TurretWaypoint.generated.h"


/**
 *
 */
UCLASS()
class LUNARPUNK_API ATurretWaypoint : public ATargetPoint
{
  GENERATED_BODY()

    ATurretWaypoint();

public:

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class AConquestZone* ConquestZoneAssigned;

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool IsActive = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ATurretWaypoint* NextWaypoint;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ATurretWaypoint* PrevWaypoint;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UBoxComponent* BoxCollisionComponent;
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
     UBoxComponent* DestinationComponent;

  UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
      AActor* OtherActor,
      UPrimitiveComponent* OtherComp,
      int32 OtherBodyIndex,
      bool bFromSweep,
      const FHitResult& SweepResult);

  void ActivateTurretWaypoint(int32 ZoneIndex);

  void DeactivateTurretWaypoint(int32 ZoneIndex);

};
