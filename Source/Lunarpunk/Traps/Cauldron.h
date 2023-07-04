// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Traps/Trap.h"
#include "Delegates/Delegate.h"
#include "Enemies/BasicEnemy.h"
#include "Components/DecalComponent.h"
#include "Cauldron.generated.h"


/**
 * 
 */
UENUM()
enum class ECauldron : uint8
{
	Disabled, // Cauldron is disabled
	Enabled, // Cauldron is activated
	WaitingSet // Cauldron is waiting to be setted
};



UCLASS()
class LUNARPUNK_API ACauldron : public ATrap
{
	GENERATED_BODY()


public:
	ACauldron();

	UPROPERTY(VisibleAnywhere)
		ECauldron State = ECauldron::Disabled;


		UPROPERTY(EditAnywhere, Category = "Config")
		float Diameter = 2000.0f;

		UPROPERTY(EditAnywhere, Category = "Config")
			float DamagePerSecond = 10.0f;
		UPROPERTY(EditAnywhere, Category = "Config")
			float NumberOfHits = 5.0f;

		UPROPERTY(EditAnywhere, Category = "Config")
			float TimeBetweenHits = 1.0f;

	//Angle in wich the explosion will have effect. The half of the angle to the right and the other half to the left of the actor forward vector.

		UPROPERTY(EditAnywhere, Category = Interaction)
			class UHealthComponent* PoisonedHealthComponent;

	UPROPERTY(EditAnywhere, Category = Interaction)
		class USphereComponent* SphereSetRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UNiagaraComponent* BubblePoisonEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UDecalComponent* FloorEffect;


	UPROPERTY(EditAnywhere, Category = "Config")
		UMaterial* ActivatedLightMaterial;

	UPROPERTY(EditAnywhere, Category = "Config")
		UMaterial* DeactivatedLightMaterial;
	


	virtual void BeginPlay() override;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	void MakePoisonDamage();

	//UFUNCTION()
		//void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);
	UFUNCTION(BlueprintCallable)
		virtual void DoTrapEffect() override;

	void DisableTrapEffect();

	//virtual void DeactivateTrap() override;

	//virtual void ReactivateTrap() override;


private:

	//void OverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	FTimerHandle TimerHandleCauldron;

	FTimerHandle TimerHandleCauldron2;

	UStaticMeshComponent* Light;

	void TurnOnDeactivatedLight();

	void TurnOnActivatedLight();

};
