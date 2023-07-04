// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IsPoolable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIsPoolable : public UInterface
{
  GENERATED_BODY()
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReactivate, AActor*, Actor);
/**
 *
 */
class LUNARPUNK_API IIsPoolable
{
  GENERATED_BODY()

    // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

  FReactivate FReactivate;

  //Index in the pool
  int32 Index = 0;
  //If true, the actor is outside the pool. If false, is available in the pool.
  bool bIsActive = false;

  virtual void ReturnToPool() = 0;
  virtual void Deactivate() = 0;
  virtual void Reactivate() = 0;
};
