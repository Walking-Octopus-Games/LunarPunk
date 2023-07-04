// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradesDataAssetFactory.h"

#include "Lunarpunk/RewardsAndUpgrades/UpgradesData.h"

UUpgradesDataAssetFactory::UUpgradesDataAssetFactory()
{
	SupportedClass = UUpgradesData::StaticClass();
	bCreateNew = true;
}

UObject* UUpgradesDataAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UUpgradesData>(InParent, InClass, InName, Flags);
}
