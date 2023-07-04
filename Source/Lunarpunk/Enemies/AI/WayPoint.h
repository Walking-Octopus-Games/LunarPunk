// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "WayPoint.generated.h"

/**
 *
 */
UCLASS()
class LUNARPUNK_API AWayPoint : public ATargetPoint
{
  GENERATED_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ENEMY | AI")
    int32 Position;
};
