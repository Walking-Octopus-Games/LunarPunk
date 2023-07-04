// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pool/IsPoolable.h"
#include "Reward.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class ALunarPunkGameMode;
class UFMODAudioComponent;
class UFMODEvent;

UCLASS()
class LUNARPUNK_API AReward : public AActor, public IIsPoolable
{
  GENERATED_BODY()

public:
  //// Sets default values for this actor's properties
  AReward();

  UPROPERTY()
    UStaticMeshComponent* StaticMesh;

  //Time life until the rewards vanishes
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Lifetime = 5.0f;

  //Time since the Reward exists
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    float CurrentTime = 0.0f;

  //Collision to do the overlaps events-
  UPROPERTY(EditAnywhere)
    USphereComponent* Collision;

  //Reference to player character
  UPROPERTY(EditAnywhere)
    class APlayableCharacter* Player;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UUserWidget> GoToHUDWidget;

  //Reference to the pool of the game
  UPROPERTY(VisibleAnywhere)
    class APool* MyPool;


protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

  UPROPERTY()
    ALunarPunkGameMode* GameMode;


public:
  // Called every frame
  virtual void Tick(float DeltaTime) override;


  //Returns the class of the reward
  virtual UClass* GetTypeOfReward();

  UFUNCTION()
    virtual void OnBeginOverlap(
      UPrimitiveComponent* OverlappedComponent,
      AActor* OtherActor,
      UPrimitiveComponent* OtherComp,
      int32 OtherBodyIndex,
      bool bFromSweep,
      const FHitResult& SweepResult
    );


  //Called when the player is drawing in the reward.
  virtual void BeingDrawIn(const FTransform& TransformActorDrawingIn, float Force);

  UFUNCTION(BlueprintImplementableEvent)
    void GoToHUD(int32 QuantityToAdd);

  UFUNCTION(BlueprintCallable)
    virtual void AddToHUD(int32 QuantityToAdd);

  //Functions to handle the in-out of the pool
  virtual void ReturnToPool() override;
  virtual void Deactivate() override;
  virtual void Reactivate() override;

  UPROPERTY(EditAnywhere, Category = "Player|Sound")
    UFMODEvent* ReceiveByPlayerAudioEvent;

  UPROPERTY(EditAnywhere, Category = "Player|Sound")
    UFMODEvent* ReceiveByHUDAudioEvent;

  UPROPERTY()
    UFMODAudioComponent* ReceiveByPlayerAudioInstance;

  UPROPERTY()
    UFMODAudioComponent* ReceiveByHUDAudioInstance;
};
