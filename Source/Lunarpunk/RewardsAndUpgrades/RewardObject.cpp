// Fill out your copyright notice in the Description page of Project Settings.


#include "RewardsAndUpgrades/RewardObject.h"
#include "DropRewardsComponent.h"
#include "Components/BoxComponent.h"
#include "Turrets/Turret.h"
#include "Turrets/HealingTurret.h"
#include "Turrets/Projectiles/HealingProjectile.h"
#include "Enemies/BasicEnemy.h"

// Sets default values
ARewardObject::ARewardObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	Rewards = CreateDefaultSubobject<UDropRewardsComponent>(TEXT("Rewards"));
	AddOwnedComponent(Rewards);
}

// Called when the game starts or when spawned
void ARewardObject::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision = FindComponentByClass<UBoxComponent>();

	if (BoxCollision)
	{
		BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ARewardObject::OnBeginOverlap);
	}
}

// Called every frame
void ARewardObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARewardObject::HitReward()
{
	Hits++;

	if(Hits >= TotalHits)
	{
		ActorDropRewards.Broadcast();
	}
}

void ARewardObject::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor->IsA<ATurret>())// || OtherActor->IsA<ABasicEnemy>())
	{
		ActorDropRewards.Broadcast();
	}

}