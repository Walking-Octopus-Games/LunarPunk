// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "SparkProjectile.generated.h"

/**
 * 
 */
UCLASS()
class LUNARPUNK_API ASparkProjectile : public AProjectile
{
	GENERATED_BODY()



public:

	ASparkProjectile();


	//Delay between sparks bouncess.
	UPROPERTY(EditAnywhere, Category = "ASparks Projectile | Set")
	float DelaySparks = 0.1f;


	// Component of the niagara shoot effect
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASparks Projectile | Set")
	class UNiagaraComponent* ShootEffectComponent;


	//Init Num of bouncess that will take place.
	UPROPERTY(EditAnywhere, Category = "ASparks Projectile | Stats")
	int32 InitNumBouncess = 1;
	
	UPROPERTY(VisibleAnywhere, Category = "ASparks Projectile | Stats")
	//Num of bouncess that will take place.
	int32 NumBouncess = InitNumBouncess;

	//Init Enemies Affected in the first bounce.
	UPROPERTY(EditAnywhere, Category = "ASparks Projectile | Stats")
	int32 InitNumEnemiesAffectedFirst = 3;

	UPROPERTY(VisibleAnywhere, Category = "ASparks Projectile | Stats")
	//Enemies Affected in the first bounce.
	int32 NumEnemiesAffectedFirst = InitNumEnemiesAffectedFirst;

	//Init Number of enemies affected from the second bounce on.
	UPROPERTY(EditAnywhere, Category = "ASparks Projectile | Stats")
	int32 InitNumEnemiesAffectedInEachBounce = 1;

	UPROPERTY(VisibleAnywhere, Category = "ASparks Projectile | Stats")
	//Number of enemies affected from the second bounce on.
	int32 NumEnemiesAffectedInEachBounce = InitNumEnemiesAffectedInEachBounce;

	UPROPERTY(EditAnywhere, Category = "ASparks Projectile | Stats")
	//Init Range of the bounce.
	float InitRange = 8.0f;

	UPROPERTY(VisibleAnywhere, Category = "ASparks Projectile | Stats")
	//Range of the bounce.
	float Range = InitRange;

	//Initial Damage percent to make from the initial damage of the projectil.
	UPROPERTY(EditAnywhere, Category = "ASparks Projectile | Stats")
	float InitDamagePercentFromInit = 0.5f;

	//Damage percent to make from the initial damage of the projectil.
	UPROPERTY(VisibleAnywhere, Category = "ASparks Projectile | Stats")
	float DamagePercentFromInit = InitDamagePercentFromInit;

	//Initial configuration. If true, percent damage decrease will be applied recursively in each bounce.
	UPROPERTY(EditAnywhere, Category = "ASparks Projectile | Stats")
	bool bInitContinuousDamageDecrease = true;

	//If true, percent damage decrease will be applied recursively in each bounce.
	UPROPERTY(VisibleAnywhere, Category = "ASparks Projectile | Stats")
	bool bContinuousDamageDecrease = bInitContinuousDamageDecrease;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Sound")
	USoundBase* SparksSound;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAudioComponent* AudioComponent;
	

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, 
								AActor* OtherActor,
								UPrimitiveComponent* OtherComp, 
								int32 OtherBodyIndex, 
								bool bFromSweep, 
								const FHitResult& SweepResult
								) override;

	virtual void Reactivate() override;


	//Function to handle the bounce.
	void DoBounce();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:


	//Actors to ignore in the bouncess.
	TArray<AActor*> ActorsToIgnore;
	//Stores de next actors whose location will be the sparks origin.
	TArray<AActor*> ActorsToBeSparksOrigins;

	//Damage component to make the sparks damage (different from the projectil damage).
	class UDamageComponent* SparksDamageComponent;

	//Timer handle to manage the sparks delay.
	FTimerHandle TimerHandle;

	//Reference of the player
	AActor* PlayerReference;

	//Current bouncess made.
	int32 CurrentBouncess = 0;
	
	//Current actors affected by the sparks
	int32 CurrentNumOtherActorsAffected = 0;


	void ResetStats();

	void SpawnNiagaraEffect(FVector SparksOriginLocation, FVector SparksEndLocation);

};
