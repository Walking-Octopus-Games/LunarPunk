// Fill out your copyright notice in the Description page of Project Settings.


#include "RewardsAndUpgrades/SparkUpgradeComponent.h"
#include "Turrets/ProjectileTurret.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"

// Sets default values for this component's properties
USparkUpgradeComponent::USparkUpgradeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USparkUpgradeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (bSparksActive)
	{
		ActivateUpgrade();
	}
	
}


// Called every frame
void USparkUpgradeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



void USparkUpgradeComponent::ActivateUpgrade()
{
	//Add spark VFX
	
	//Changes the turret projecil
	Cast<AProjectileTurret>(GetOwner())->SetProjectile(SparkProjectileClass);
	bSparksActive = true;
	UNiagaraComponent* SparksNiagaraComponent = Cast<UNiagaraComponent>(GetOwner()->GetDefaultSubobjectByName(TEXT("NiagaraSparks")));
	if (IsValid(SparksNiagaraComponent))
	{
		SparksNiagaraComponent->ActivateSystem();
	}
		
	SparksNiagaraComponent = Cast<UNiagaraComponent>(GetOwner()->GetDefaultSubobjectByName(TEXT("NiagaraSparksRing")));

	if (IsValid(SparksNiagaraComponent))
	{
		SparksNiagaraComponent->ActivateSystem();
	}
}


void  USparkUpgradeComponent::ImproveUpgrade()
{
	//Modify Stats.
	++ExtraBouncess;
}



