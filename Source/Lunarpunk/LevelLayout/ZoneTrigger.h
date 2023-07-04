// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZoneTrigger.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTriggerActivated);

class  UForkComponent;

UCLASS()
class LUNARPUNK_API AZoneTrigger : public AActor
{
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  AZoneTrigger();

  //Collision to handles the turret placement.
  UPROPERTY(VisibleAnywhere)
    class UBoxComponent* ZoneTrigger;

  //Index to asociate this Turret Place to a Conquest Zone
  UPROPERTY(EditAnywhere)
    int32 IndexOwnerConquestZone = 1;

  UPROPERTY(EditAnywhere, Category = "Debug")
    bool bActivateAtBegin = false;

  UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* FogEffect;

  UPROPERTY(EditAnywhere)
    UForkComponent* ForkComponent;

  UPROPERTY(EditAnywhere)
    UBoxComponent* BlockCollisionComponent;

  UPROPERTY(EditAnywhere)
    USceneComponent* DefaultRootComponent;
protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

public:
  // Called every frame
  virtual void Tick(float DeltaTime) override;

  void ActivateZoneTrigger();

  void DeactivateZoneTrigger();


  UFUNCTION()
    virtual void OnBeginOverlap(
      UPrimitiveComponent* OverlappedComponent,
      AActor* OtherActor,
      UPrimitiveComponent* OtherComp,
      int32 OtherBodyIndex,
      bool bFromSweep,
      const FHitResult& SweepResult
    );

  //Delegate to notice when the trigger gets overlapped.
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTriggerActivated TriggerActivated;



private:

  class APlayableCharacter* PlayerReference;
};
