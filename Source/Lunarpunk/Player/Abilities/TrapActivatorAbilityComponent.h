// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Abilities/AbilityBaseComponent.h"
#include "TrapActivatorAbilityComponent.generated.h"


class ATrap;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FThrowTraps);
/**
 * 
 */
UCLASS()
class LUNARPUNK_API UTrapActivatorAbilityComponent : public UAbilityBaseComponent
{
	GENERATED_BODY()
	

public:

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebugSphere = false;

	UPROPERTY(EditAnywhere, Category = "Debug")
	float DrawTime = 2.0f;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable)
		FThrowTraps ThrowTrapsEvent;

	UTrapActivatorAbilityComponent();

	virtual void Initialize(ALunarPunkPlayerController* _PlayableController, APlayableCharacter* _PlayableCharacter) override;

	virtual void ThrowAbility() override;

	virtual void StopAbility() override;

	virtual void Button2Pressed(bool bIsPressed) override;

	void ActivateTraps();

private:

	TArray<ATrap*> TrapsToActivate;
};
