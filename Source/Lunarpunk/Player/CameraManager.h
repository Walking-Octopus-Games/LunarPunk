// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "CameraManager.generated.h"

class ACameraChangeZone;
class APlayableCharacter;

// Event called when the player controller is initialized
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCameraPositionChanged);
/**
 *
 */
UCLASS()
class LUNARPUNK_API ACameraManager : public APlayerCameraManager
{
  GENERATED_BODY()

public:
  ACameraManager();


  UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Camera")
    FVector Position = FVector(0, 0, 0);

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Camera")
    FRotator InitialRotation = FRotator(0, 0, 0);

  //Current rotation in wich the camera should rest. (Rotation to keep after shaking).
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Camera")
    FRotator CurrentRestingRotation = FRotator(0, 0, 0);

  //Current Rotation of the camera
  UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Camera")
    FRotator Rotation = FRotator(0, 0, 0);

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Cinematic")
    bool bIsCinematicPlaying = false;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Cinematic")
    float ZoomingQuantity = 200;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Camera")
    bool bShakeEffectEnabled = true;

  FCameraPositionChanged CameraPositionChanged;

  void UpdateCamera(float DeltaTime) override;

  virtual void BeginPlay() override;

  UFUNCTION(BlueprintCallable)
    void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime);

  void ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot);

  UFUNCTION(BlueprintCallable, category = "Cinematic")
    void SetIsCinematicPlaying(bool bIsPlaying);

  UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void StartShakingCamera(float UpDownRotationSpeed, float RightLeftRotationSpeed, float UpDownAngle, float RightLeftAngle, bool ShouldVibrate = false);

  UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void StopShakingCamera();

  UFUNCTION(BlueprintCallable)
    void AddCameraRotation(const FRotator& RotationOffset, bool ShouldModifyRestingRotation = false);

  UFUNCTION(BlueprintCallable)
    void SetCameraRotation(const FRotator& NewRotation, bool ShouldModifyRestingRotation = false);

  UFUNCTION(BlueprintCallable)
    void UpdateRestingRotation();

  UFUNCTION(BlueprintCallable)
     void StopCameraChangeZones(ACameraChangeZone* NonStopZone = nullptr);

  UFUNCTION(BlueprintCallable)
    void ZoomInOnDeath();

private:

  UPROPERTY()
    bool bIsZooming = false;

  UPROPERTY()
    FVector InitialZoomPosition;
  
  UPROPERTY()
    FRotator InitialZoomRotation;

  UPROPERTY()
    float CurrentZoom = 0;

  UPROPERTY()
    float CurrentZoomAlpha = 0;

  UPROPERTY()
    APlayableCharacter *PlayerCharacter;

  TArray<ACameraChangeZone*> CameraChangeZones;

  void SetCameraChangeZones();

};
