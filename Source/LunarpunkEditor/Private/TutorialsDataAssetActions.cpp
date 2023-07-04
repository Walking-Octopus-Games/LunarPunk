#include "TutorialsDataAssetActions.h"

#include "Lunarpunk/Tutorials/TutorialsData.h"

FText FTutorialsDataAssetActions::GetName() const
{
	return FText::FromString(TEXT("Tutorials Data Asset"));
}

UClass* FTutorialsDataAssetActions::GetSupportedClass() const
{
	return UTutorialsData::StaticClass();
}

FColor FTutorialsDataAssetActions::GetTypeColor() const
{
	return FColor::Magenta;
}

uint32 FTutorialsDataAssetActions::GetCategories()
{
	return Category;
}

FText FTutorialsDataAssetActions::GetAssetDescription(const FAssetData& AssetData) const
{
	return FText::FromString(TEXT("Contains information related to tutorials"));
}

void FTutorialsDataAssetActions::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
}