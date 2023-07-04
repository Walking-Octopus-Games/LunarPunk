// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageComponent.generated.h"

class UHealthComponent;
class AProjectile;
class AAIController;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDamageReceive);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUNARPUNK_API UDamageComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UDamageComponent();

protected:
  // Called when the game starts
  virtual void BeginPlay() override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<UHealthComponent*> ActualHealthComponentsBeingDamage;



  bool IsTheTargetAtTheCorrectDistance(AActor* Actor);

  void DamageComponent(UHealthComponent* HealthComponent, float Damage);

  void DrawEnemyDamagePointWidget(const UHealthComponent* HealthComponentToDamage);
public:
  // Called every frame
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  bool bIsDamagingPerSecond = false;

  /*Damage of one hit attacks*/
  UPROPERTY(EditAnywhere, Category = "Basic Atrributes")
    float BaseDamageStrength = 100;

  /*Damage of one hit attacks*/
  UPROPERTY(EditAnywhere, Category = "Basic Atrributes")
    float DamageStrength = BaseDamageStrength;

  /*Base Damage per second attacks*/
  UPROPERTY(EditAnywhere, Category = "Basic Atrributes")
    float BaseDPSStrength = 2;

  /*Damage per second attacks*/
  UPROPERTY(EditAnywhere, Category = "Basic Atrributes")
    float DPSStrength = BaseDPSStrength;

  /*The max distance to apply damage to the target. If zero this property is ignored.*/
  UPROPERTY(EditAnywhere, Category = "Basic Atrributes")
    float MaxDistanceToDamage = 0.0f;

  /*Make one hit damage*/
  UFUNCTION(BlueprintCallable)
    void MakeDamage(UHealthComponent* HealthComponent);

  /*Start Making damage per second to an actor*/
  UFUNCTION(BlueprintCallable)
    void MakeDamagePerSecond(UHealthComponent* HealthComponent);

  /*Stop Making damage per second to an actor*/
  UFUNCTION(BlueprintCallable)
    void StopDamagePerSecond(UHealthComponent* HealthComponent);

  float GetDPSSBase()
  {
    return BaseDPSStrength;
  }

  void SetDamage(float NewDPSS, float NewDamageStrength);

  void ResetComponent();

  //Player vampirism component.
  class UVampirismComponent* VampirismComponent;

  /*Applies force to the actor in the direction of the projectile forward vector*/
  UFUNCTION()
    void ApplyThrustImpact(FVector Forward, float Force, UCharacterMovementComponent* ActorHitted, AAIController* AIEnemyController);

  FDamageReceive DamageReceiveEvent;



private:

  bool bIsMakingDamage = false;

};
