// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DropRewardsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUNARPUNK_API UDropRewardsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDropRewardsComponent();

	//Plates class reference
	UPROPERTY(EditAnyWhere)
		TSubclassOf<AActor> PlatesClass;

	//Num of plates to drop.
	UPROPERTY(EditAnywhere)
		int32 NumPlatesToDrop = 8;

	//If true, the component will drop a random number of plates bewtween MinPlatesDropped and MaxPlatesDroppedPlates.
	UPROPERTY(EditAnyWhere)
		bool bRandomPlates = false;

	//Max Num of plates to drop in Random case
	UPROPERTY(EditAnyWhere)
		int32 MaxPlatesDropped = 3;

	//Min Num of plates to drop in Random case
	UPROPERTY(EditAnyWhere)
		int32 MinPlatesDropped = 1;


	//Probability to drop and upgrade.
	UPROPERTY(EditAnyWhere)
		float UpgradesProb = 0.3f;

	//Probability of appearance of each upgrade.
	UPROPERTY(EditAnyWhere)
		TMap<TSubclassOf<AActor>, float> EachUpgradeProbs;

	//Radius in which the objects will be dropped.
	UPROPERTY(EditAnyWhere)
		float Radius = 50.0f;

	//Offset in Z axis.
	UPROPERTY(EditAnyWhere)
		float ZOffset = -87.0f;

	UPROPERTY(VisibleAnywhere)
		class APool* MyPool;


	UPROPERTY(VisibleAnywhere)
		int32 BaseNumPlatesToDrop = 8;


	UPROPERTY(VisibleAnywhere)
		bool bBaseRandomPlates = false;

	//Num of plates to drop.
	UPROPERTY(VisibleAnywhere)
		float BaseUpgradesProb = 0.0f;






protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Function called when the actor have to drop the rewards.
	UFUNCTION(BlueprintCallable)
		void DropRewards();

	void ResetValues();


private:

		int32 PlatesToDrop = 0;
		
};
