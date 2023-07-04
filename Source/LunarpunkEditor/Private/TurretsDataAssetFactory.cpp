
#include "TurretsDataAssetFactory.h"

#include "Lunarpunk/Turrets/TurretsData.h"

UTurretsDataAssetFactory::UTurretsDataAssetFactory()
{
	SupportedClass = UTurretsData::StaticClass();
	bCreateNew = true;
}

UObject* UTurretsDataAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UTurretsData>(InParent, InClass, InName, Flags);
}
