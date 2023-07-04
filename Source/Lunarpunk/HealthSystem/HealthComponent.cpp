// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include <Kismet/GameplayStatics.h>
#include "GameFramework/Character.h"
#include "Managers/HUDManager.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;


}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
  Super::BeginPlay();

  // ...
  ResetHealth();

}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // ...
  if (bIsRestoringHealth)
  {
    ActualHealth += RestoringHealthSpeed * DeltaTime;
    UpdateHealth.Broadcast(ActualHealth);
    if (ActualHealth >= MaxHealth)
    {
      ActualHealth = MaxHealth;
      bIsRestoringHealth = false;
    }
  }
}

float UHealthComponent::GetHealth()
{
  return ActualHealth;
}

void UHealthComponent::SetHealth(float newHealth)
{
  ActualHealth = newHealth;
  UpdateHealth.Broadcast(ActualHealth);
}

float UHealthComponent::GetMaxHealth()
{
  return MaxHealth;
}

void UHealthComponent::SetMaxHealth(float NewMaxHealth, bool ShouldResetHealth)
{
  MaxHealth = NewMaxHealth;
  if (ShouldResetHealth)
  {
      ResetHealth();
  }
  
}


void UHealthComponent::IncreaseMaxHealthWithPercentage(float Percentage)
{
  float IncreaseAmount = MaxHealth * Percentage;
  MaxHealth += IncreaseAmount;
  RegainHealth(IncreaseAmount);
}

bool UHealthComponent::CanBeDamaged()
{
  return bCanBeDamaged;
}

void UHealthComponent::SetCanBeDamaged(bool bImmunity)
{
  bCanBeDamaged = bImmunity;
}


void UHealthComponent::SetGodModeEnabled(bool GodMode)
{
  bGodMode = GodMode;
}

void UHealthComponent::ResetHealth()
{
  ActualHealth = MaxHealth;
  SetCanBeDamaged(true);
  UpdateHealth.Broadcast(ActualHealth);
}

void UHealthComponent::ReceiveDamage(float Damage)
{
  if (!bCanBeDamaged || bGodMode)
  {
    return;
  }
  ActualHealth = ActualHealth - Damage;

  if (ActualHealth <= 0)
  {
    SetCanBeDamaged(false);
    ActualHealth = 0;
    ActorDeath.Broadcast();
  }

  UpdateHealth.Broadcast(ActualHealth);
  DamageReceived.Broadcast(Damage);
}


void UHealthComponent::ReceivePercentageDamage(float Percentage)
{
  ReceiveDamage(MaxHealth * Percentage);
}


void UHealthComponent::InstantKill()
{
  ReceiveDamage(ActualHealth);
}

void UHealthComponent::RegainHealth(float regainHealth)
{
  ActualHealth += regainHealth;

  if (ActualHealth > MaxHealth)
  {
    ActualHealth = MaxHealth;
  }

  UpdateHealth.Broadcast(ActualHealth);
}


void UHealthComponent::ResetComponent()
{
  MaxHealth = BaseMaxHealth;
  ResetHealth();
}