// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trap.generated.h"


#define STENCIL_TRAP 252
#define STENCIL_TRAP_WITH_OUTLINE 255 //Adds outline

class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTrapLunched);

UCLASS(Abstract)
class LUNARPUNK_API ATrap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrap();

	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere, Category = "Config")
	//Num of hits to activate the trap
	int32 HitsToLaunch = 1;

	UPROPERTY(EditAnywhere, Category = "Config")
	//Times the trap can be activated before disapear.
	int32 Lives = 1;

	UPROPERTY(EditAnywhere, Category = "Config")
	//Time to reactivate the trap after it has been activated.
	float TimeToReactivate = 4.0f;

	UPROPERTY()
	//Event when a trap is lunched
	FTrapLunched TrapLunched;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Do a hit to the trap
	virtual void HitTrap();

	//Launch the trap when the current hits are equal to the hits to activate
	virtual void LaunchTrap();

	//Deactivates the trap
	virtual void DeactivateTrap();

	//Set the trap ready for launch
	virtual void ReactivateTrap();

	virtual void DestroyTrap();

	//Trap effect implementation
	virtual void DoTrapEffect() PURE_VIRTUAL(ATrap::DoTrapEffect, );

	void TurnOnOutline();

	void TurnOffOutline();

private:



	UPROPERTY(VisibleAnywhere, Category = "Debug")
	int32 RemainingLives = Lives;

	UPROPERTY(VisibleAnywhere, Category = "Debug")
	int32 CurrentHits = 0;

	//Timer handle to manage the reactivate delay.
	FTimerHandle TimerHandle;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
