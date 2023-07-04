// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilitiesManger.h"
#include "AbilitiesData.generated.h"


USTRUCT(BlueprintType)
struct LUNARPUNK_API FAbilitiesDataStruct
{
	GENERATED_USTRUCT_BODY()

public:

	FAbilitiesDataStruct()
	{
		Image = nullptr;
	};


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EAbilityType Type;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere)
		UTexture2D* Image;
};


/**
 * 
 */
UCLASS()
class LUNARPUNK_API UAbilitiesData : public UDataAsset
{
	GENERATED_BODY()
	

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TMap<EAbilityType, FAbilitiesDataStruct> Abilities;
};
