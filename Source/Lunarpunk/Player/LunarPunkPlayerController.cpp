// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LunarPunkPlayerController.h"

#include "CameraManager.h"
#include "PlayableCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "LevelSequencePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "AimAssist.h"


void ALunarPunkPlayerController::BeginPlay()
{
  Super::BeginPlay();
}

void ALunarPunkPlayerController::Initialize(APlayableCharacter* _PlayableCharacter)
{
  PlayableCharacter = _PlayableCharacter;


  CameraManager = Cast<ACameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));/*Cast<ACameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));*/

  //Changin listener to player pos
  SetAudioListenerAttenuationOverride(PlayableCharacter->GetRootComponent(), PlayableCharacter->GetActorLocation());

  InitializeInput(PlayableCharacter, true);

  //Values for the correct movement of the camera and the player.
  AdjustControllerToCamera();

  if (CameraManager)
  {
    CameraManager->CameraPositionChanged.AddDynamic(this, &ALunarPunkPlayerController::AdjustControllerToCamera);
  }

  SetMouseIsController(true);

  SetAudioListenerOverride(PlayableCharacter->GetRootComponent(), FVector::ZeroVector, FRotator::ZeroRotator);
  InitializeEvent.Broadcast();
}

void ALunarPunkPlayerController::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (IsValid(CameraManager))
  {
    RotateTowardsMouse();
  }

}

void ALunarPunkPlayerController::InitializeInput(APlayableCharacter* PlayableChar, bool bIsActive)
{
  bInputIsActive = bIsActive;

  // Setup Actions
  if (bIsActive)
  {
    SetupBinding(FName("Shoot"), IE_Pressed, [PlayableChar, this]()
      {
        if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
        {
          return;
        }
        if (PlayableChar->ActualPlayerStatus == EPlayerState::Default)
        {
          PlayableChar->ActualPlayerStatus = EPlayerState::Shooting;
          ShootEvent.Broadcast(true);
        }
      });


    SetupBinding(FName("Shoot"), IE_Released, [PlayableChar, this]()
      {

        ShootEvent.Broadcast(false);
        /* if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
         {
           return;
         }*/
        if (PlayableChar->ActualPlayerStatus == EPlayerState::Shooting)
        {
          PlayableChar->ActualPlayerStatus = EPlayerState::Default;
        }
      });


    ReactivateAbbilities(PlayableChar);

    ReactivateGrenadeAbbility(PlayableChar);


    ReactivateShieldAbbility(PlayableChar);

    ReactivateSparkleAbbility(PlayableChar);

 

    SetupBinding(FName("ThrowActivateTraps"), IE_Pressed, [PlayableChar, this]()
      {
        if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
        {
          return;
        }
        if (PlayableChar->ActualPlayerStatus == EPlayerState::Default)
        {
          PlayableChar->ActualPlayerStatus = EPlayerState::LaunchingAbility;
          ThrowActivateTrapsAbility.Broadcast();
        }
      });



    SetupBinding(FName("ThrowActivateTraps"), IE_Released, [PlayableChar, this]()
      {
        if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
        {
          return;
        }
        StopThrowingActivateTrapsAbility.Broadcast();
        if (PlayableChar->ActualPlayerStatus == EPlayerState::LaunchingAbility)
        {
          PlayableChar->ActualPlayerStatus = EPlayerState::Default;
        }
      });


    SetupBinding(FName("GodMode"), IE_Pressed, [PlayableChar, this]()
      {
        if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
        {
          return;
        }

        GodModeEvent.Broadcast();
      });

    SetupBinding(FName("GodModeTurrets"), IE_Pressed, [PlayableChar, this]()
      {
        if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
        {
          return;
        }

        GodModeTurretsEvent.Broadcast();
      });


    SetupBinding(FName("GetAllPlates"), IE_Pressed, [PlayableChar, this]()
      {
        if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
        {
          return;
        }

        GetAllPlatesEvent.Broadcast();
      });


    SetupBinding(FName("RefillUpgrades"), IE_Pressed, [PlayableChar, this]()
        {
            if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
            {
                return;
            }

            RefillUpgradesEvent.Broadcast();
        });


    SetupBinding(FName("KillEnemies"), IE_Pressed, [PlayableChar, this]()
        {
            if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
            {
                return;
            }

            KillEnemiesEvent.Broadcast();
        });

    SetupBinding(FName("PositionTurret"), IE_Pressed, [PlayableChar, this]()
      {
        if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
        {
          return;
        }
        if (PlayableChar->ActualPlayerStatus == EPlayerState::PositioningTurret)
        {
          //Leaving Turret
          //PlayableChar->ActualPlayerStatus = EPlayerState::Default;
          PositionTurretEvent.Broadcast();
        }
        else if (PlayableChar->ActualPlayerStatus == EPlayerState::Default)
        {
          //PlayableChar->ActualPlayerStatus = EPlayerState::PositioningTurret;

          PositionTurretEvent.Broadcast();
        }
      });

    SetupBinding(FName("TreatTurret"), IE_Pressed, [PlayableChar, this]()
      {
        if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
        {
          return;
        }
        if (PlayableChar->ActualPlayerStatus == EPlayerState::Default)
        {
          PlayableChar->ActualPlayerStatus = EPlayerState::TurretInteraction;

          TreatTurretEvent.Broadcast();
        }
      });

    SetupBinding(FName("TreatTurret"), IE_Released, [PlayableChar, this]()
      {
        if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
        {
          return;
        }
        if (PlayableChar->ActualPlayerStatus == EPlayerState::TurretInteraction)
        {
          StopTreatingTurretEvent.Broadcast();
          PlayableChar->ActualPlayerStatus = EPlayerState::Default;
        }
      });

    SetupBinding(FName("UpgradeTurret"), IE_Pressed, [PlayableChar, this]()
      {
        if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
        {
          return;
        }
        if (PlayableChar->ActualPlayerStatus == EPlayerState::Default)
        {
          PlayableChar->ActualPlayerStatus = EPlayerState::TurretInteraction;

          UpgradeTurretEvent.Broadcast();
          // Change state in turret component
          //PlayableCharacter->ActualPlayerStatus = EPlayerState::Default;
        }
      });

    SetupBinding(FName("UpgradeTurret"), IE_Released, [PlayableChar, this]()
      {
        if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
        {
          return;
        }
        if (PlayableChar->ActualPlayerStatus == EPlayerState::TurretInteraction)
        {
          //if (PlayableChar->ActualPlayerStatus == EPlayerState::Default)
          PlayableChar->ActualPlayerStatus = EPlayerState::Default;
        }
        //UpgradeTurretEvent.Broadcast();

        // Change state in turret component
        //PlayableCharacter->ActualPlayerStatus = EPlayerState::Default;
      });

    SetupBinding(FName("SelectNextUpgrade"), IE_Pressed, [PlayableChar, this]()
      {
        if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
        {
          return;
        }

        SelectNextUpgradeEvent.Broadcast();
      });

    SetupBinding(FName("SelectPreviousUpgrade"), IE_Pressed, [PlayableChar, this]()
      {
        if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
        {
          return;
        }

        SelectPreviousUpgradeEvent.Broadcast();
      });



    ReactivateMoveAllTurrets(PlayableChar);


    SetupBinding(FName("DestroyPortalButton1"), IE_Pressed, [PlayableChar, this]()
      {
        if (PlayableChar->ActualPlayerStatus == EPlayerState::DestroyingPortal)
        {
          DestroyPortalShootButton1Event.Broadcast(true);
        }
      }
    );

    SetupBinding(FName("DestroyPortalButton1"), IE_Released, [PlayableChar, this]()
      {
        /*if (PlayableChar->ActualPlayerStatus == EPlayerState::DestroyingPortal)
        {
        }*/
        DestroyPortalShootButton1Event.Broadcast(false);
      }
    );

    SetupBinding(FName("DestroyPortalButton2"), IE_Pressed, [PlayableChar, this]()
      {
        if (PlayableChar->ActualPlayerStatus == EPlayerState::DestroyingPortal)
        {
          DestroyPortalShootButton2Event.Broadcast(true);
        }
      }
    );


    SetupBinding(FName("DestroyPortalButton2"), IE_Released, [PlayableChar, this]()
      {
        /*  if (PlayableChar->ActualPlayerStatus == EPlayerState::DestroyingPortal)
          {
          }*/
        DestroyPortalShootButton2Event.Broadcast(false);
      }
    );

    SetupBinding(FName("SkipCinematic"), IE_Pressed, [PlayableChar, this]()
      {
        if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
        {
          SkipCinematic.Broadcast(true);
        }
      }
    );

    SetupBinding(FName("SkipCinematic"), IE_Released, [PlayableChar, this]()
      {
        if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
        {
          SkipCinematic.Broadcast(false);
        }
      }
    );


    SetupBinding(FName("SkipTutorial"), IE_Pressed, [PlayableChar, this]()
        {
            if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
            {
                SkipTutorial.Broadcast();
            }
            if (PlayableChar->ActualPlayerStatus == EPlayerState::Default)
            {
                SkipTutorial.Broadcast();
            }
        }
    );

    SetupBinding(FName("SetMouseActive"), IE_Pressed, [PlayableChar, this]()
      {
        if (!bMouseActive)
        {
          SetMouseIsController(true);
        }
      }
    );

    SetupBinding(FName("SetControllerActive"), IE_Pressed, [PlayableChar, this]()
      {
        if (bMouseActive)
        {
          SetMouseIsController(false);
        }
      }
    );

    if (IsValid(InputComponent))
    {
      // Setup Axis
      InputComponent->BindAxis(FName("MoveForward"), this, &ALunarPunkPlayerController::MoveForward);
      InputComponent->BindAxis(FName("MoveRight"), this, &ALunarPunkPlayerController::MoveRight);
      InputComponent->BindAxis(FName("Turn"), this, &ALunarPunkPlayerController::TurnX);
      InputComponent->BindAxis(FName("TurnY"), this, &ALunarPunkPlayerController::TurnY);
      InputComponent->BindAxis(FName("SetControllerActiveAxis"), this, &ALunarPunkPlayerController::SetControllerActiveThumbStick);
    }
  }
  else
  {
    RemoveInput();
  }
}

void ALunarPunkPlayerController::RemoveInput()
{
  InputComponent->ClearActionBindings();
  InputComponent->ClearBindingValues();
}

void ALunarPunkPlayerController::SetCutsceneMode(bool bIsEnabled)
{
  if (IsValid(PlayableCharacter))
  {
    if (bIsEnabled)
    {
      PlayableCharacter->ActualPlayerStatus = EPlayerState::Cinematic;
    }
    else
    {
      PlayableCharacter->ActualPlayerStatus = EPlayerState::Default;
    }
  }
}

void ALunarPunkPlayerController::SetupInputComponent()
{
  Super::SetupInputComponent();

}

void ALunarPunkPlayerController::MoveForward(float value)
{
  if (PlayableCharacter->ActualPlayerStatus == EPlayerState::Cinematic)
  {
    return;
  }
  // bCanDestroyPortal -> its true while charging the ray, so cant move.
  if (IsValid(CameraManager) && !IsDeactivated)
  {

    CurrentVel = FMath::Lerp(CurrentVel, MaxVel, AcelPlayer);

    if (value == 0 && LastRight == 0)
    {
      CurrentVel = 0;

      PlayerMovementStoppedEvent.Broadcast();
    }
    else
    {

      if (value * LastRight > 0)
      {

        PlayableCharacter->AddMovementInput(CameraForward, CurrentVel * value * XAxisCorrection);
      }
      else
      {
        PlayableCharacter->AddMovementInput(CameraForward, CurrentVel * value * XAxisCorrection2);
      }
      LastForward = value;

      PlayerIsMovingEvent.Broadcast();
    }


  }

}

void ALunarPunkPlayerController::MoveRight(float value)
{
  if (PlayableCharacter->ActualPlayerStatus == EPlayerState::Cinematic)
  {
    return;
  }
  // bCanDestroyPortal -> its true while charging the ray, so cant move.
  if (IsValid(CameraManager) && !IsDeactivated)
  {
    CurrentVel = FMath::Lerp(CurrentVel, MaxVel, AcelPlayer);
    if (value * LastForward > 0)
    {
      PlayableCharacter->AddMovementInput(CameraRight, CurrentVel * value * YAxisCorrection);
    }
    else
    {
      PlayableCharacter->AddMovementInput(CameraRight, CurrentVel * value * YAxisCorrection2);
    }
    LastRight = value;

    if (value == 0 && LastForward == 0)
    {
      CurrentVel = 0;
    }
  }
}

void ALunarPunkPlayerController::TurnX(float value)
{
  if (PlayableCharacter->ActualPlayerStatus == EPlayerState::Cinematic)
  {
    return;
  }
  if (IsValid(CameraManager))
  {
    TurnXValue = value;
  }
}

void ALunarPunkPlayerController::TurnY(float value)
{
  if (PlayableCharacter->ActualPlayerStatus == EPlayerState::Cinematic)
  {
    return;
  }
  if (IsValid(CameraManager))
  {
    TurnYValue = value;
    RotateGamePad();
  }
}

void ALunarPunkPlayerController::RotateTowardsMouse()
{
  if (PlayableCharacter->ActualPlayerStatus == EPlayerState::Cinematic)
  {
    return;
  }
  FVector MouseLocation;
  FVector MouseDirection;

  // Gamepad CameraRight Stick input deactivate mouse until game restart.
  // Player can't rotate in the destroy portal zone
  if (bMouseActive && !IsDeactivated && DeprojectMousePositionToWorld(MouseLocation, MouseDirection))
  {
    FVector ActorLocation = PlayableCharacter->GetActorLocation();
    FVector Intersection = FMath::LinePlaneIntersection(MouseLocation, MouseLocation + MouseDirection, FVector(0, 0, ActorLocation.Z), FVector::UpVector);
    const FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(ActorLocation, Intersection);
    PlayableCharacter->SetActorRotation(NewRotation);
  }
}

void ALunarPunkPlayerController::RotateGamePad()
{
  if (PlayableCharacter->ActualPlayerStatus == EPlayerState::Cinematic)
  {
    return;
  }
  if (!IsDeactivated)
  {

    if ((0.15 < TurnXValue || TurnXValue < -0.15) || (0.15 < TurnYValue || TurnYValue < -0.15))
    {
      if (!bMouseActive)
      {
        //SetMouseIsController(false);

        FVector TargetRotation = TurnXValue * CameraRight - TurnYValue * CameraForward;

        PlayerRotation = PlayableCharacter->GetActorRotation();
        GoalRotation = UKismetMathLibrary::FindLookAtRotation(PlayableCharacter->GetActorLocation(), PlayableCharacter->GetActorLocation() + TargetRotation);

        GoalRotation = FMath::Lerp(PlayerRotation, GoalRotation, RotationSpeed);

        FRotator NewRot;

        bool bAimAssist = PlayableCharacter->GetAimAssistComponent()->AimAssist(
          PlayableCharacter->StartShootReferencePoint->GetComponentLocation(),
          GoalRotation.Quaternion().GetForwardVector(),
          NewRot
        );

        if (bAimAssist && PlayableCharacter->ActualPlayerStatus != EPlayerState::PositioningTurret)
        {
          PlayableCharacter->SetActorRotation(FMath::Lerp(PlayerRotation, NewRot, RotationSpeed));
        }
        else
        {
          PlayableCharacter->SetActorRotation(GoalRotation);
        }
      }
    }
  }
}

void ALunarPunkPlayerController::UpDirectionalPad()
{

  if (PlayableCharacter->ActualPlayerStatus == EPlayerState::Cinematic)
  {
    return;
  }
  if (PlayableCharacter->ActualPlayerStatus == EPlayerState::Default)
  {
    // PlayableCharacter->ActualPlayerStatus = EPlayerState::LaunchingAbility;
    ThrowGrenadeAbility.Broadcast();
  }
}

void ALunarPunkPlayerController::RightDirectionalPad()
{

  if (PlayableCharacter->ActualPlayerStatus == EPlayerState::Cinematic)
  {
    return;
  }
  if (PlayableCharacter->ActualPlayerStatus == EPlayerState::Default)
  {
    //PlayableCharacter->ActualPlayerStatus = EPlayerState::LaunchingAbility;
    ThrowSparkleAbility.Broadcast();
    
  }
}
void ALunarPunkPlayerController::DownDirectionalPad()
{
  if (PlayableCharacter->ActualPlayerStatus == EPlayerState::Cinematic)
  {
    return;
  }
  if (PlayableCharacter->ActualPlayerStatus == EPlayerState::Default)
  {
    // PlayableCharacter->ActualPlayerStatus = EPlayerState::LaunchingAbility;
    ThrowActivateTrapsAbility.Broadcast();
  }
}
void ALunarPunkPlayerController::LeftDirectionalPad()
{
  if (PlayableCharacter->ActualPlayerStatus == EPlayerState::Cinematic)
  {
    return;
  }
  if (PlayableCharacter->ActualPlayerStatus == EPlayerState::Default)
  {
    //PlayableCharacter->ActualPlayerStatus = EPlayerState::LaunchingAbility;
    ThrowShieldAbility.Broadcast();
  }
}

void ALunarPunkPlayerController::ResetMoveTurretsCooldown()
{
  bCanMoveTurrets = true;
}



void ALunarPunkPlayerController::AdjustControllerToCamera()
{
  if (IsValid(CameraManager))
  {
    const float YawCamera = CameraManager->Rotation.Yaw;
    UE_LOG(LogTemp, Warning, TEXT("Angulo: %f"), YawCamera);
    const FRotator Rotator = FRotator(0, YawCamera, 0);
    const float CameraAngle1 = (360 - YawCamera + 5.0) * 3.14 / 180;

    const FRotator Rotator2 = FRotator(0, -104, 0);
    CameraRight = Rotator.RotateVector(FVector(0, 1, 0));
    CameraForward = Rotator2.RotateVector(CameraRight);
    XAxisCorrection = 10 * FMath::Cos(CameraAngle1);
    YAxisCorrection = 10 * FMath::Sin(CameraAngle1);

    const float CameraAngle2 = (360 - YawCamera - 9) * 3.14 / 180;
    XAxisCorrection2 = 10 * FMath::Cos(CameraAngle2);
    YAxisCorrection2 = 10 * FMath::Sin(CameraAngle2);
  }
}


void ALunarPunkPlayerController::DeactivateAbbilities()
{

  InputComponent->RemoveActionBinding(FName("ActiveAbility"), IE_Pressed);
  InputComponent->RemoveActionBinding(FName("ActiveAbility"), IE_Released);

}

void ALunarPunkPlayerController::DeactivateSparkleAbbility()
{
    InputComponent->RemoveActionBinding(FName("ThrowSparkle"), IE_Pressed);
    InputComponent->RemoveActionBinding(FName("ThrowSparkle"), IE_Released);
}


void ALunarPunkPlayerController::DeactivateShieldAbbility()
{
    InputComponent->RemoveActionBinding(FName("ThrowShield"), IE_Pressed);
    InputComponent->RemoveActionBinding(FName("ThrowShield"), IE_Released);
}


void ALunarPunkPlayerController::DeactivateGrenadeAbbility()
{
    InputComponent->RemoveActionBinding(FName("ThrowGrenade"), IE_Pressed);
    InputComponent->RemoveActionBinding(FName("ThrowGrenade"), IE_Released);
}

void ALunarPunkPlayerController::ReactivateAbbilities(APlayableCharacter* PlayableChar)
{


  SetupBinding(FName("ActiveAbility"), IE_Pressed, [PlayableChar, this]()
    {
      if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
      {
        return;
      }
      if (PlayableChar->ActualPlayerStatus == EPlayerState::Default)
      {
        ActiveAbilityEvent.Broadcast(true);
      }
    });

  SetupBinding(FName("ActiveAbility"), IE_Released, [PlayableChar, this]()
    {
      if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
      {
        return;
      }
      /*   if (PlayableChar->ActualPlayerStatus == EPlayerState::Default || PlayableChar->ActualPlayerStatus == EPlayerState::LaunchingAbility)
         {
         }*/
      ActiveAbilityEvent.Broadcast(false);
    });

}


void ALunarPunkPlayerController::DeactivateMoveAllTurrets()
{

  InputComponent->RemoveActionBinding(FName("TurretAIMovement"), IE_Pressed);

}



void ALunarPunkPlayerController::ReactivateMoveAllTurrets(APlayableCharacter* PlayableChar)
{

  SetupBinding(FName("TurretAIMovement"), IE_Pressed, [PlayableChar, this]()
    {
      if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
      {
        return;
      }

      if (this->bCanMoveTurrets)
      {
        AutoMoveTurretsEvent.Broadcast();

        if (this->ControlMoveTurretsCooldownTime > 0)
        {
          this->bCanMoveTurrets = false;
          GetWorld()->GetTimerManager().SetTimer(TimerHandler, this, &ALunarPunkPlayerController::ResetMoveTurretsCooldown, ControlMoveTurretsCooldownTime, false, ControlMoveTurretsCooldownTime);
        }
      }
    });
}


void ALunarPunkPlayerController::ReactivateSparkleAbbility(APlayableCharacter* PlayableChar)
{
    SetupBinding(FName("ThrowSparkle"), IE_Pressed, [PlayableChar, this]()
        {
            if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
            {
                return;
            }
            if (PlayableChar->ActualPlayerStatus == EPlayerState::Default)
            {
                PlayableChar->ActualPlayerStatus = EPlayerState::LaunchingAbility;
                ThrowSparkleAbility.Broadcast();
            }
        });



    SetupBinding(FName("ThrowSparkle"), IE_Released, [PlayableChar, this]()
        {
            if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
            {
                return;
            }

            StopThrowingSparkleAbility.Broadcast();

            if (PlayableChar->ActualPlayerStatus == EPlayerState::LaunchingAbility)
            {
                PlayableChar->ActualPlayerStatus = EPlayerState::Default;
            }
        });

}

void ALunarPunkPlayerController::ReactivateShieldAbbility(APlayableCharacter* PlayableChar)
{

    SetupBinding(FName("ThrowShield"), IE_Pressed, [PlayableChar, this]()
        {
            if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
            {
                return;
            }
            if (PlayableChar->ActualPlayerStatus == EPlayerState::Default)
            {
                PlayableChar->ActualPlayerStatus = EPlayerState::LaunchingAbility;
                ThrowShieldAbility.Broadcast();
            }
        });

    SetupBinding(FName("ThrowShield"), IE_Released, [PlayableChar, this]()
        {
            if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
            {
                return;
            }
            StopThrowingShieldAbility.Broadcast();
            if (PlayableChar->ActualPlayerStatus == EPlayerState::LaunchingAbility)
            {
                PlayableChar->ActualPlayerStatus = EPlayerState::Default;
            }
        });
}



void ALunarPunkPlayerController::ReactivateGrenadeAbbility(APlayableCharacter* PlayableChar)
{
    SetupBinding(FName("ThrowGrenade"), IE_Pressed, [PlayableChar, this]()
        {
            if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
            {
                return;
            }
            if (PlayableChar->ActualPlayerStatus == EPlayerState::Default)
            {
                PlayableChar->ActualPlayerStatus = EPlayerState::LaunchingAbility;
                ThrowGrenadeAbility.Broadcast();
            }
        });

    SetupBinding(FName("ThrowGrenade"), IE_Released, [PlayableChar, this]()
        {
            if (PlayableChar->ActualPlayerStatus == EPlayerState::Cinematic)
            {
                return;
            }

            StopThrowingGrenadeAbility.Broadcast();

            if (PlayableChar->ActualPlayerStatus == EPlayerState::LaunchingAbility)
            {
                PlayableChar->ActualPlayerStatus = EPlayerState::Default;
            }
        });
}


void ALunarPunkPlayerController::SetControllerActiveThumbStick(float value)
{
  if (bMouseActive)
  {
    if (0.15 < value || value < -0.15)
    {
      SetMouseIsController(false);
    }
  }
}

// Change between mouse and Gamepad controller.
void ALunarPunkPlayerController::SetMouseIsController(bool bIsActive)
{
    bMouseActive = bIsActive;
    bShowMouseCursor = bIsActive;

    float MousePosX;
    float MousePosY;
    

    if (GetMousePosition(MousePosX, MousePosY))
    {
      SetMouseLocation(MousePosX, MousePosY);
    }
}