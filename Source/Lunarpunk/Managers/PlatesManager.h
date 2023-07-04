// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatesManager.generated.h"

class ALunarPunkPlayerController;
class UEntityManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlatesUpgrade, int32, NumPlatesUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNotEnoughPlates2);


UCLASS()
class LUNARPUNK_API APlatesManager : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(EditAnywhere)
	TSubclassOf<class APlate> PlatesClass;

	//Initial plates of the player in the level.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InitialPlayerPlates = 0;

	//Max plates of the player in the level.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxPlayerPlates = 100;

	//Max plates of the player in the level.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRewardsCanDisapeared = true;


	UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
	FPlatesUpgrade PlatesUpgrades;

	UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
	FNotEnoughPlates2 NotEnoughPlates;

	// Sets default values for this actor's properties
	APlatesManager();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void Initialize(ALunarPunkPlayerController* PlayerController, UEntityManager* EntityManager);

	UFUNCTION(BlueprintCallable)
	void GetAllPlates();


	UFUNCTION()
	bool CheckEnoughPlates(int32 PlatesToSpend) const;

	UFUNCTION()
	bool TrySpendPlates(int32 PlatesToSpend);

	UFUNCTION()
	void AddPlates(int32 PlatesToAdd);

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentPlates() const { return CurrentPlayerPlates; }

	UFUNCTION(BlueprintCallable)
	int32 GetMaxPlates() const { return MaxPlayerPlates; }

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



private:

	//Number the current plates of the player.
	UPROPERTY(VisibleAnywhere)
	int32 CurrentPlayerPlates = 0;


};
