// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Abilities/TrapActivatorAbilityComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/AbilitiesManger.h"
#include "Managers/EntityManager.h"
#include "Managers/PlatesManager.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "Utilities/UtilitiesLunarpunk.h"
#include "Traps/Trap.h"
#include "Components/DecalComponent.h"

UTrapActivatorAbilityComponent::UTrapActivatorAbilityComponent()
{
	AbilityType = EAbilityType::TrapActivator;
	//Create audio component.
}


void UTrapActivatorAbilityComponent::Initialize(ALunarPunkPlayerController* _PlayableController, APlayableCharacter* _PlayableCharacter)
{
	Super::Initialize(_PlayableController, _PlayableCharacter);
}

void UTrapActivatorAbilityComponent::ThrowAbility()
{
	if (GetOwner())
	{
		UUtilitiesLunarpunk::ObtainActorsInRange<ATrap>(
			GetWorld(),
			GetOwner()->GetActorLocation(),
			0.01 * TraceRange,
			TrapsToActivate,
			{ UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel3) },
			bDrawDebugSphere,
			DrawTime
			);

		for (ATrap* Trap : TrapsToActivate)
		{
			Trap->TurnOnOutline();
		}
	}

	if (AssertThrowAbility())
	{
		ThrowTrapsEvent.Broadcast();
		ActivateTraps();
		Super::ThrowAbility();
	}
}

void UTrapActivatorAbilityComponent::StopAbility()
{
	Super::StopAbility();
	if (TrapsToActivate.Num() > 0)
	{
		for (ATrap* Trap : TrapsToActivate)
		{
			Trap->TurnOffOutline();
		}
	}
}

void UTrapActivatorAbilityComponent::Button2Pressed(bool bIsPressed)
{
	Super::Button2Pressed(bIsPressed);
}


void UTrapActivatorAbilityComponent::ActivateTraps()
{

	if(TrapsToActivate.Num()>0)
	{
		for (ATrap* Trap : TrapsToActivate)
		{
			Trap->LaunchTrap();
			Trap->TurnOffOutline();
		}
	}
	
}
