// Fill out your copyright notice in the Description page of Project Settings.


#include "Traps/Trap.h"
#include "Components/BoxComponent.h"
#include "Turrets/Projectiles/Projectile.h"


ATrap::ATrap()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//BoxCollision = CreateDefaultSubobject<UBoxComponent>(FName("TrapCollision"));
}

// Called when the game starts or when spawned
void ATrap::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision = Cast<UBoxComponent>(GetDefaultSubobjectByName("TriggerTrapCollision"));

	if (BoxCollision)
	{
		BoxCollision->OnComponentHit.AddDynamic(this, &ATrap::OnHit);
	}

	RemainingLives = Lives;

	ReactivateTrap();
}

// Called every frame
void ATrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATrap::HitTrap()
{
	++CurrentHits;
	if (CurrentHits == HitsToLaunch)
	{
		LaunchTrap();
	}
}

void ATrap::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->IsA<AProjectile>())
	{
		HitTrap();
	}
}


void ATrap::LaunchTrap()
{
	DoTrapEffect();
	DeactivateTrap();
	--Lives;
	TrapLunched.Broadcast();

	if (Lives > 0)
	{
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ATrap::ReactivateTrap, TimeToReactivate, false);
	}
}

void ATrap::DeactivateTrap()
{
	if (BoxCollision)
	{
		BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ATrap::ReactivateTrap()
{
	if (BoxCollision)
	{
		BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	CurrentHits = 0;
}

void ATrap::DestroyTrap()
{
	Destroy();
}


void ATrap::TurnOnOutline()
{
	TArray<UStaticMeshComponent*> Components;
	GetComponents<UStaticMeshComponent>(Components);
	for (int32 i = 0; i < Components.Num(); i++)
	{
		Components[i]->SetCustomDepthStencilValue(STENCIL_TRAP_WITH_OUTLINE);
	}
}

void  ATrap::TurnOffOutline()
{
	TArray<UStaticMeshComponent*> Components;
	GetComponents<UStaticMeshComponent>(Components);
	for (int32 i = 0; i < Components.Num(); i++)
	{
		Components[i]->SetCustomDepthStencilValue(STENCIL_TRAP);
	}
}