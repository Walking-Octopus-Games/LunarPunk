// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "WaveData.generated.h"

USTRUCT(BlueprintType)
struct LUNARPUNK_API FWaveDataStruct
{
	GENERATED_USTRUCT_BODY()

public:

	FWaveDataStruct() {};

	FWaveDataStruct(int TotalEnemies
		, float TimeToStart
		, float TimeToSpawn
		, bool bSpawnRandomNumOfEnemiesEachTime
		, TMap<TSubclassOf<AActor>, float> EnemiesClasses
		, TMap<TSubclassOf<AActor>, int32> MaxNumSpawnedEachTime
		, TMap<TSubclassOf<AActor>, float> SpeedPercentageVariation
		, TMap<TSubclassOf<AActor>, float> HPPercentageVariation
		, TMap<TSubclassOf<AActor>, float> StrengthPercentageVariation
		, TMap<TSubclassOf<AActor>, int32> PlatesVariationValue
		, TMap<TSubclassOf<AActor>, float> UpgradesVariationValue
	) 
		: TotalEnemies(TotalEnemies)
		, TimeToStart(TimeToStart)
		, TimeToSpawn(TimeToSpawn)
		, bSpawnRandomNumOfEnemiesEachTime(bSpawnRandomNumOfEnemiesEachTime)
		, EnemiesClasses(EnemiesClasses)
		, MaxNumSpawnedEachTime(MaxNumSpawnedEachTime)
		, SpeedPercentageVariation(SpeedPercentageVariation)
		, HPPercentageVariation(HPPercentageVariation)
		, StrengthPercentageVariation(StrengthPercentageVariation)
		, PlatesVariation(PlatesVariationValue)
		, UpgradesVariation(UpgradesVariationValue)
	{};

	//Total enemies to spawn in this wave
	UPROPERTY(EditAnyWhere, meta = (ClampMin = "0", UIMin = "0"))
		int TotalEnemies = 20;

	//Time to start first spawn after the wave starts
	UPROPERTY(EditAnyWhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
		float TimeToStart = 2.f;

	//Spawn cadence
	UPROPERTY(EditAnyWhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
		float TimeToSpawn = 0.5f;

	//If true the spawner will try to spawn a random number of enemies each time from 1 to MaxNumEnemyToSpawnEachTime
	UPROPERTY(EditAnyWhere)
		bool bSpawnRandomNumOfEnemiesEachTime = true;

	//Classes to spawn in this wave and their percentage out of TotalEnemies value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave", meta = (ClampMin = "0.0", UIMin = "0.0"))
		TMap<TSubclassOf<AActor>, float> EnemiesClasses;

	//Max number of enemies spawned each time by class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave", meta = (ClampMin = "0.0", UIMin = "0.0"))
		TMap<TSubclassOf<AActor>, int32> MaxNumSpawnedEachTime;

	//Speed percentage variation for each class in this wave
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave", meta = (ClampMin = "0.0", UIMin = "0.0"))
		TMap<TSubclassOf<AActor>, float> SpeedPercentageVariation;

	//HP percentage variation for each class in this wave
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave", meta = (DisplayName = "HP Percentage Variation", ClampMin = "0.0", UIMin = "0.0"))
		TMap<TSubclassOf<AActor>, float> HPPercentageVariation;

	//Strength percentage variation for each class in this wave
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave", meta = (ClampMin = "0.0", UIMin = "0.0"))
		TMap<TSubclassOf<AActor>, float> StrengthPercentageVariation;

	//Plates variation for each class in this wave
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave", meta = (ClampMin = "0.0", UIMin = "0.0"))
		TMap<TSubclassOf<AActor>, int32> PlatesVariation;

	//Plates variation for each class in this wave
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave", meta = (ClampMin = "0.0", UIMin = "0.0"))
		TMap<TSubclassOf<AActor>, float> UpgradesVariation;

};

/**
 * 
 */
UCLASS()
class LUNARPUNK_API UWaveData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<AActor>> SupportedEnemies;

	UPROPERTY(EditAnywhere)
		TArray<FWaveDataStruct> Waves;

	UFUNCTION()
		bool LoadFile(FString FileName);

	UFUNCTION()
		bool ExportFile(FString FileName);

private:
	void ParseFile(TSharedPtr<FJsonObject> JsonObject);

	void WriteJson(TSharedPtr<FJsonObject> JsonObject);

	void ReadContainerData(TMap<TSubclassOf<AActor>, float>& Data, const FString& DataName, TSharedPtr<FJsonObject> Json);

	void ReadContainerData(TMap<TSubclassOf<AActor>, int32>& Data, const FString& DataName, TSharedPtr<FJsonObject> Json);

	void WriteContainerData(const TMap<TSubclassOf<AActor>, float>& Data, const FString& DataName, TSharedPtr<FJsonObject> WaveObject);

	void WriteContainerData(const TMap<TSubclassOf<AActor>, int32>& Data, const FString& DataName, TSharedPtr<FJsonObject> WaveObject);

};
