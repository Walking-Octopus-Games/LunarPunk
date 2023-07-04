// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitiesDataAssetFactory.h"

#include "Lunarpunk/Managers/AbilitiesData.h"

UAbilitiesDataAssetFactory::UAbilitiesDataAssetFactory()
{
	SupportedClass = UAbilitiesData::StaticClass();
	bCreateNew = true;
}

UObject* UAbilitiesDataAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UAbilitiesData>(InParent, InClass, InName, Flags);
}
