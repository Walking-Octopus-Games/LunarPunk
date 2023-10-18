// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConquestZone.generated.h"

using TypeDelegate = const TMap<TSubclassOf<AActor>, int32>&;

DECLARE_MULTICAST_DELEGATE_OneParam(FMaxEnemiesInConquestZoneChanged, TypeDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShowPlayerCanShoot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHidePlayerCanShoot);
DECLARE_MULTICAST_DELEGATE_OneParam(FZoneCompleted, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FZoneStarted, int32);

UCLASS()
class LUNARPUNK_API AConquestZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AConquestZone();

	//Time to rest before enemies start to come.
	UPROPERTY(EditAnywhere)
	int32 RestingTime = 20.0f;


	UPROPERTY(BlueprintReadWrite)
	//Manage the remaining time of the resting.
	int32 CurrentTimeToEndResting = 0;

	//Index os the ConquestZone to asociate the portal and Turrets Conquest Places
	UPROPERTY(EditAnywhere)
	int32 Index = 1;

	//Max enemies in screen of each class in the area asociated to this  conquest zone. (-1 means no limit)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<AActor>, int32> MaxEnemiesInScreen;

	UPROPERTY(EditAnywhere)
	bool bLimitEnemiesNumberWhenTurretsAreDeath = true;

	//Number of remaining alive turrets to change the max number of enemies in screen. 0 Means that this number will change when every turret is death.
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bLimitEnemiesNumberWhenTurretsAreDeath", EditConditionHides))
	int32 AliveTurretsToChangeMaxEnemiesInScreen = 0;

	//Number of turrets wich should be alive to restore the regular max number of enemies in screen. (Note: this value must be greater than AliveTurretsToChangeMaxEnemiesInScreen).
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bLimitEnemiesNumberWhenTurretsAreDeath", EditConditionHides))
	int32 AliveTurretsToRestoreRegularEnemiesNumber = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bLimitEnemiesNumberWhenTurretsAreDeath", EditConditionHides))
	TMap<TSubclassOf<AActor>, int32> MaxEnemiesInScreenWhenTurretsAreDeath;


	//Time to show the remaining time.
	UPROPERTY(EditAnywhere)
	int32 TimeToShowRestingTime = 10.4f;


	//Num of turrets placed in asociated turrets places. 
	UPROPERTY(VisibleAnywhere)
	int32 CurrentTurretsPlacedInConquestPlaces = 0;


	//If True, the player is in the zone.
	bool bPlayerIsInZone = false;

	//Collision of the conquest area
	class UBoxComponent* ConquestArea;

	//Player reference
	class APlayableCharacter* PlayerReference;

	//Array of asociated Turrets conquest places
	TArray<class ATurretConquestPlace*> TurretsConquestPlaces;

	//Array of Zone Triggers asociated to this ConquesZone.
	TArray<class AZoneTrigger*> ZoneTriggersAsociated;

	//Total num of Turrets conquest places
	int32 NumTurretsConquestPlacesAsociated;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable)
	FShowPlayerCanShoot ShowPlayerCanShoot;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable)
	FHidePlayerCanShoot HidePlayerCanShoot;


	FMaxEnemiesInConquestZoneChanged MaxEnemiesInConquestZoneChanged;


	//Spawners asociated to this conquest zone.
	UPROPERTY(VisibleAnywhere)
	TArray<class AEnemySpawner*> EnemySpawners;

	FZoneCompleted ZoneCompleted;

	FZoneStarted ZoneStarted;

	UPROPERTY(BlueprintReadWrite)
	bool bShowingRestingTime = false;

private:

	
	UPROPERTY(VisibleAnywhere, Category = "Debug")
	int32 TurretsInLevel = 0;

	UPROPERTY(VisibleAnywhere, Category = "Debug")
	int32 TurretsDeathInLevel = 0;

	UPROPERTY(VisibleAnywhere, Category = "Debug")
	TMap<TSubclassOf<AActor>, int32> InitialMaxEnemiesInScreen;

	//Handle del Timer
	FTimerHandle TimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		virtual void OnBeginOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
		);


	UFUNCTION()
		void OnEndOverlap(UPrimitiveComponent* OverlappedComponent
			, AActor* OtherActor
			, UPrimitiveComponent* OtherComp
			, int32 OtherBodyIndex
		);


	//Functions to enable a disable the conquest zone
	void EnableConquestZone();
	void DisableConquestZone();


	//Callbacks to handle when a turret is placed or retired in a turret conquest place asociated.
	UFUNCTION()
	void OnTurretPlaced();

	UFUNCTION()
	void OnTurretRetired();

	UFUNCTION()
	void OnTurretDeath();

	UFUNCTION()
	void OnTurretReactivated();

	//Functions to handle if the player can perform the shoot or not.
	void ActivatePlayerCanChargeShoot();

	void DeactivatePlayerCanChargeShoot();

	//Functions to show the zone effect
	UFUNCTION(BlueprintImplementableEvent)
	void ShowZoneActivatedEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void RemoveZoneActivatedEffect();

	UFUNCTION()
	void ConquestZoneCompleted(bool IsLastHit, int IndexPortal);


	UFUNCTION()
	void OpenConquestZone();

	UFUNCTION(BlueprintCallable)
	void PauseConquestZone();

	UFUNCTION(BlueprintCallable)
	void ResumeConquestZone();

	//Search and stores all the enemies spawners which index is the same as the sector index.
	void GetEnemySpawnersAsociated();

	//Function to activate the asociated enemy Spawners.
	UFUNCTION(BlueprintCallable)
	void ActivateEnemySpawnersAsociated();

	//Function to activate the asociated enemy Spawners.
	void DeactivateEnemySpawnersAsociated();


	float GetCurrentTimeEndResting();

	//Deactivate all the enemies and put them back in the pool
	void DeactivateAllEnemiesInMap() const;



	void UpdateTimeToNextWave();

	UFUNCTION(BlueprintImplementableEvent)
	void RefreshHUDTimeToNextWave();

	UFUNCTION(BlueprintImplementableEvent)
	void HideTimeToNextWaveInHUD();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowTimeToNextWaveInHUD();

	UFUNCTION()
	void ManageZoneTriggerOverlapped(bool ShouldDelay, float DelayTime);

	UFUNCTION()
	void ForceBeginEnemiesSpawners();


};
