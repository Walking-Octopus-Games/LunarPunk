// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SparkUpgradeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUNARPUNK_API USparkUpgradeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USparkUpgradeComponent();

	//Sparks projectile class
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> SparkProjectileClass;

	//If true, the sparks are active.
	UPROPERTY(EditAnywhere)
	bool bSparksActive = false;

	UPROPERTY(EditAnywhere)
	int32 ExtraBouncess = 0;
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	//Activate the sparks
	UFUNCTION(BlueprintCallable)
	void ActivateUpgrade();

	//Improve the sparks force and the bouncess.
	void ImproveUpgrade();

		
};
