#include "TurretsDataAssetActions.h"

#include "Lunarpunk/Turrets/TurretsData.h"

FText FTurretsDataAssetActions::GetName() const
{
  return FText::FromString(TEXT("Turrets Data Asset"));
}

UClass* FTurretsDataAssetActions::GetSupportedClass() const
{
  return UTurretsData::StaticClass();
}

FColor FTurretsDataAssetActions::GetTypeColor() const
{
  return FColor::Magenta;
}

uint32 FTurretsDataAssetActions::GetCategories()
{
  return Category;
}

FText FTurretsDataAssetActions::GetAssetDescription(const FAssetData& AssetData) const
{
  return FText::FromString(TEXT("Contains information related to turrets"));
}

void FTurretsDataAssetActions::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
}