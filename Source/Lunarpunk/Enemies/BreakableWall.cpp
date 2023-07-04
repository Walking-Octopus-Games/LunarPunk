// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/BreakableWall.h"

// Sets default values
ABreakableWall::ABreakableWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));


	

	TriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Component"));
	TriggerComponent->SetupAttachment(RootComponent);

	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("Destrcutible Component"));
	DestructibleComponent->SetupAttachment(TriggerComponent);
	//DestructibleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DestructibleComponent->SetSimulatePhysics(false);
	
	
	//DestructibleComponent->SetNotifyRigidBodyCollision(true);
	
	MaxHealth = 1.f;

	DefaultDamage = 1.f;
	DefaultImpulse = 1.f;

}



// Called when the game starts or when spawned
void ABreakableWall::BeginPlay()
{
	Super::BeginPlay();

	DestructibleComponent->SetCollisionResponseToChannel(ECC_Destructible, ECR_Ignore);

	//DestructibleComponent->OnComponentHit.AddDynamic(this, &ABreakableWall::Damage);

	//TriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ABreakableWall::Trigger);

	CurrentHealth = MaxHealth;

	if (bCanBeBroken)
	{
		DestructibleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DestructibleComponent->SetNotifyRigidBodyCollision(true);
		EnableBreakWall();
	}
	else
	{
		DestructibleComponent->Deactivate();
	}
	
}

// Called every frame
void ABreakableWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}




void ABreakableWall::HitWall()
{
	Hits++;

	if (Hits >= TotalHits)
	{
		//Destroy();
	}
}


void ABreakableWall::CanBeBreak()
{
	DestructibleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DestructibleComponent->SetNotifyRigidBodyCollision(true);
	EnableBreakWall();
}

void ABreakableWall::BreackWall()
{
	DeactivateEffect();
}


