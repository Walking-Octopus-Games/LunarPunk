// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AvoidEnemies.generated.h"

class ABasicEnemy;
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUNARPUNK_API UAvoidEnemies : public UActorComponent
{
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UAvoidEnemies();

protected:
  void DecreaseEnemiesSpeed();
  UFUNCTION()
    void OnBeginOverlapAvoidRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep,
      const FHitResult& SweepResult);
  UFUNCTION()
    void OnEndOverlapAvoidRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
  // Called when the game starts
  virtual void BeginPlay() override;

public:
  // Called every frame
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USphereComponent* AvoidSphereCollision;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bAvoidOtherEnemies = false;

  UPROPERTY(VisibleAnywhere)
    TArray<ABasicEnemy*> OverlappedEnemies;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AvoidanceSpeed;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AvoidanceOffset;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxSpeedConstant;
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float OriginalVelocity;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinVelocity;


  UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FTimerHandle DecreaseEnemySpeed;
  UPROPERTY(EditAnywhere)
    UCharacterMovementComponent* CharacterComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<UCharacterMovementComponent*> OverlappedEntryCharacters;
};
