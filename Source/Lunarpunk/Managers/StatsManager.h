// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AbilitiesManger.h"
#include "Turrets/TurretsData.h"
#include "RewardsAndUpgrades/Upgrade.h"
#include "StatsManager.generated.h"


class ALunarPunkGameMode;
class ALunarPunkPlayerController;

/**
 * 
 */
UCLASS()
class LUNARPUNK_API UStatsManager : public UObject
{
	GENERATED_BODY()

public:

	UStatsManager();

	UPROPERTY(BlueprintReadWrite, Category = "Stats | Time")
		float GameTime=0;

	UPROPERTY(BlueprintReadWrite, Category = "Stats | Rewards")
		float PlatesEarned = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Stats | Upgrades")
		TMap<EUpgradeType, float> TotalUpgradesUsed;
		

	UPROPERTY(BlueprintReadWrite, Category = "Stats | Abilities")
		TMap<EAbilityType, float> TotalAbilitiesUsed;

	UPROPERTY(BlueprintReadWrite, Category = "Stats | Damage")
		TMap<ETurretType, float> TotalDamagePerTurret;

	UPROPERTY(BlueprintReadWrite, Category = "Stats | Damage")
		float TotalDamagePlayer = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Stats | Damage")
		float TotalShootsTurrets;

	UPROPERTY(BlueprintReadWrite, Category = "Stats | Damage")
		float TotalShootsPlayer = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Stats | Damage")
		float EnemiesDefeated = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Stats | Traps")
		float TotalTrapsActivated = 0;

	UFUNCTION()
		void Initialize(ALunarPunkGameMode* GM, ALunarPunkPlayerController* PC);



private:
	UPROPERTY()
	ALunarPunkGameMode* GameMode;
	ALunarPunkPlayerController* PlayerController;

	// Increase the total shoot make by the player
	UFUNCTION()
		void IncrementTotalShootsPlayer();

	// Increase the total damage made by the player
	UFUNCTION()
		void IncrementTotalDamagePlayer(float quantity);

	// Increase the total shoots make by the turrets
	UFUNCTION()
		void IncrementTotalShootsTurrets();
	// Increase the plates earned
	UFUNCTION()
		void IncrementPlatesEarned(int32 quatiNumPlatesUpdatety);
	// Set the time transcurred in the game
	UFUNCTION()
		void SetGameTime();
	// Increase the enemies defeated by the player and the turrets
	UFUNCTION()
		void IncrementEnemiesDefeated();

	// Increase the turrets activated 
	UFUNCTION()
		void IncrementTotalTrapsActivated();

	// Increase the turrets activated 
	UFUNCTION()
		void IncrementTotalUpgradesUsed(EUpgradeType type);

	// Increase the turrets activated 
	UFUNCTION()
		void IncrementTotalAbilitiesUsed(EAbilityType type);

	// Increase the total damage made by the player
	UFUNCTION()
		void IncrementTotalDamageTurrets(ETurretType TurretType, float quantity);
	
};
