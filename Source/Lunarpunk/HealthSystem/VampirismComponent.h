// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VampirismComponent.generated.h"

class UHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGetLife);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUNARPUNK_API UVampirismComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UVampirismComponent();

	/*Attach here the health component to restore the life of it when vampirism hits*/
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UHealthComponent* HealthComponentToRestore;

	/*Vampirism %. Percentage of the Life that will be recovered when damagin. */
	UPROPERTY(EditAnywhere, Category = "Basic Atrributes", meta = (UIMin = "0.0", UIMax = "1.0"))
		float PercentageOfLifeRecoveredWhenKillingEnemy = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
	FGetLife GetLife;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void HitVampirism();


};
