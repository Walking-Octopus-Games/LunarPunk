// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "Turrets/Projectiles/Projectile.h"
#include "BeaconAreaProjectile.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBeaconAreaProjectileDelegate);

UCLASS()
class LUNARPUNK_API ABeaconAreaProjectile : public AProjectile
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Speed = 14;


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float MaxTam = 40;

	UPROPERTY()
		FBeaconAreaProjectileDelegate FinishEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* ProjectileMesh;

	ABeaconAreaProjectile();

	virtual void BeginPlay();

	virtual void Tick(float DeltaTime);

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void ReturnToPool() override;

	virtual void Reactivate() override;
};
