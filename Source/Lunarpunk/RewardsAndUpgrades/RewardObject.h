// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RewardObject.generated.h"

class URewardsComponent;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActorDropRewards);

UCLASS()
class LUNARPUNK_API ARewardObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARewardObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void HitReward();

	UFUNCTION()
		virtual void OnBeginOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
		);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UDropRewardsComponent* Rewards;

	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere)
		int Hits = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int TotalHits = 3;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable)
		FActorDropRewards ActorDropRewards;
};
