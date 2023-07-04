#pragma once

#include "AssetTypeActions_Base.h"

class LUNARPUNKEDITOR_API FWaveDataAssetActions : public FAssetTypeActions_Base
{
public:

  FWaveDataAssetActions(EAssetTypeCategories::Type ValueCategory) : Category(ValueCategory) {}
  FText GetName() const override;
  UClass* GetSupportedClass() const override;
  FColor GetTypeColor() const override;
  uint32 GetCategories() override;
  FText GetAssetDescription(const FAssetData& AssetData) const override;

  virtual bool HasActions(const TArray<UObject*>& InObjects) const override
  {
    return true;
  }

  virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;

private:

  EAssetTypeCategories::Type Category;

  void OpenFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, TArray<FString>& OutFileNames);

  void SaveFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, TArray<FString>& OutFileNames);

  void LoadFile();

  void ExportFile();

};
