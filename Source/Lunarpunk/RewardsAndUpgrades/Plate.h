// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RewardsAndUpgrades/Reward.h"
#include "Plate.generated.h"

/**
 * 
 */
UCLASS()
class LUNARPUNK_API APlate : public AReward
{
	GENERATED_BODY()



public:

		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		UClass* GetTypeOfReward() override;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Quantity = 1;


		UFUNCTION(BlueprintImplementableEvent)
			void UpdateQuantityWidget();


		void SetQuantity(int32 NewQuantity);


		virtual void OnBeginOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
		)override;


		virtual void AddToHUD(int32 QuantityToAdd) override;

};
