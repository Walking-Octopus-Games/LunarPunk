// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Factories/Factory.h"
#include "TutorialsDataAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class LUNARPUNKEDITOR_API UTutorialsDataAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	UTutorialsDataAssetFactory();

	UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

};
