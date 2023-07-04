
#pragma once

#include "Factories/Factory.h"

#include "TurretsDataAssetFactory.generated.h"

UCLASS()
class LUNARPUNKEDITOR_API UTurretsDataAssetFactory : public UFactory
{
	GENERATED_BODY()

public:

	UTurretsDataAssetFactory();
	UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

};
