// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraChangeZone.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CameraManager.h"
#include "Player/PlayableCharacter.h"

// Sets default values
ACameraChangeZone::ACameraChangeZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerCollision = CreateDefaultSubobject<UBoxComponent>("TiggerCollisionComponent");
	RootComponent = TriggerCollision;
	TriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &ACameraChangeZone::OnBeginOverlap);

}

// Called when the game starts or when spawned
void ACameraChangeZone::BeginPlay()
{
	Super::BeginPlay();
	
	CameraManager = CameraManager = Cast<ACameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));


}

// Called every frame
void ACameraChangeZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRotating)
	{
		Increment += DeltaTime * RotationSpeed;
		CameraManager->SetCameraRotation(FMath::Lerp(InitialRotation, FinalRotation, Increment));

		if (Increment >= 1)
		{
			//Set the exact final rotation to avoid acumulative errors in camera rotation.
			//Also set the final rotation as the new resting rotation of the camera
			CameraManager->SetCameraRotation(FinalRotation, true);
			bIsRotating = false;
		}
	}

}



void ACameraChangeZone::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor->IsA<APlayableCharacter>())
	{
		if (!bIsRotating)
		{
			if (CameraManager)
			{
				//Set the initial and final rotation.
				InitialRotation = CameraManager->Rotation;
				FinalRotation = CameraManager->InitialRotation + OffsetRotation;

				//Start to rotate only if the initial and final rotation are differents
				if (InitialRotation != FinalRotation)
				{
					bIsRotating = true;
					Increment = 0;

					//Anounces the rotation start.
					CameraChangeZoneActivated.Broadcast(this);
				}
			}
		}

	}

}


void ACameraChangeZone::StopRotation() 
{
	bIsRotating = false;
}