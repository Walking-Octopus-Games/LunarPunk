// Fill out your copyright notice in the Description page of Project Settings.


#include "VampirismComponent.h"
#include "HealthComponent.h"

// Sets default values for this component's properties
UVampirismComponent::UVampirismComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVampirismComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	if (!IsValid(HealthComponentToRestore) && PercentageOfLifeRecoveredWhenKillingEnemy > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("0 life will be recovered. The component must have a health component to have Vampirism"));
	}

}

// Called every frame
void UVampirismComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UVampirismComponent::HitVampirism()
{
	if (PercentageOfLifeRecoveredWhenKillingEnemy > 0 && IsValid(HealthComponentToRestore) /*&& HealthComponent->GetHealth() <= 0*/)
	{
		HealthComponentToRestore->RegainHealth(PercentageOfLifeRecoveredWhenKillingEnemy * HealthComponentToRestore->GetMaxHealth());
		GetLife.Broadcast();
	}
}

