// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "AbilitiesDataAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class LUNARPUNKEDITOR_API UAbilitiesDataAssetFactory : public UFactory
{
	GENERATED_BODY()

	UAbilitiesDataAssetFactory();
	UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	
};
