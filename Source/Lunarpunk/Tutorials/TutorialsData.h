// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include <MediaAssets/Public/MediaSource.h>
#include "TutorialsData.generated.h"


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
