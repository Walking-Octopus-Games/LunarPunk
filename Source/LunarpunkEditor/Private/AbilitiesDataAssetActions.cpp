#include "AbilitiesDataAssetActions.h"

#include "Lunarpunk/Managers/AbilitiesData.h"

FText FAbilitiesDataAssetActions::GetName() const
{
	return FText::FromString(TEXT("Abilities Data Asset"));
}

UClass* FAbilitiesDataAssetActions::GetSupportedClass() const
{
	return UAbilitiesData::StaticClass();
}

FColor FAbilitiesDataAssetActions::GetTypeColor() const
{
	return FColor::Magenta;
}

uint32 FAbilitiesDataAssetActions::GetCategories()
{
	return Category;
}

FText FAbilitiesDataAssetActions::GetAssetDescription(const FAssetData& AssetData) const
{
	return FText::FromString(TEXT("Contains information related to abilities"));
}

void FAbilitiesDataAssetActions::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
}