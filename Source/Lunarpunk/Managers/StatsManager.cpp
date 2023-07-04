// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/StatsManager.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "Managers/EntityManager.h"
#include "Managers/PlatesManager.h"
#include "Player/LunarPunkPlayerController.h"
#include "Turrets/Turret.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HealthSystem/HealthComponent.h"
#include "HealthSystem/DamageComponent.h"
#include "Enemies/BasicEnemy.h"
#include "Traps/Trap.h"
#include "Player/Abilities/ShootAbilityComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Abilities/AbilityBaseComponent.h"
#include "Turrets/TurretsData.h"


UStatsManager::UStatsManager()
{

}

void UStatsManager::Initialize(ALunarPunkGameMode* GM, ALunarPunkPlayerController* PC)
{
  //Initialize Gamemode & PlayerController
  this->GameMode = GM;
  this->PlayerController = PC;

  //Initialize Stats float
  this->GameTime = 0;
  this->PlatesEarned = 0;
  this->TotalDamagePlayer = 0;
  this->TotalShootsTurrets = 0;
  this->TotalShootsPlayer = 0;
  this->EnemiesDefeated = 0;
  this->TotalTrapsActivated = 0;

 
  //Event Subcription
  
  //Player Events
  GameMode->EntityManager->PlayerCharacter->ShootAbilityComponent->PlayerShooting.AddDynamic(this, &UStatsManager::IncrementTotalShootsPlayer);
  GameMode->EntityManager->PlayerCharacter->ShootAbilityComponent->PlayerMakeDamage.AddDynamic(this, &UStatsManager::IncrementTotalDamagePlayer);

  // Turret Shoots, Upgrade applied, damage per turret
  for (ATurret * turret: GameMode->EntityManager->Turrets)
  {
    turret->ShootEvent.AddDynamic(this, &UStatsManager::IncrementTotalShootsTurrets);
    turret->UpgradeAdded.AddDynamic(this, &UStatsManager::IncrementTotalUpgradesUsed);
    turret->TurretMakeDamage.AddDynamic(this, &UStatsManager::IncrementTotalDamageTurrets);
  }

  //Plates Earned
  GameMode->PlatesManager->PlatesUpgrades.AddDynamic(this, &UStatsManager::IncrementPlatesEarned);

  //Enemy Defeated
  for (ABasicEnemy* enemy : GameMode->EntityManager->Enemies)
  {
    enemy->GetHealthComponent()->ActorDeath.AddDynamic(this, &UStatsManager::IncrementEnemiesDefeated);
  }

  //Traps Activated
  TArray<AActor*> TrapsActors;
  UGameplayStatics::GetAllActorsOfClass(GameMode->GetWorld(), ATrap::StaticClass(), TrapsActors);
  for (AActor * trap : TrapsActors)
  {
    Cast<ATrap>(trap)->TrapLunched.AddDynamic(this,&UStatsManager::IncrementTotalTrapsActivated);
  }
  
  //Abilities Lunched

  for (auto ability : GameMode->EntityManager->PlayerCharacter->GetComponentsByClass(UAbilityBaseComponent::StaticClass()))
  {
    Cast<UAbilityBaseComponent>(ability)->AbilityThrowed.AddDynamic(this, &UStatsManager::IncrementTotalAbilitiesUsed);
  }
}

void UStatsManager::IncrementTotalShootsPlayer()
{
  TotalShootsPlayer++;
}

void UStatsManager::IncrementTotalDamagePlayer(float quantity)
{
  TotalDamagePlayer += quantity;
}

void UStatsManager::IncrementTotalShootsTurrets()
{
  TotalShootsTurrets++;
}

void UStatsManager::IncrementPlatesEarned(int32 NumPlatesUpdate)
{
  PlatesEarned += NumPlatesUpdate;
  
}

void UStatsManager::SetGameTime()
{
  GameTime += UKismetSystemLibrary::GetGameTimeInSeconds(GameMode->GetWorld());

}

void UStatsManager::IncrementEnemiesDefeated()
{
  EnemiesDefeated++;

}

void UStatsManager::IncrementTotalTrapsActivated()
{
  TotalTrapsActivated++;

}

void UStatsManager::IncrementTotalUpgradesUsed(EUpgradeType type)
{
  if (TotalUpgradesUsed.Find(type) == nullptr)
  {
    TotalUpgradesUsed.Add(type, 1);
  }
  else
  {
    *TotalUpgradesUsed.Find(type) += 1;
  }
}

void UStatsManager::IncrementTotalAbilitiesUsed(EAbilityType type)
{

  if (TotalAbilitiesUsed.Find(type) == nullptr)
  {
    TotalAbilitiesUsed.Add(type, 1);
  }
  else
  {
    *TotalAbilitiesUsed.Find(type) +=1;
  }
}


void UStatsManager::IncrementTotalDamageTurrets(ETurretType TurretType, float quantity)
{
  //TotalDamagePerTurret.FindOrAdd(TurretType, quantity);
  if (TotalDamagePerTurret.Find(TurretType) == nullptr)
  {
    TotalDamagePerTurret.Add(TurretType, quantity);
  }
  else
  {
    *TotalDamagePerTurret.Find(TurretType) += quantity;
  }
  
}

