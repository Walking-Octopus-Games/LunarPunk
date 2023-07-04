// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RewardsAndUpgrades/Reward.h"
#include "Upgrade.generated.h"

/**
 * 
 */
class UNiagaraSystem;

UENUM(BlueprintType)
enum class EUpgradeType : uint8
{
	None			UMETA(Hidden)
	, Power			UMETA(DisplayName = "Power")
	, Cadence		UMETA(DisplayName = "Cadence")
	, Range			UMETA(DisplayName = "Range")
	, Health		UMETA(DisplayName = "Health")
	, Sparkle		UMETA(DisplayName = "Sparkle")
	, Size			UMETA(DisplayName = "Size")
};

UCLASS()
class LUNARPUNK_API AUpgrade : public AReward
{
	GENERATED_BODY()
	

public:
	AUpgrade();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		UClass* GetTypeOfReward() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EUpgradeType Type;


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
