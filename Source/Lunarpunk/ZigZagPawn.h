// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "AIController.h"
#include "Player/LunarPunkPlayerController.h"
#include "Components/SphereComponent.h"

#include "ZigZagPawn.generated.h"

UCLASS()
class LUNARPUNK_API AZigZagPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AZigZagPawn();

	UPROPERTY(VisibleAnywhere, Category = "Grenade")
		USphereComponent* CentralSphere;

	UPROPERTY(VisibleAnywhere, Category = "Grenade")
		USphereComponent* LateralSphere;

	//Indicates how big the zig zag will be
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, category = "Turret|Distance")
		float Distance = 500.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, category = "Turret|Health")
		float Speed = 1000.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector Direction;
	float TotalDistance;
	float CurrentDistance;
	FVector StartLocation;

	FRotator RotationPAwn;



	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, category = "Turret|Health")
		float CurrentTime = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, category = "Turret|Health")
		float AdvanceSpeed = 400.0f;

public:	
	UPROPERTY(VisibleAnywhere, Category = "Grenade", BlueprintReadOnly)
		UStaticMeshComponent* MeshComponent1;

	UPROPERTY(VisibleAnywhere, Category = "Grenade", BlueprintReadOnly)
		UStaticMeshComponent* MeshComponent2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UFloatingPawnMovement* FloatingMovementComponent;


	UPROPERTY()
		FVector DestinationPawn;

	UPROPERTY()
		APawn* ZigZagOwner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		AAIController* AIController; 

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Cuando el enemigo llega a la efera objetivo esta cambia de posici�n
	void TimeToChangePosition();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
