// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "UpgradesDataAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class LUNARPUNKEDITOR_API UUpgradesDataAssetFactory : public UFactory
{
	GENERATED_BODY()

public:

	UUpgradesDataAssetFactory();
	UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

};
