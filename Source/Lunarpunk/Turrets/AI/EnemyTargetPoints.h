// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "EnemyTargetPoints.generated.h"

class ATurret;
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUNARPUNK_API UEnemyTargetPoints : public USceneComponent
{
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UEnemyTargetPoints();

protected:
  // Called when the game starts
  virtual void BeginPlay() override;

public:
  // Called every frame
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  UFUNCTION()
    bool IsOcuppied();

  UFUNCTION()
    void SetbIsOcuppied(bool bCond);

  UPROPERTY()
    ATurret* OwnerTurret;

  UPROPERTY(VisibleAnywhere)
    bool bIsOcuppied = false;
private:

};
