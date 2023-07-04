// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/AbilitiesManger.h"

#include "EntityManager.h"
#include "GameFrameworks/LunarPunkGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LunarPunkPlayerController.h"
#include "Player/PlayableCharacter.h"

class APlayableCharacter;

UAbilitiesManger::UAbilitiesManger()
{
    CurrentAbility = EAbilityType::None;
}

void UAbilitiesManger::Initialize(ALunarPunkPlayerController* PlayerController)
{
  GameMode = Cast<ALunarPunkGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

  PlayerController->ThrowGrenadeAbility.AddDynamic(this, &UAbilitiesManger::SelectGrenade);
  PlayerController->ThrowShieldAbility.AddDynamic(this, &UAbilitiesManger::SelectShield);
  PlayerController->ThrowSparkleAbility.AddDynamic(this, &UAbilitiesManger::SelectSparkle);
  PlayerController->ThrowActivateTrapsAbility.AddDynamic(this, &UAbilitiesManger::SelectTraps);
}

void UAbilitiesManger::SelectGrenade()
{
    CurrentAbility = EAbilityType::Granade;
    SelectAbility.Broadcast(CurrentAbility);
}

void UAbilitiesManger::SelectShield()
{
    CurrentAbility = EAbilityType::Shield;
    SelectAbility.Broadcast(CurrentAbility);
}

void UAbilitiesManger::SelectSparkle()
{
    CurrentAbility = EAbilityType::Spark;
    SelectAbility.Broadcast(CurrentAbility);
}

void UAbilitiesManger::SelectTraps()
{
    CurrentAbility = EAbilityType::TrapActivator;
    SelectAbility.Broadcast(CurrentAbility);
}

EAbilityType UAbilitiesManger::GetCurrentAbility()
{
    return CurrentAbility;
}