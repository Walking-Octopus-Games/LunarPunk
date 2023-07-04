// Fill out your copyright notice in the Description page of Project Settings.

#include "WaveData.h"

#include "Serialization/JsonReader.h"
#include "Templates/SharedPointer.h"
#include "Lunarpunk/Enemies/BasicEnemy.h"




bool UWaveData::LoadFile(FString FileName)
{
	bool bSuccessfulParse = false;

	TSharedPtr<FJsonObject> JsonObject;
	FString JsonString;

	FFileHelper::LoadFileToString(JsonString, *FileName);
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		ParseFile(JsonObject);
		bSuccessfulParse = true;
	}
	return bSuccessfulParse;
}

bool UWaveData::ExportFile(FString FileName)
{
	bool bSuccessfulWrite = false;

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);

	WriteJson(JsonObject);

	if (FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer))
	{
		if (FFileHelper::SaveStringToFile(JsonString, *FileName))
		{
			bSuccessfulWrite = true;
		}
	}

	return bSuccessfulWrite;
}

void UWaveData::ParseFile(TSharedPtr<FJsonObject> JsonObject)
{
	TArray<TSharedPtr<FJsonValue>> ObjArray = JsonObject->GetArrayField(TEXT("Waves"));

	Waves.Empty();

	for (int32 i = 0; i < ObjArray.Num(); i++)
	{
		TSharedPtr<FJsonValue> Value = ObjArray[i];
		TSharedPtr<FJsonObject> Json = Value->AsObject();

		int32	TotalEnemies = Json->GetNumberField(TEXT("TotalEnemies"));
		float	TimeToStart = Json->GetNumberField(TEXT("TimeToStart"));
		float TimeToSpawn = Json->GetNumberField(TEXT("TimeToSpawn"));
		bool	bSpawnRandomNumOfEnemiesEachTime = Json->GetBoolField(TEXT("SpawnRandomNumOfEnemiesEachTime"));
		
		TMap<TSubclassOf<AActor>, float> EnemiesClasses;
		ReadContainerData(EnemiesClasses, "EnemiesClasses", Json);

		TMap<TSubclassOf<AActor>, int32> MaxNumSpawnedEachTime;
		ReadContainerData(MaxNumSpawnedEachTime, "MaxNumSpawnedEachTime", Json);

		TMap<TSubclassOf<AActor>, float> SpeedPercentageVariation;
		ReadContainerData(SpeedPercentageVariation, "SpeedPercentageVariation", Json);

		TMap<TSubclassOf<AActor>, float> HPPercentageVariation;
		ReadContainerData(HPPercentageVariation, "HPPercentageVariation", Json);

		TMap<TSubclassOf<AActor>, float> StrengthPercentageVariation;
		ReadContainerData(StrengthPercentageVariation, "StrengthPercentageVariation", Json);

		TMap<TSubclassOf<AActor>, int32> PlatesVariation;
		ReadContainerData(PlatesVariation, "PlatesVariation", Json);

		TMap<TSubclassOf<AActor>, float> UpgradesVariation;
		ReadContainerData(UpgradesVariation, "UpgradesVariation", Json);

		FWaveDataStruct WaveStruct = FWaveDataStruct(
			TotalEnemies
			, TimeToStart
			, TimeToSpawn
			, bSpawnRandomNumOfEnemiesEachTime
			, EnemiesClasses
			, MaxNumSpawnedEachTime
			, SpeedPercentageVariation
			, HPPercentageVariation
			, StrengthPercentageVariation
			, PlatesVariation
			, UpgradesVariation
		);
		
		Waves.Push(WaveStruct);
	}
}

void UWaveData::WriteJson(TSharedPtr<FJsonObject> JsonObject)
{
	TArray<TSharedPtr<FJsonValue>> WavesArray;
	
	for (const FWaveDataStruct& WaveStruct : Waves)
	{
		TSharedPtr<FJsonObject> WaveObject = MakeShareable(new FJsonObject());

		WaveObject->SetNumberField("TotalEnemies", WaveStruct.TotalEnemies);
		WaveObject->SetNumberField("TimeToStart", WaveStruct.TimeToStart);
		WaveObject->SetNumberField("TimeToSpawn", WaveStruct.TimeToSpawn);
		WaveObject->SetBoolField("SpawnRandomNumOfEnemiesEachTime", WaveStruct.bSpawnRandomNumOfEnemiesEachTime);

		WriteContainerData(WaveStruct.EnemiesClasses, "EnemiesClasses", WaveObject);
		WriteContainerData(WaveStruct.MaxNumSpawnedEachTime, "MaxNumSpawnedEachTime", WaveObject);
		WriteContainerData(WaveStruct.SpeedPercentageVariation, "SpeedPercentageVariation", WaveObject);
		WriteContainerData(WaveStruct.HPPercentageVariation, "HPPercentageVariation", WaveObject);
		WriteContainerData(WaveStruct.StrengthPercentageVariation, "StrengthPercentageVariation", WaveObject);

		WavesArray.Add(MakeShareable(new FJsonValueObject(WaveObject)));
	}

	JsonObject->SetArrayField("Waves", WavesArray);
}

void UWaveData::ReadContainerData(TMap<TSubclassOf<AActor>, float>& Data, const FString& DataName, TSharedPtr<FJsonObject> Json)
{
	TArray<TSharedPtr<FJsonValue>> ObjectArray = Json->GetArrayField(DataName);
	double EnemyValue;

	for (TSharedPtr<FJsonValue>& Value : ObjectArray)
	{
		TSharedPtr<FJsonObject> JsonObject = Value->AsObject();

		for (TSubclassOf<AActor>& Enemy : SupportedEnemies)
		{
			if (Enemy)
			{
				if (JsonObject->TryGetNumberField(Enemy->ClassGeneratedBy->GetName(), EnemyValue))
				{
					Data.Add(Enemy, (float)EnemyValue);
				}
			}
		}
	}
}

void UWaveData::ReadContainerData(TMap<TSubclassOf<AActor>, int32>& Data, const FString& DataName, TSharedPtr<FJsonObject> Json)
{
	TArray<TSharedPtr<FJsonValue>> ObjectArray = Json->GetArrayField(DataName);
	double EnemyValue;

	for (TSharedPtr<FJsonValue>& Value : ObjectArray)
	{
		TSharedPtr<FJsonObject> JsonObject = Value->AsObject();

		for (TSubclassOf<AActor>& Enemy : SupportedEnemies)
		{
			if (Enemy)
			{
				if (JsonObject->TryGetNumberField(Enemy->ClassGeneratedBy->GetName(), EnemyValue))
				{
					Data.Add(Enemy, (float)EnemyValue);
				}
			}
		}
	}
}

void UWaveData::WriteContainerData(const TMap<TSubclassOf<AActor>, float>& Data, const FString& DataName, TSharedPtr<FJsonObject> WaveObject)
{
	if (Data.Num() > 0)
	{
		TArray<TSharedPtr<FJsonValue>> DataValue;

		for (TSubclassOf<AActor>& Enemy : SupportedEnemies)
		{
			if (Enemy && Data.Contains(Enemy))
			{
				TSharedPtr<FJsonObject> Object = MakeShareable(new FJsonObject());
				Object->SetNumberField(Enemy->ClassGeneratedBy->GetName(), (double)*Data.Find(Enemy));
				DataValue.Add(MakeShareable(new FJsonValueObject(Object)));
			}
		}

		if (DataValue.Num() > 0)
		{
			WaveObject->SetArrayField(DataName, DataValue);
		}
	}
}

void UWaveData::WriteContainerData(const TMap<TSubclassOf<AActor>, int32>& Data, const FString& DataName, TSharedPtr<FJsonObject> WaveObject)
{
	if (Data.Num() > 0)
	{
		TArray<TSharedPtr<FJsonValue>> DataValue;

		for (TSubclassOf<AActor>& Enemy : SupportedEnemies)
		{
			if (Enemy && Data.Contains(Enemy))
			{
				TSharedPtr<FJsonObject> Object = MakeShareable(new FJsonObject());
				Object->SetNumberField(Enemy->ClassGeneratedBy->GetName(), (double)*Data.Find(Enemy));
				DataValue.Add(MakeShareable(new FJsonValueObject(Object)));
			}
		}

		if (DataValue.Num() > 0)
		{
			WaveObject->SetArrayField(DataName, DataValue);
		}
	}
}
