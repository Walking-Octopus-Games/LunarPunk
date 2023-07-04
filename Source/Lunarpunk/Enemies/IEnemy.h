// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IEnemy.generated.h"

// This does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemy : public UInterface
{
  GENERATED_BODY()
};

/**
 *
 */
class LUNARPUNK_API IEnemy
{
  GENERATED_BODY()

    // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

  virtual class UHealthComponent* GetHealthComponent() = 0;

  virtual class UDamageComponent* GetDamageComponent() = 0;

  virtual class UDropRewardsComponent* GetRewardsComponent() = 0;

  virtual class APool* GetMyPool() = 0;

  virtual class UWalkToDestinationComponent* GetWalkToDestinationComponent() = 0;

  virtual void SetMaxHealth(float PercentageVariationMaxHealth) = 0;

  virtual void SetMaxDamage(float PercentageVariationMaxDamage) = 0;

  virtual void SetMovementSpeed(float PercentageVariationSpeed) = 0;

  virtual void SetPlatesVariation(int32 PlatesVariation) = 0;

  virtual void SetUpgradesVariation(float UpgradesVariation) = 0;

  virtual void DrawDamageWidget(float DamageReceived) = 0;

};
