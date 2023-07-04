
#include "WaveDataAssetFactory.h"

#include "Lunarpunk/EnemySpawner/WaveData.h"

UWaveDataAssetFactory::UWaveDataAssetFactory()
{
	SupportedClass = UWaveData::StaticClass();
	bCreateNew = true;
}

UObject* UWaveDataAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UWaveData>(InParent, InClass, InName, Flags);
}
