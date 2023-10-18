// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "Portal.generated.h"

class UStaticMeshComponent;
class UWidgetComponent;
class UNiagaraComponent;
class UFMODEvent;
class UFMODAudioComponent;
class APortalWaypoint;
class UPawnMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPortalDestroy, bool, IsLastHit, int, IndexPortal);
UCLASS()
class LUNARPUNK_API APortal : public APawn
{
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  APortal();

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMeshComponent* MeshRootComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool isMainPortal = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool IsMovablePortal = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Index = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Hits = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int TotalHits = 3;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bAddLifeToPlayerWhenDie = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bAddLifeToPlayerWhenDie", EditConditionHides))
    float LifeToAdd = 30.0f;

  UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable)
    FPortalDestroy PortalDestroy;

  UPROPERTY(EditAnywhere, Category = "Portal|Sound")
    UFMODEvent* PortalAudioEvent;
  UPROPERTY(EditAnywhere, Category = "Portal|Sound")
    UFMODEvent* DestroyPortalAudioEvent;

  UPROPERTY()
    UFMODAudioComponent* PortalAudioEventInstance;
  UPROPERTY()
    UFMODAudioComponent* DestroyPortalAudioEventInstance;

  // Component of the niagara shoot effect
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UNiagaraComponent* DestroyEffectComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UBoxComponent* BoxComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AAIController* AIController;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<APortalWaypoint*> Waypoints;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UFloatingPawnMovement* FloatingMovementComponent;

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;


public:
  // Called every frame
  virtual void Tick(float DeltaTime) override;

  void StopAudio();
  UFUNCTION()
    void StartDestroyAudio(bool f, int a);

  UFUNCTION(BlueprintCallable)
    bool DestroyPortal();

  void MoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& PathFollowingResult);

  virtual void BeginDestroy() override;
};
