// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/EnemyTargetPoints.h"
#include "Components/ActorComponent.h"
#include "TargetForEnemiesHandler.generated.h"

class ABasicEnemy;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUNARPUNK_API UTargetForEnemiesHandler : public UActorComponent
{
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UTargetForEnemiesHandler();


  UFUNCTION()
    void TurretDeactivated();

  UFUNCTION()
    void TurretReactivated();

  UFUNCTION()
    void Initialize(ATurret* Turret);
    UFUNCTION()
    void GetETPAndAssignItToAFreeEnemy();
protected:

  UFUNCTION()
    void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult);

  UFUNCTION()
    void GetAFreeETP(float PointDistanceToEnemy, UEnemyTargetPoints*& ETPToEnemy);

  UFUNCTION()
    ABasicEnemy* GetFreeEnemy();



  UFUNCTION()
    void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


public:
  // Called every frame
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  //UPROPERTY(EditAnywhere, BlueprintReadWrite)
  //  USphereComponent* AreaForEnemies;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TArray<ABasicEnemy*> EnemiesInsideTheArea;
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    ATurret* OwnerTurret;
};
