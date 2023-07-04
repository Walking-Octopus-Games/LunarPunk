// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/LunarPunkPlayerController.h"
#include "UObject/NoExportTypes.h"
#include "AbilitiesManger.generated.h"

/**
 *
 */

UENUM(BlueprintType)
enum class EAbilityType : uint8
{
  None			UMETA(Hidden),
  Granade		UMETA(DisplayName = "Granade"),
  Shield		UMETA(DisplayName = "Shield"),
  DestroyPortal UMETA(DisplayName = "Destroy Portal"),
  Shot          UMETA(DisplayName = "Shot"),
  Spark         UMETA(DisplayName = "Spark"),
  TrapActivator UMETA(DisplayName = "Trap Activator")
};

class UAbilitiesData;
class ALunarPunkGameMode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectAbility, EAbilityType, Type);

UCLASS()
class LUNARPUNK_API UAbilitiesManger : public UObject
{
  GENERATED_BODY()

public:
  UAbilitiesManger();

  UFUNCTION()
    void Initialize(ALunarPunkPlayerController* PlayerController);

  UFUNCTION(BlueprintCallable)
    EAbilityType GetCurrentAbility();

  UFUNCTION()
    void SelectGrenade();
  
  UFUNCTION()
    void SelectShield();

  UFUNCTION()
    void SelectSparkle();
  
  UFUNCTION()
    void SelectTraps();


  UPROPERTY(EditAnywhere)
      EAbilityType CurrentAbility;
  
  UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable)
    FSelectAbility SelectAbility;

private:
  UPROPERTY()
    ALunarPunkGameMode* GameMode;
};
