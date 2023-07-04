// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelLayout/DeathZone.h"
#include "HealthSystem/HealthComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ADeathZone::ADeathZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DeathZoneCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneCollision"));
	DeathZoneCollision->OnComponentBeginOverlap.AddDynamic(this, &ADeathZone::OnBeginOverlap);

}

// Called when the game starts or when spawned
void ADeathZone::BeginPlay()
{
	Super::BeginPlay();
	
}



void ADeathZone::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (UHealthComponent* HealthComponent = OtherActor->FindComponentByClass<UHealthComponent>())
	{
		HealthComponent->InstantKill();
	}

}