// Fill out your copyright notice in the Description page of Project Settings.

#include "LunarpunkEditorModule.h"

#include "EngineUtils.h"
#include "LevelEditor.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Modules/ModuleManager.h"

#include "WaveDataAssetActions.h"
#include "TurretsDataAssetActions.h"
#include "UpgradesDataAssetActions.h"
#include "TutorialsDataAssetActions.h"
#include "AbilitiesDataAssetActions.h"

IMPLEMENT_MODULE(FLunarpunkEditorModule, LunarpunkEditor);

void FLunarpunkEditorModule::StartupModule()
{
	FAssetToolsModule& AssetsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	IAssetTools& AssetsTools = AssetsModule.Get();

	EAssetTypeCategories::Type DataCategory = AssetsTools.RegisterAdvancedAssetCategory(FName(TEXT("Data")), FText::FromString("Data"));

	FWaveDataAssetActions* WaveDataActions = new FWaveDataAssetActions(DataCategory);
	FTurretsDataAssetActions* TurretsDataActions = new FTurretsDataAssetActions(DataCategory);
	FTutorialsDataAssetActions* TutorialsDataActions = new FTutorialsDataAssetActions(DataCategory);
	FUpgradesDataAssetActions* UpgradesDataActions = new FUpgradesDataAssetActions(DataCategory);
	FAbilitiesDataAssetActions* AbilitiesDataActions = new FAbilitiesDataAssetActions(DataCategory);

	WaveDataAssetActions = MakeShareable(WaveDataActions);
	TurretsDataAssetActions = MakeShareable(TurretsDataActions);
	UpgradesDataAssetActions = MakeShareable(UpgradesDataActions);
	TutorialsDataAssetActions = MakeShareable(TutorialsDataActions);
	AbilitiesDataAssetActions = MakeShareable(AbilitiesDataActions);

	AssetsTools.RegisterAssetTypeActions(WaveDataAssetActions.ToSharedRef());
	AssetsTools.RegisterAssetTypeActions(TurretsDataAssetActions.ToSharedRef());
	AssetsTools.RegisterAssetTypeActions(UpgradesDataAssetActions.ToSharedRef());
	AssetsTools.RegisterAssetTypeActions(TutorialsDataAssetActions.ToSharedRef());
	AssetsTools.RegisterAssetTypeActions(AbilitiesDataAssetActions.ToSharedRef());
}

void FLunarpunkEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetsTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		AssetsTools.UnregisterAssetTypeActions(WaveDataAssetActions.ToSharedRef());
		AssetsTools.UnregisterAssetTypeActions(TurretsDataAssetActions.ToSharedRef());
		AssetsTools.UnregisterAssetTypeActions(UpgradesDataAssetActions.ToSharedRef());
		AssetsTools.RegisterAssetTypeActions(TutorialsDataAssetActions.ToSharedRef());
		AssetsTools.RegisterAssetTypeActions(AbilitiesDataAssetActions.ToSharedRef());

		WaveDataAssetActions.Reset();
		TurretsDataAssetActions.Reset();
		UpgradesDataAssetActions.Reset();
		TutorialsDataAssetActions.Reset();
		AbilitiesDataAssetActions.Reset();
	}
}