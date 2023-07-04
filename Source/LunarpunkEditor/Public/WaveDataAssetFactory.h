
#pragma once

#include "Factories/Factory.h"

#include "WaveDataAssetFactory.generated.h"

UCLASS()
class LUNARPUNKEDITOR_API UWaveDataAssetFactory : public UFactory
{
	GENERATED_BODY()

public:

	UWaveDataAssetFactory();
	UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

};
