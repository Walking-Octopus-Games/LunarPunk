// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Abilities/AbilityBaseComponent.h"
#include "SparkAbilityComponent.generated.h"


class UNiagaraSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FThrowSparkle);

UENUM()
enum class ESparksDamageMode : uint8
{
	Value, //The sparks will make a constant value damage
	Percentage, //The sparks will make a percentage of the enemy life damage.
};

/**
 * 
 */
UCLASS()
class LUNARPUNK_API USparkAbilityComponent : public UAbilityBaseComponent
{
    GENERATED_BODY()

public: 

    USparkAbilityComponent();

	//DamageMode
	UPROPERTY(EditAnywhere, Category = "Config.")
	ESparksDamageMode DamageMode = ESparksDamageMode::Percentage;

	//Damage of sparks bouncess.
	UPROPERTY(EditAnywhere, Category = "Config.", meta = (EditCondition = "DamageMode != ESparksDamageMode::Percentage", EditConditionHides))
		float DamageSparks = 50.0f;

	//Percentage of the enemy life damage of sparks bouncess.
	UPROPERTY(EditAnywhere, Category = "Config.", meta = (EditCondition = "DamageMode != ESparksDamageMode::Value", EditConditionHides))
		float PercentageDamageSparks = 0.5f;

	//Delay between sparks bouncess.
	UPROPERTY(EditAnywhere, Category = "Config.")
		float DelaySparks = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Config.")
		//Num of bouncess that will take place.
		int32 NumBouncess = 3;

	UPROPERTY(EditAnywhere, Category = "Config.")
		//Enemies Affected when throw the ability.
		int32 NumEnemiesAffectedFirst = 3;

	UPROPERTY(EditAnywhere, Category = "Config.")
		//Number of enemies affected from the second bounce on.
		int32 NumEnemiesAffectedInEachBounce = 1;

	//If true, a percentage decrease will be applied recursively to the damage in each bounce.
	UPROPERTY(EditAnywhere, Category = "Config.")
		bool bContinuousDamageDecrease = true;

	//Percentage decrement of the damage made in the subsequent bounces.
	UPROPERTY(EditAnywhere, Category = "Config.")
		float DamagePercentFromInit = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config.")
		UNiagaraSystem* SparkEffect;

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config.")
		UNiagaraSystem* LaunchSparkEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config.")
		float RadiusLaunchEffect = 1.0f;*/

	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Sound")
		USoundBase* SparksSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAudioComponent* AudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable)
		FThrowSparkle ThrowSparkleEvent;

    virtual void Initialize(ALunarPunkPlayerController* _PlayableController, APlayableCharacter* _PlayableCharacter) override;

    virtual void ThrowAbility() override;
  
    virtual void StopAbility() override;

	virtual void Button2Pressed(bool bIsPressed) override;


	//Function to handle the bounce.
	void DoBounce();

protected:

	virtual void BeginPlay() override;
private:

	//Actors to ignore in the bouncess.
	TArray<AActor*> ActorsToIgnore;

	//Stores de next actors whose location will be the sparks origin.
	TArray<AActor*> ActorsToBeSparksOrigins;

	//Timer handle to manage the sparks delay.
	FTimerHandle TimerHandle;

	//Reference of the player
	AActor* PlayerReference;

	//Current bouncess made.
	int32 CurrentBouncess = 0;

	float InitDamageSparks = DamageSparks;

	float InitPercentageDamageSparks = PercentageDamageSparks;

	//Current actors affected by the sparks
	int32 CurrentNumOtherActorsAffected = 0;

	UNiagaraComponent* SparksEffectComponent;

	void SpawnNiagaraEffect(FVector SparksOriginLocation, FVector SparksEndLocation);

	void SpawnLaunchSparkNiagaraEffect();

    UFUNCTION()
        void ThrowSparks();

};
