// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeathZone.generated.h"

class UBoxComponent;

UCLASS()
class LUNARPUNK_API ADeathZone : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(EditAnywhere)
	UBoxComponent* DeathZoneCollision;

	// Sets default values for this actor's properties
	ADeathZone();


	UFUNCTION()
		virtual void OnBeginOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
		);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
