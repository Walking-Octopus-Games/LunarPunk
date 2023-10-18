// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/PlayableCharacter.h"
#include "Turrets/Turret.h"
#include "TurretInteractionComponent.generated.h"

class UDecalComponent;
class ALunarPunkGameMode;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerStartReparingTurret);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerStopReparingTurret);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerStartTakeTurret);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAutoMoveTurretsDelegate, bool, IsAllTurretsMoving);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUNARPUNK_API UTurretInteractionComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UTurretInteractionComponent();

protected:
  // Called when the game starts
  virtual void BeginPlay() override;

public:
  // Called every frame
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  UFUNCTION()
  void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult);
  UFUNCTION()
  void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
  UFUNCTION()
    void Initialize();

  UFUNCTION(BlueprintCallable)
    void AutoMoveTurrets();

  UFUNCTION(BlueprintCallable)
    void GetNearestTurret();

  UFUNCTION()
    void PositionTurret();

  UFUNCTION()
    void TreatTurret();

  UFUNCTION()
    void StopTreatingTurret();

  UFUNCTION()
    void UpgradeTurret();

  UFUNCTION()
    void TakeTurret();

  UFUNCTION()
    void SetTurret();

  UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable)
    FAutoMoveTurretsDelegate AutoMoveTurretsDelegate;

  UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable, Category = "Player Reparing")
    FPlayerStartReparingTurret PlayerStartReparingTurret;

  UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable, Category = "Player Reparing")
      FPlayerStartReparingTurret PlayerStopReparingTurret;

  UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable, Category = "Player Taking Turret")
      FPlayerStartTakeTurret PlayerStartTakeTurret;

  UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable, Category = "Player Taking Turret")
      FPlayerStartTakeTurret PlayerStopTakeTurret;

  UPROPERTY(EditAnywhere, category = "HeightToSetTurret")
      float HeightToSetTurret = 200.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Deactivate Rotation" )
      bool DeactivateRotationDuringReparing = false;
  

private:

  UPROPERTY()
    APlayableCharacter* PlayableCharacter;

  UPROPERTY()
    ALunarPunkGameMode* GameMode;

  UPROPERTY(VisibleAnywhere, Category = Interaction)
    class UBoxComponent* PositioningCollider;

  UPROPERTY()
    class ATurret* NearestTurret = nullptr;

  UPROPERTY()
    class ATurret* InteractedTurret = nullptr;

  UPROPERTY()
    bool bUpdateNearestTurret = false;

  UPROPERTY(EditAnywhere, category = "HeightToRepair")
      float HeightToRepair = 0.0f;

  
  UPROPERTY(EditAnywhere, category = "Interaction | Turret")
    int OffsetTurretNewPosition = 200.0f;

  UFUNCTION()
    bool Interact(bool bStartInteraction);
};
