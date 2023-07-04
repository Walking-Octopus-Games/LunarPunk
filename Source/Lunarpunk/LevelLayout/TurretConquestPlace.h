// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurretConquestPlace.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurretPlaced);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurretRetired);

UCLASS()
class LUNARPUNK_API ATurretConquestPlace : public AActor
{
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  ATurretConquestPlace();

  //Index to asociate this Turret Place to a Conquest Zone
  UPROPERTY(EditAnywhere)
    int32 IndexOwnerConquestZone = 1;

  //Index to asociate this Turret Place to a Conquest Zone
  UPROPERTY(EditAnywhere)
    FVector Offset = FVector(0.0, 0.0, -30.0);


  UPROPERTY(EditAnywhere)
    float AdjustToCenterSpeed = 3.0f;

  //Collision to handles the turret placement.
  UPROPERTY(BlueprintReadWrite)
    class UBoxComponent* TurretPlaceCollision;

  //Collision to handles the turret placement.
  UStaticMeshComponent* PlaceMesh;

  UPROPERTY(BlueprintReadWrite)
    bool bTurretIsPlaced = false;

  //Stores the placed turret reference
  UPROPERTY()
    class ATurret* TurretPlacedReference = nullptr;

  //Delegates to handle the placement and retirment of turrets.
  UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
    FTurretPlaced TurretPlaced;

  UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
    FTurretRetired TurretRetired;

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

public:
  // Called every frame
  virtual void Tick(float DeltaTime) override;


  UFUNCTION()
    virtual void OnBeginOverlap(
      UPrimitiveComponent* OverlappedComponent,
      AActor* OtherActor,
      UPrimitiveComponent* OtherComp,
      int32 OtherBodyIndex,
      bool bFromSweep,
      const FHitResult& SweepResult
    );


  UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComponent
      , AActor* OtherActor
      , UPrimitiveComponent* OtherComp
      , int32 OtherBodyIndex
    );


  //Callbacks to handle the alive or death state of the placed turret.
  UFUNCTION()
    void PlacedTurretDeath();

  UFUNCTION()
    void PlacedTurretReactivated();


  //Functions to show the zone effect
  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void ShowZoneActivatedEffect();

  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void RemoveZoneActivatedEffect();

  //Functions to show the zone effect
  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void ShowTurretGhost();

  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void RemoveTurretGhost();

  UFUNCTION(BlueprintCallable)
    void DisableTurretPlace();

  UFUNCTION(BlueprintCallable)
    void EnableTurretPlace();


  //Deactivates the conquest place for ever.
  void CloseConquestPlace();

private:

  bool bGoingToCenter = false;

};
