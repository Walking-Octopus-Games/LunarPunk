// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Turrets/Turret.h"

#include "AoETurret.generated.h"

class UDamageComponent;
class UVampirismComponent;
class UBoxComponent;
class URotatingMovementComponent;
class UStaticMeshComponent;

/**
 * 
 */
UCLASS()
class LUNARPUNK_API AAoETurret : public ATurret
{
	GENERATED_BODY()

public:
	AAoETurret();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Turret|Damage")
		UDamageComponent* DamageComponent = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Turret|Damage")
		UVampirismComponent* VampirismComponent = nullptr;

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
	UBoxComponent* AoECollision = nullptr;

	URotatingMovementComponent* RotatingMovement = nullptr;

	UStaticMeshComponent* FireComponent = nullptr;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent
		, AActor* OtherActor
		, UPrimitiveComponent* OtherComp
		, int32 OtherBodyIndex
		, bool bFromSweep
		, const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent
		, AActor* OtherActor
		, UPrimitiveComponent* OtherComp
		, int32 OtherBodyIndex
	);
	
	virtual void StartTurret() override;

	virtual void StopTurret() override;

};
