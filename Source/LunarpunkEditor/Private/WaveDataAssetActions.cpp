#include "WaveDataAssetActions.h"

#include "Lunarpunk/EnemySpawner/WaveData.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "EditorStyle.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Interfaces/IMainFrameModule.h"
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "FWaveDataAssetActions"

FText FWaveDataAssetActions::GetName() const
{
  return FText::FromString(TEXT("Wave Data Asset"));
}

UClass* FWaveDataAssetActions::GetSupportedClass() const
{
  return UWaveData::StaticClass();
}

FColor FWaveDataAssetActions::GetTypeColor() const
{
  return FColor::Magenta;
}

uint32 FWaveDataAssetActions::GetCategories()
{
  return Category;
}

FText FWaveDataAssetActions::GetAssetDescription(const FAssetData& AssetData) const
{
  return FText::FromString(TEXT("Contains information related to waves"));
}


void FWaveDataAssetActions::OpenFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, TArray<FString>& OutFileNames)
{
  TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
  void* ParentWindowHandle = nullptr;

  if (ParentWindow.IsValid() && ParentWindow->GetNativeWindow().IsValid())
  {
    ParentWindowHandle = ParentWindow->GetNativeWindow()->GetOSWindowHandle();
  }

  IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
  if (DesktopPlatform)
  {
    uint32 SelectionFlag = 0; //A value of 0 represents single file selection while a value of 1 represents multiple file selection
    DesktopPlatform->OpenFileDialog(ParentWindowHandle, DialogTitle, DefaultPath, FString(""), FileTypes, SelectionFlag, OutFileNames);
  }
}

void FWaveDataAssetActions::SaveFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, TArray<FString>& OutFileNames)
{
  TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
  void* ParentWindowHandle = nullptr;

  if (ParentWindow.IsValid() && ParentWindow->GetNativeWindow().IsValid())
  {
    ParentWindowHandle = ParentWindow->GetNativeWindow()->GetOSWindowHandle();
  }

  IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
  if (DesktopPlatform)
  {
    uint32 SelectionFlag = 0; //A value of 0 represents single file selection while a value of 1 represents multiple file selection
    DesktopPlatform->SaveFileDialog(ParentWindowHandle, DialogTitle, DefaultPath, FString(""), FileTypes, SelectionFlag, OutFileNames);
  }
}


void FWaveDataAssetActions::LoadFile()
{
  if (FModuleManager::Get().IsModuleLoaded("ContentBrowser"))
  {
    FContentBrowserModule& ContentBrowserModule = FModuleManager::GetModuleChecked<FContentBrowserModule>("ContentBrowser");
    IContentBrowserSingleton& ContentBrowserTools = ContentBrowserModule.Get();
    TArray<FAssetData> SelectedAssets;
    ContentBrowserTools.GetSelectedAssets(SelectedAssets);

    for (FAssetData& AssetData : SelectedAssets)
    {
      if (AssetData.GetClass() == UWaveData::StaticClass())
      {
        UWaveData* WaveData = Cast<UWaveData>(AssetData.GetAsset());

        TArray<FString> Files;
        OpenFileDialog(TEXT("Select file to be loaded"), FPaths::Combine(FPaths::ProjectDir(), TEXT("Data/")), TEXT("Json Files|*.json"), Files);
        if (Files.Num() > 0)
        {
          if (WaveData->LoadFile(Files[0]))
          {
            //File loaded successfully
            UE_LOG(LogTemp, Display, TEXT("Parsed file %s"), *(Files[0]));

            FNotificationInfo Info(LOCTEXT("JsonLoaded", "Loaded Json file"));
            Info.ExpireDuration = 6.f;
            Info.FadeOutDuration = 1.0f;
            Info.bUseSuccessFailIcons = true;

            FSlateNotificationManager::Get().AddNotification(Info);
          }
          else
          {
            //File loading has failed
            UE_LOG(LogTemp, Error, TEXT("Could not parse json file %s"), *(Files[0]));

            FNotificationInfo Info(LOCTEXT("JsonNotLoaded", "Json file could not be parsed"));
            Info.ExpireDuration = 6.f;
            Info.FadeOutDuration = 1.0f;
            Info.bUseSuccessFailIcons = true;
            Info.Image = FEditorStyle::GetBrush(TEXT("MessageLog.Error"));

            FSlateNotificationManager::Get().AddNotification(Info);
          }
        }
      }
    }
  }
}

void FWaveDataAssetActions::ExportFile()
{
  if (FModuleManager::Get().IsModuleLoaded("ContentBrowser"))
  {
    FContentBrowserModule& ContentBrowserModule = FModuleManager::GetModuleChecked<FContentBrowserModule>("ContentBrowser");
    IContentBrowserSingleton& ContentBrowserTools = ContentBrowserModule.Get();
    TArray<FAssetData> SelectedAssets;
    ContentBrowserTools.GetSelectedAssets(SelectedAssets);

    for (FAssetData& AssetData : SelectedAssets)
    {
      if (AssetData.GetClass() == UWaveData::StaticClass())
      {
        UWaveData* WaveData = Cast<UWaveData>(AssetData.GetAsset());

        TArray<FString> Files;
        SaveFileDialog(TEXT("Save as"), FPaths::Combine(FPaths::ProjectDir(), TEXT("Data/")), TEXT("Json Files|*.json"), Files);
        if (Files.Num() > 0)
        {
          if (WaveData->ExportFile(Files[0]))
          {
            //File saved successfully
            UE_LOG(LogTemp, Display, TEXT("Saved file %s"), *(Files[0]));

            FNotificationInfo Info(LOCTEXT("FileSaved", "Saved file successfuly"));
            Info.ExpireDuration = 6.f;
            Info.FadeOutDuration = 1.0f;
            Info.bUseSuccessFailIcons = true;

            FSlateNotificationManager::Get().AddNotification(Info);
          }
          else
          {
            //File saving has failed
            UE_LOG(LogTemp, Error, TEXT("Could not parse json file %s"), *(Files[0]));

            FNotificationInfo Info(LOCTEXT("FileNotSaved", "File could not be saved"));
            Info.ExpireDuration = 6.f;
            Info.FadeOutDuration = 1.0f;
            Info.bUseSuccessFailIcons = true;
            Info.Image = FEditorStyle::GetBrush(TEXT("MessageLog.Error"));

            FSlateNotificationManager::Get().AddNotification(Info);
          }
        }
      }
    }
  }
}

void FWaveDataAssetActions::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
  MenuBuilder.AddMenuEntry(
    FText::FromString(TEXT("Load JSON file")),
    FText::FromString(TEXT("Load contents of JSON file.")),
    FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.ViewOptions"),
    FUIAction(FExecuteAction::CreateRaw(this, &FWaveDataAssetActions::LoadFile))
  );
  MenuBuilder.AddMenuEntry(
    FText::FromString(TEXT("Export to JSON file")),
    FText::FromString(TEXT("Export the contents of the asset to a JSON file.")),
    FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.ViewOptions"),
    FUIAction(FExecuteAction::CreateRaw(this, &FWaveDataAssetActions::ExportFile))
  );
}