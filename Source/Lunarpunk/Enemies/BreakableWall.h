// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DestructibleComponent.h"
#include "Components/BoxComponent.h"
#include "BreakableWall.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWallBroken);

UCLASS()
class LUNARPUNK_API ABreakableWall : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ABreakableWall();

	


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Destructible)
		class UBoxComponent* TriggerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DestructibleComponent)
		bool bEnableHardSleeping = true;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Destructible)
		class UDestructibleComponent* DestructibleComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Destructible)
		bool IsTriggerEnabled;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Destructible)
		bool IsDestroyed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Destructible)
		float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Destructible)
		float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Destructible)
		float DefaultDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Destructible)
		float DefaultImpulse;










	/// <summary>
	/// //////////////////////
	/// </summary>

	//7UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//	UDestructibleComponent* DestructibleComponent;

	UPROPERTY(VisibleAnywhere)
		int Hits = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int TotalHits = 3;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(BlueprintAssignable)
	FWallBroken WallBroken;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeBroken = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isTutorialWall = false;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void HitWall();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void EnableBreakWall();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void DeactivateEffect();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ABreakableWall* WallToActive;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void EnableTutorialWall();

	UFUNCTION(BlueprintCallable, Category = "BreakSecondWall")
	void CanBeBreak();

	void BreackWall();

};
