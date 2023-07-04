// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AimAssist.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUNARPUNK_API UAimAssist : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAimAssist();

	//Quantity of assist. Multiplies the max radius and the Max Angle.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AssistAmount = 0.5f;

	//Radius of the trace to detect targets
	UPROPERTY(EditAnywhere)
	float MaxRadius = 250.0f;

	//Distance from the owner to apply the assist
	UPROPERTY(EditAnywhere)
	float MaxDistance = 3000.0f;

	//Angle from the owner's forward in wich the assit will be applied
	UPROPERTY(EditAnywhere)
	float MaxAngle = 40.0f;

	//Channel in which the aim assist will look for targets
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> TargetChannel = ECC_GameTraceChannel3;


	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebugSpheres = true;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawTargetSphere = true;

	UPROPERTY(EditAnywhere, Category = "Debug")
	float DrawDuration = 2.0f;



protected:
	// Called when the game starts
	virtual void BeginPlay() override;


private:

	UPROPERTY(VisibleAnywhere, Category = "Debug")
	float AngleTolerance = FMath::Cos(FMath::DegreesToRadians(AssistAmount*MaxAngle));

	TArray<const AActor*> ActorsHitted;

	TMap<const AActor*, float> ActorsHittedMap;

#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& _event) override;

#endif


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool AimAssist(const FVector& ShotStart, const FVector& DesiredDirection, FRotator& CurrentRotation);

		
};
