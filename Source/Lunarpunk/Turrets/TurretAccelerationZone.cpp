// Fill out your copyright notice in the Description page of Project Settings.


#include "Turrets/TurretAccelerationZone.h"
#include "Components/BoxComponent.h"
#include "Turret.h"
#include "Turrets/AI/MovementTurretComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "Managers/EntityManager.h"

// Sets default values
ATurretAccelerationZone::ATurretAccelerationZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AccelerationZone = CreateDefaultSubobject<UBoxComponent>("AccelerationZone");

	AccelerationZone->OnComponentBeginOverlap.AddDynamic(this, &ATurretAccelerationZone::OnBeginOverlap);
	AccelerationZone->OnComponentEndOverlap.AddDynamic(this, &ATurretAccelerationZone::OnEndOverlap);

}

// Called when the game starts or when spawned
void ATurretAccelerationZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATurretAccelerationZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float Increment = TurretsAcceleration*DeltaTime;

	for (ATurret* Turret : OverlappedTurrets)
	{
		//Case: increment speed
		if (Turret->FloatingMovementComponent->MaxSpeed < NewTurretsSpeed)
		{
			Turret->FloatingMovementComponent->MaxSpeed += Increment;

			if (Turret->FloatingMovementComponent->MaxSpeed > NewTurretsSpeed)
			{
				Turret->FloatingMovementComponent->MaxSpeed = NewTurretsSpeed;
			}
		}

		//case: decrement speed
		if (Turret->FloatingMovementComponent->MaxSpeed > NewTurretsSpeed)
		{
			Turret->FloatingMovementComponent->MaxSpeed -= Increment;

			if (Turret->FloatingMovementComponent->MaxSpeed < NewTurretsSpeed)
			{
				Turret->FloatingMovementComponent->MaxSpeed = NewTurretsSpeed;
			}
		}

	}
}

void ATurretAccelerationZone::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ATurret* Turret = Cast<ATurret>(OtherActor))
	{
		if (bRestoreDefaultSpeed)
		{
			ALunarPunkGameMode* GameMode = Cast<ALunarPunkGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
			if (GameMode && GameMode->EntityManager)
			{
				NewTurretsSpeed = GameMode->EntityManager->GetTurretsDefaultSpeed();
			}
		}

		//Check if the turret is enter in the collision using the "entrance" side of the collider.
		float Direction = FVector::DotProduct(GetActorForwardVector(), (Turret->GetActorLocation()- GetActorLocation()).GetSafeNormal());
		if (!OverlappedTurrets.Contains(Turret) && Direction<0)
		{
			OverlappedTurrets.Add(Turret);

			if(OverlappedTurrets.Num() == 1)
			{
				SetActorTickEnabled(true);
			}
		}
	}
}


void ATurretAccelerationZone::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ATurret* Turret = Cast<ATurret>(OtherActor))
	{
		if (OverlappedTurrets.Contains(Turret))
		{
			OverlappedTurrets.Remove(Turret);

			if (OverlappedTurrets.Num() == 0)
			{
				SetActorTickEnabled(false);
			}

			//If the turret finish the overlap using the entrance side of the collision, then restores its speed.
			float Direction = FVector::DotProduct(GetActorForwardVector(), (Turret->GetActorLocation() - GetActorLocation()).GetSafeNormal());
			if (Direction < 0)
			{
				ALunarPunkGameMode* GameMode = Cast<ALunarPunkGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
				if (GameMode && GameMode->EntityManager)
				{
					Turret->FloatingMovementComponent->MaxSpeed = GameMode->EntityManager->GetTurretsDefaultSpeed();
				}
				
			}
		}

	}
}
