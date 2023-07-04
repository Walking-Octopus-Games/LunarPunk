// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Pool/IsPoolable.h"
#include "Components/SphereComponent.h"
//#include "Enemies/EnemyBasicComponent.h"
#include "Enemies/BasicEnemy.h"
#include "HealthSystem/DamageComponent.h"

#include "HealthSystem/HealthComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Sound/SoundBase.h"
#include "Particles/ParticleSystem.h" 

#include "TimerManager.h"
#include "Granade.generated.h"

//#include "Projectile.generated.h"

class UNiagaraSystem;


UCLASS()
class LUNARPUNK_API AGranade : public AActor, public IIsPoolable
{
private:
	GENERATED_BODY()

	
public:
	// Sets default values for this actor's properties
	AGranade();


	UPROPERTY(EditAnywhere, Category = "Variables")
		float GranadeDistance;

	void setGranadeDistance(float value);

	float getGranadeDistance();

	UPROPERTY()
		APool* Pool;

	UFUNCTION(BlueprintImplementableEvent)
		void ShowParticle();

	UPROPERTY(EditAnywhere, category = "Gameplay")
		USoundBase* ExplosionSound;

	//The mesh component compoent
	UPROPERTY(VisibleAnywhere, Category = "Grenade", BlueprintReadOnly)
		UStaticMeshComponent* MeshComponent;

	//Projectile component movement
	UPROPERTY(EditAnywhere, Category = "Grenade")
		UProjectileMovementComponent* ProjectileMovementComponent;

	//The collision sphere compoent
	UPROPERTY(VisibleAnywhere, Category = "Grenade")
		USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Grenade", BlueprintReadOnly)
		USphereComponent* ExplosionComponent;

	//Radius  
	UPROPERTY(EditAnywhere, Category = "Grenade")
		float ExplosionRadius = 3000.0f;

	//Time to explosion
	UPROPERTY(EditAnywhere, Category = "Grenade")
		float Timetoxplode = 3.0f;

	//Granade damage 
	UPROPERTY(EditAnywhere)
		UDamageComponent* DamageComponent;



	//UPROPERTY(EditAnywher, Category = "Grenade")
	//	UNiagaraSystem* NS_Explosion;

	//This fuction will be called when the granade hits something

	UFUNCTION()
		void Explode();

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	virtual void ReturnToPool() override;
	void FireInDirection(const FVector& ShootDirection);
	UFUNCTION(BlueprintCallable)
	virtual void Reactivate() override;
	UFUNCTION(BlueprintCallable)

	virtual void Deactivate() override;

	FVector ProjectileVelocity = FVector(1000.0, 2000.0, 0.0);

private:

	FTimerHandle TimerHandle;

	UFUNCTION()
		void OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit);


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UNiagaraSystem* NS_Explosion;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);



};
