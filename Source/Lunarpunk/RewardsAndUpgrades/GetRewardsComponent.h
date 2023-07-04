// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GetRewardsComponent.generated.h"


//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlatePickedUp, TSubclassOf<AActor>, RewardClass);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNotEnoughPlates);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartDrawIn);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopDrawIn);

class ALunarPunkGameMode;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUNARPUNK_API UGetRewardsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGetRewardsComponent();

	//Fuerza de absorción
	UPROPERTY(EditAnywhere)
	float DrawInForce = 50.0f;

	//If true, the character ir drawing in
	UPROPERTY(EditAnywhere)
	bool bIsDrawingIn = false;

	//Distance to atract the rewards
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DrawInDistance = 6.0f;

	//Angle of the cone which does the draw in.
	UPROPERTY(EditAnywhere)
	float DrawInAngle = 90.0f;

	//If true, the draw in will be done all the time.
	UPROPERTY(EditAnywhere)
	bool bAutomaticDrawIn = true;

	UPROPERTY(EditAnywhere)
	bool bDrawCone = false;

	UStaticMeshComponent* DrawInCone;

	////Delegates to refresh the HUD information about the upgrades.
	//UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
	//FPlatePickedUp PlatePickedUp;

	//UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
	//FNotEnoughPlates NotEnoughPlates;


	//Delegates to notify the start/stop draw in
	/*FStartDrawIn StartDrawIn;
	FStopDrawIn StopDrawIn;*/


protected:
	// Called when the game starts
	virtual void BeginPlay() override;


private:

	//Function to draw the draw in cone (For debug).
	void DrawCone() const;

	ALunarPunkGameMode* GameMode;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Function to start draw in
	void StartDrawingIn();

	//Function to Stop Draw in
	void StopDrawingIn();
};
