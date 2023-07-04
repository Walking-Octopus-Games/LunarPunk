// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TurretsData.generated.h"

UENUM(BlueprintType)
enum class ETurretType : uint8
{
	None									UMETA(Hidden)
	, MachineGunTurret		UMETA(DisplayName = "Machine Gun")
	, AoETurret						UMETA(DisplayName = "Area")
	, HealingTurret				UMETA(DisplayName = "Healing")
};


USTRUCT(BlueprintType)
struct LUNARPUNK_API FTurretDataStruct
{
	GENERATED_USTRUCT_BODY()

public:

	FTurretDataStruct() {};

	UPROPERTY(BlueprintReadOnly, EditAnyWhere)
		TSubclassOf<AActor> TurretBlueprint;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere)
		FString Name;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere)
		FString Description;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere)
		UTexture2D* Image;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere, meta = (ClampMin = "0", ClampMax = "6", UIMin = "0", UIMax = "6"))
		int32 Stat1;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere, meta = (ClampMin = "0", ClampMax = "6", UIMin = "0", UIMax = "6"))
		int32 Stat2;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere, meta = (ClampMin = "0", ClampMax = "6", UIMin = "0", UIMax = "6"))
		int32 Stat3;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere, meta = (ClampMin = "0", ClampMax = "6", UIMin = "0", UIMax = "6"))
		int32 Stat4;

};

/**
 *
 */
UCLASS(BlueprintType)
class LUNARPUNK_API UTurretsData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TMap<ETurretType, FTurretDataStruct> Turrets;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TArray<ETurretType> TurretsOrder;

};