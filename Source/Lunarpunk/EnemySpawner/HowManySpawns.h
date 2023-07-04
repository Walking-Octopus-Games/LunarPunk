// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Pool/IsPoolable.h"
#include "HowManySpawns.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHowManySpawns : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LUNARPUNK_API IHowManySpawns
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//Function to implement to obtain the classes and number of actor required for each class.
	virtual void GetTotalSpawns(TMap<TSubclassOf<AActor>, int32>& ArraySpawns)=0;
};
