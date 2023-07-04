// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/PlatesManager.h"
#include "Player/PlayableCharacter.h"
#include "RewardsAndUpgrades/GetRewardsComponent.h"
#include "RewardsAndUpgrades/Plate.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LunarPunkPlayerController.h"
#include "Managers/EntityManager.h"
#include "Turrets/Turret.h"

// Sets default values
APlatesManager::APlatesManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlatesManager::BeginPlay()
{
	Super::BeginPlay();

	CurrentPlayerPlates = InitialPlayerPlates;
	PlatesUpgrades.Broadcast(CurrentPlayerPlates);
	
}

// Called every frame
void APlatesManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlatesManager::Initialize(ALunarPunkPlayerController* PlayerController, UEntityManager* EntityManager)
{
	PlayerController->GetAllPlatesEvent.AddDynamic(this, &APlatesManager::GetAllPlates);

	if (EntityManager)
	{
		for (ATurret* Turret : EntityManager->Turrets)
		{
			PlatesUpgrades.AddDynamic(Turret, &ATurret::RefreshInfoWidget);
		}
	}
}

void APlatesManager::GetAllPlates()
{
	CurrentPlayerPlates = MaxPlayerPlates;
	PlatesUpgrades.Broadcast(CurrentPlayerPlates);
}

bool APlatesManager::CheckEnoughPlates(int32 PlatesToSpend) const
{
	if (CurrentPlayerPlates >= PlatesToSpend)
	{
		return true;
	}
	else
	{
		NotEnoughPlates.Broadcast();
		return false;
	}
}


bool APlatesManager::TrySpendPlates(int32 PlatesToSpend)
{
	if (CurrentPlayerPlates >= PlatesToSpend)
	{
		CurrentPlayerPlates -= PlatesToSpend;
		//PlatesUpgrades.Broadcast(PlatesToSpend);
		PlatesUpgrades.Broadcast(CurrentPlayerPlates);
		return true;
	}
	else
	{
		//Not enough plates.
		NotEnoughPlates.Broadcast();
		return false;
	}
}

void APlatesManager::AddPlates(int32 PlatesToAdd)
{
	int NewPlates = CurrentPlayerPlates + PlatesToAdd;
	if (NewPlates > MaxPlayerPlates) 
	{
		CurrentPlayerPlates = MaxPlayerPlates;
	}
	else 
	{
		CurrentPlayerPlates = NewPlates;
	}
	//PlatesUpgrades.Broadcast(PlatesToAdd);
	PlatesUpgrades.Broadcast(CurrentPlayerPlates);
}