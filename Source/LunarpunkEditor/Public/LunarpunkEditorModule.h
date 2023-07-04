// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleInterface.h"

class FWaveDataAssetActions;
class FTurretsDataAssetActions;
class FUpgradesDataAssetActions;
class FTutorialsDataAssetActions;
class FAbilitiesDataAssetActions;

class FLunarpunkEditorModule : public IModuleInterface
{
public:
  virtual void StartupModule() override;
  virtual void ShutdownModule() override;

private: 
  TSharedPtr<FWaveDataAssetActions> WaveDataAssetActions;
  TSharedPtr<FTurretsDataAssetActions> TurretsDataAssetActions;
  TSharedPtr<FUpgradesDataAssetActions> UpgradesDataAssetActions;
  TSharedPtr<FTutorialsDataAssetActions> TutorialsDataAssetActions;
  TSharedPtr<FAbilitiesDataAssetActions> AbilitiesDataAssetActions;
};