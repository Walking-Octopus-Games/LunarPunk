// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sector.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FSectorClosed, int32);

UCLASS()
class LUNARPUNK_API ASector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASector();

	//Collision to manage the enemies
	UPROPERTY(EditAnywhere)
	class UBoxComponent* SectorCollision;

	//Collision to manage the enemies
	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* SectorExit;

	//Point to emplace the turrets when the sector opens
	UPROPERTY(EditAnywhere)
	class USphereComponent* TurretsZone;

	//Index of the sector in the level.
	UPROPERTY(EditAnywhere)
	int32 Index = 0;

	//Time to rest before enemies start to come.
	UPROPERTY(EditAnywhere)
	float RestingTime = 20.0f;

	//Spawners asociated to this sector.
	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> EnemySpawners;

	//Time you need to have the sector empty of enemies to conquer the sector.
	UPROPERTY(EditAnywhere)
	float TimeToConquer = 10.0f;

	//If true, the sector is active.
	UPROPERTY(EditAnywhere)
	bool bActive = false;

	//True when the first enemy enters in the sector.
	UPROPERTY(VisibleAnywhere)
	bool bStarted = false;


	//If true, this sector is the last sector of the level.
	UPROPERTY(EditAnywhere)
	bool bIsLastSector = false;

	//Number of enemies into the sector.
	UPROPERTY(VisibleAnywhere)
	int32 CurrentEnemiesInSector = 0;

	//Delegate to announce that the sector closes.
	FSectorClosed SectorClosed;

private:

	//If true, there are enemies in the sector.
	bool bEnemiesInSector = false;

	//Manage the time without enemies.
	float CurrentTimeWithoutEnemies = 0;

	//Manage the remaining time of the resting.
	float CurrentTimeToEndResting = 0;

	//Deactivate all the enemies and put them back in the pool
	void DeactivateAllEnemiesInMap() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, 
		const FHitResult& SweepResult
	);


	UFUNCTION()
		void OnEndOverlap(
			UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex
	);


	UFUNCTION()
		void OnBeginOverlapExit(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep,
			const FHitResult& SweepResult
		);

	//Manage how the sector closes
	void CloseSector();

	//Manage how the sector opens
	void OpenSector();

	//Refresh the enemies in the sector.
	void EnemyGotOut();

	//Search and stores all the enemies spawners which index is the same as the sector index.
	void GetEnemySpawnersAsociated();

	//Function to activate the asociated enemy Spawners.
	void ActivateEnemySpawnersAsociated();

	//Function to activate the asociated enemy Spawners.
	void DeactivateEnemySpawnersAsociated();

	float GetCurrentTimeEndResting();

	float GetTimeToConquer();

	UFUNCTION(BlueprintImplementableEvent)
	void ChangeMaterial(int32 MaterialIndex);


	//Call if the sector is conquered and it is the last sector. Determines how to end the level.
	UFUNCTION(BlueprintImplementableEvent)
	void OnLastSectorCompleted();


	//Call if the sector is conquered and it is the last sector. Determines how to end the level.
	UFUNCTION(BlueprintImplementableEvent)
	void UpWall();
};
