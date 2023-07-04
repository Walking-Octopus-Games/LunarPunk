// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Camera/CameraChangeZone.h"
#include "Player/PlayableCharacter.h"
#include "HealthSystem/HealthComponent.h"
#include "Kismet/KismetMathLibrary.h"

ACameraManager::ACameraManager()
{
  ViewPitchMin = -87.0f;
  ViewPitchMax = 87.0f;
  bAlwaysApplyModifiers = true;

}

void ACameraManager::UpdateCamera(float DeltaTime)
{
  Super::UpdateCamera(DeltaTime);
}

void ACameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
  Super::UpdateViewTarget(OutVT, DeltaTime);

  if (!bIsCinematicPlaying)
  {
    if (IsValid(PlayerCharacter))
    {
      if (bIsZooming)
      {
        FRotator Target = UKismetMathLibrary::FindLookAtRotation(PlayerCharacter->GetActorLocation() + InitialZoomPosition, PlayerCharacter->GetActorLocation());
        Rotation = UKismetMathLibrary::RInterpTo(InitialZoomRotation, Target, DeltaTime, 0.5);
        
        if (CurrentZoomAlpha < 1)
        {
          CurrentZoom = UKismetMathLibrary::Ease(0, ZoomingQuantity, CurrentZoomAlpha, EEasingFunc::EaseOut);
          CurrentZoomAlpha += DeltaTime;
          Position = InitialZoomPosition + Rotation.Vector() * CurrentZoom;
        }
      }
      OutVT.POV.Location = PlayerCharacter->GetActorLocation() + Position;
      OutVT.POV.Rotation = Rotation;
    }
  }
}

void ACameraManager::ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
  Super::ProcessViewRotation(DeltaTime, OutViewRotation, OutDeltaRot);
}

void ACameraManager::SetIsCinematicPlaying(bool bIsPlaying)
{
  bIsCinematicPlaying = bIsPlaying;
}


void ACameraManager::BeginPlay()
{
  Super::BeginPlay();

  InitialRotation = Rotation;
  CurrentRestingRotation = InitialRotation;

  SetCameraChangeZones();

  if (IsValid(GetOwningPlayerController()) && IsValid(GetOwningPlayerController()->GetCharacter()))
  {
    PlayerCharacter = Cast<APlayableCharacter>(GetOwningPlayerController()->GetCharacter());
    PlayerCharacter->HealthComponent->ActorDeath.AddDynamic(this, &ACameraManager::ZoomInOnDeath);
  }
}

void ACameraManager::SetCameraRotation(const FRotator& NewRotation, bool ShouldModifyRestingRotation)
{
    Rotation = NewRotation;

    //Modify the resting
    if (ShouldModifyRestingRotation)
    {
        UpdateRestingRotation();
    }

    CameraPositionChanged.Broadcast();
}

void ACameraManager::AddCameraRotation(const FRotator& RotationOffset, bool ShouldModifyRestingRotation)
{
    SetCameraRotation(Rotation + RotationOffset, ShouldModifyRestingRotation);
}

void ACameraManager::UpdateRestingRotation()
{
    CurrentRestingRotation = Rotation;
}

void ACameraManager::StopCameraChangeZones(ACameraChangeZone* NonStopZone)
{
    for (ACameraChangeZone* CameraChange : CameraChangeZones)
    {
        if (CameraChange != NonStopZone)
        {
            CameraChange->StopRotation();
        }
    }
}

void ACameraManager::SetCameraChangeZones()
{
    //Obtains all the camera change zones
    TArray<AActor*> CamerasChangesActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraChangeZone::StaticClass(), CamerasChangesActors);

    //Set the camera change zones
    for (AActor* CameraChangeZone : CamerasChangesActors)
    {
        ACameraChangeZone* CameraChangeCasted = Cast<ACameraChangeZone>(CameraChangeZone);

        if (CameraChangeCasted)
        {
            CameraChangeZones.Add(CameraChangeCasted);
            CameraChangeCasted->CameraChangeZoneActivated.AddDynamic(this, &ACameraManager::StopCameraChangeZones);
        }
    }
}

void ACameraManager::ZoomInOnDeath()
{
  bShakeEffectEnabled = false;
  bIsZooming = true;
  CurrentZoomAlpha = 0;
  CurrentZoom = 0;
  InitialZoomPosition = Position;
  InitialZoomRotation = Rotation;
}