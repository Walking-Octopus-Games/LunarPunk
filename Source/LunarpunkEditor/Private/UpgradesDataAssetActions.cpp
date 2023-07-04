#include "UpgradesDataAssetActions.h"

#include "Lunarpunk/RewardsAndUpgrades/UpgradesData.h"

FText FUpgradesDataAssetActions::GetName() const
{
  return FText::FromString(TEXT("Upgrades Data Asset"));
}

UClass* FUpgradesDataAssetActions::GetSupportedClass() const
{
  return UUpgradesData::StaticClass();
}

FColor FUpgradesDataAssetActions::GetTypeColor() const
{
  return FColor::Magenta;
}

uint32 FUpgradesDataAssetActions::GetCategories()
{
  return Category;
}

FText FUpgradesDataAssetActions::GetAssetDescription(const FAssetData& AssetData) const
{
  return FText::FromString(TEXT("Contains information related to upgrades"));
}

void FUpgradesDataAssetActions::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
}