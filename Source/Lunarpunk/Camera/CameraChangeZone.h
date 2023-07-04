// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraChangeZone.generated.h"

class UBoxComponent;
class ACameraManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCameraChangeZoneActivated, ACameraChangeZone*, ChangeZone);

UCLASS()
class LUNARPUNK_API ACameraChangeZone : public AActor
{
	GENERATED_BODY()
	
public:	

	//Offset from the initial rotation of the camera.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Config.")
	FRotator OffsetRotation = FRotator(0, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Config.")
	float RotationSpeed = 0.5f;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TriggerCollision;

	FCameraChangeZoneActivated CameraChangeZoneActivated;



	// Sets default values for this actor's properties
	ACameraChangeZone();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	void StopRotation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:


	bool bIsRotating = false;

	ACameraManager* CameraManager;

	float Increment = 0.0f;


	UPROPERTY(VisibleAnywhere, category = "Config.")
	FRotator InitialRotation;

	UPROPERTY(VisibleAnywhere, category = "Config.")
	FRotator FinalRotation;

};
