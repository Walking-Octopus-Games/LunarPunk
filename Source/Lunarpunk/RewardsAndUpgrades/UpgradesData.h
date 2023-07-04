// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Upgrade.h"
#include "Turrets/TurretsData.h"
#include "UpgradesData.generated.h"


USTRUCT(BlueprintType)
struct LUNARPUNK_API FUpgradesIncreaseInfo
{
	GENERATED_USTRUCT_BODY()

public:

	FUpgradesIncreaseInfo() {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETurretType Type;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int> MaxRanges;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<float> IncreasesOfUpgrade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<float> IncreasesOfHealing;
};

USTRUCT(BlueprintType)
struct LUNARPUNK_API FUpgradesDataStruct
{
	GENERATED_USTRUCT_BODY()

public:

	FUpgradesDataStruct()
	{
		Name = "";
		Description = "";
		Image = nullptr;
	};


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EUpgradeType Type;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere)
		FString Name;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere)
		FString Description;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere)
		UTexture2D* Image;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere)
		FLinearColor Tint;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere)
		TArray<FUpgradesIncreaseInfo> IncreasesInfo;
	
	UPROPERTY(BlueprintReadOnly, EditAnyWhere)
		bool bIsAvailableAtBegin = false;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere)
		int32 NumUpgradesAvailableAtBegin = 1;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere)
		int32 NumUpgrades = 0;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere)
		int32 MaxUpgrades = 5;
};

UCLASS(BlueprintType)
class LUNARPUNK_API UUpgradesData : public UDataAsset
{
	GENERATED_BODY()

public: 

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TMap<EUpgradeType, FUpgradesDataStruct> Upgrades;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TArray<EUpgradeType> UpgradesOrder;
	
};
