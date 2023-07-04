// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialsDataAssetFactory.h"

#include "Lunarpunk/Tutorials/TutorialsData.h"


UTutorialsDataAssetFactory::UTutorialsDataAssetFactory()
{
	SupportedClass = UTutorialsData::StaticClass();
	bCreateNew = true;
}

UObject* UTutorialsDataAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UTutorialsData>(InParent, InClass, InName, Flags);
}