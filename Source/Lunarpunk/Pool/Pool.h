// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pool/IsPoolable.h"
#include "Pool.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPoolCreated);

UCLASS()
class LUNARPUNK_API APool : public AActor
{
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  APool();

  //Classes to store in the pool and the quantity of them
  UPROPERTY(EditAnyWhere, Category = "Pool")
    TMap<TSubclassOf<AActor>, int32> ClassesToSpawn;

  //Contains the world location where the actors will be stored.
  UPROPERTY(VisibleAnyWhere, Category = "Pool")
    FVector PoolLocation;

  //Multimap that stores all the spawns required, classified by class.
  TMultiMap<TSubclassOf<AActor>, AActor*> Spawns;

  //Multimap that stores the index of each actor in the pool. Note: Not neccesary at the moment
  /*TMultiMap<TSubclassOf<AActor>, int32> IndexesSpawnedActors;*/
  FPoolCreated PoolCreated;
  UFUNCTION()
    void InitializePool();

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

public:
  // Called every frame
  virtual void Tick(float DeltaTime) override;

  //Function to obtain the first available actor in the pool of the specified class.
  AActor* GetNextActor(TSubclassOf<AActor> classToSpawn);

  //Function to obtain all the actors of the specified class.
  TArray<AActor*> GetAllActors(TSubclassOf<AActor> classToSpawn);

  int32 NumActiveActorsOfClass(TSubclassOf<AActor> Class);

  bool CheckMaxActorsInScreenReached(TSubclassOf<AActor> Class, int32 OverrideMax = -1);

};
