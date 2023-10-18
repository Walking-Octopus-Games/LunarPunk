// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include <MediaAssets/Public/MediaSource.h>
#include "TutorialsData.generated.h"


USTRUCT(BlueprintType)
struct LUNARPUNK_API FIndicationStruct
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnyWhere)
	FString Text;

	UPROPERTY(BlueprintReadWrite, EditAnyWhere)
	UTexture2D* Icon;

	UPROPERTY(BlueprintReadWrite, EditAnyWhere)
	UTexture2D* PCIcon;

	UPROPERTY(BlueprintReadWrite, EditAnyWhere)
	FString Text2;

};

USTRUCT(BlueprintType)
struct LUNARPUNK_API FTutorialDataStruct
{
	GENERATED_USTRUCT_BODY()

public:

	FTutorialDataStruct() {};
	
	UPROPERTY(BlueprintReadWrite, EditAnyWhere)
		FString Title;

	UPROPERTY(BlueprintReadWrite, EditAnyWhere)
		FString Description;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FIndicationStruct> Indications;

	UPROPERTY(BlueprintReadWrite, EditAnyWhere)
		bool bIsVideo = true; 

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UMediaSource* MediaSource = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnyWhere)
		UTexture2D* Image = nullptr;
};

/**
 * 
 */
UCLASS(BlueprintType)
class LUNARPUNK_API UTutorialsData : public UDataAsset
{
	GENERATED_BODY()


public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FTutorialDataStruct> Tutorials;
};
