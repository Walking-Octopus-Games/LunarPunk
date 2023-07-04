// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurretAccelerationZone.generated.h"

class UBoxComponent;
class ATurret;

UCLASS()
class LUNARPUNK_API ATurretAccelerationZone : public AActor
{
	GENERATED_BODY()
	
public:	

	//If true, the acceleration zone will restore the turrets original speed.
	UPROPERTY(EditAnywhere)
	bool bRestoreDefaultSpeed = false;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bRestoreDefaultSpeed", EditConditionHides))
	float NewTurretsSpeed = 200.0f;

	UPROPERTY(EditAnywhere)
	float TurretsAcceleration = 50.0f;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* AccelerationZone;

	// Sets default values for this actor's properties
	ATurretAccelerationZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
    virtual void OnBeginOverlap(
      UPrimitiveComponent* OverlappedComponent,
      AActor* OtherActor,
      UPrimitiveComponent* OtherComp,
      int32 OtherBodyIndex,
      bool bFromSweep,
      const FHitResult& SweepResult
    );

	UFUNCTION()
	virtual void OnEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

private:

	TArray<ATurret*> OverlappedTurrets;

};
