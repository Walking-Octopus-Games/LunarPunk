// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Traps/Trap.h"
#include "ExplosivePipe.generated.h"

class ABasicEnemy;
class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class LUNARPUNK_API AExplosivePipe : public ATrap
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "Config")
	float Range = 16.0f;

	//Angle in wich the explosion will have effect. The half of the angle to the right and the other half to the left of the actor forward vector.
	UPROPERTY(EditAnywhere, Category = "Config")
	float Angle = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Config")
	TMap<TSubclassOf<ABasicEnemy>, float> DamagePercentageEachClass;

	UPROPERTY(EditAnywhere, Category = "Config")
	UMaterial* ActivatedLightMaterial;

	UPROPERTY(EditAnywhere, Category = "Config")
	UMaterial* DeactivatedLightMaterial;

	UPROPERTY(EditAnywhere, Category = "Config|VFX")
	UNiagaraSystem* ExplosionEffect;

	UPROPERTY(EditAnywhere, Category = "Config|VFX")
	UNiagaraSystem* ImpactEffect;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebugCone = true;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebugSphere = false;

	UPROPERTY(EditAnywhere, Category = "Debug")
	float DrawTime = 5.0f;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void DoTrapEffect() override;

	virtual void DeactivateTrap() override;

	virtual void ReactivateTrap() override;

private:


	UStaticMeshComponent* Light;

	void TurnOnDeactivatedLight();

	void TurnOnActivatedLight();

	
	

};
