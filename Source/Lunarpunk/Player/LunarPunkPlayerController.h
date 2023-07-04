// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Delegates/Delegate.h"
#include "LunarPunkPlayerController.generated.h"

class APlayableCharacter;
class ACameraManager;

// Event called when the player controller is initialized
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInitialize);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerShoot, bool, IsPressed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDestroyPortalShoot, bool, IsPressed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSkipCinematic, bool, IsPressed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActiveAbility, bool, IsPressed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSkipTutorial);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGodMode);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGodModeTurrets);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRefillUpgrades);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGetAllPlates);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FKillEnemies);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPositionTurret);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTreatTurret);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopTreatingTurret);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpgradeTurret);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSelectNextUpgrade);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSelectPreviousUpgrade);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSelectNextAbility);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSelectPreviousAbility);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAutoMoveTurrets);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FThrowGrenadeAbility);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopThrowingGrenadeAbility);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FThrowShieldAbility);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopThrowingShieldAbility);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FThrowActivateTrapsAbility);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopThrowingActivateTrapsAbility);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FThrowSparkleAbility);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopThrowingSparkleAbility);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerIsMoving);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerMovementStopped);

/**
 *
 */
UCLASS()
class LUNARPUNK_API ALunarPunkPlayerController : public APlayerController
{
  GENERATED_BODY()

public:

  virtual void BeginPlay() override;

  UFUNCTION()
    void Initialize(APlayableCharacter* _PlayableCharacter);

  // Called every frame
  virtual void Tick(float DeltaTime) override;


  UFUNCTION(BlueprintCallable)
    void InitializeInput(APlayableCharacter* PlayableChar, bool bIsActive);


  UFUNCTION(BlueprintCallable)
    void RemoveInput();

  UFUNCTION(BlueprintCallable, Category = "PlayerController")
    void SetCutsceneMode(bool bIsEnabled);

  // Called to bind functionality to input
// Input: UInputComponent* PlayerInputComponent 
  virtual void SetupInputComponent() override;

  // Function to handle forward and backward movement 
  // Input: float value - movement quantity (1, -1)
  UFUNCTION()
    void MoveForward(float value);

  // Function to handle the movement to the right and left 
  // Input: float value - movement quantity (1, -1)
  UFUNCTION()
    void MoveRight(float value);

  // Function to handle rotation for GamePad in X value
  // Input: float value - movement quantity (1, -1)
  UFUNCTION()
    void TurnX(float value);

  // Function to handle rotation for GamePad in Y value
  // Input: float value - movement quantity (1, -1)
  UFUNCTION()
    void TurnY(float value);

  // Function to rotate the player to the mouse location
  UFUNCTION()
    void RotateTowardsMouse();
  // Function to rotate the player with GamePad
  UFUNCTION()
    void RotateGamePad();

  // Functions called when Directionalpad pressed
  UFUNCTION(BlueprintCallable, Category = "PlayerController")
    void UpDirectionalPad();

  UFUNCTION(BlueprintCallable, Category = "PlayerController")
    void RightDirectionalPad();

  UFUNCTION(BlueprintCallable, Category = "PlayerController")
    void DownDirectionalPad();

  UFUNCTION(BlueprintCallable, Category = "PlayerController")
    void LeftDirectionalPad();

  UFUNCTION(BlueprintCallable, Category = "PlayerController")
    void DeactivateAbbilities();

  UFUNCTION(BlueprintCallable, Category = "PlayerController")
    void DeactivateShieldAbbility();

  UFUNCTION(BlueprintCallable, Category = "PlayerController")
    void DeactivateGrenadeAbbility();

  UFUNCTION(BlueprintCallable, Category = "PlayerController")
    void DeactivateSparkleAbbility();

  UFUNCTION(BlueprintCallable, Category = "PlayerController")
    void ReactivateAbbilities(APlayableCharacter* PlayableChar);

  UFUNCTION(BlueprintCallable, Category = "PlayerController")
    void ReactivateShieldAbbility(APlayableCharacter* PlayableChar);

  UFUNCTION(BlueprintCallable, Category = "PlayerController")
    void ReactivateGrenadeAbbility(APlayableCharacter* PlayableChar);

  UFUNCTION(BlueprintCallable, Category = "PlayerController")
    void ReactivateSparkleAbbility(APlayableCharacter* PlayableChar);

  UFUNCTION(BlueprintCallable, Category = "PlayerController")
    void DeactivateMoveAllTurrets();

  UFUNCTION(BlueprintCallable, Category = "PlayerController")
    void ReactivateMoveAllTurrets(APlayableCharacter* PlayableChar);

  UFUNCTION(BlueprintCallable, Category = "Playercontroller")
    void AdjustControllerToCamera();

  UFUNCTION(BlueprintCallable, Category = "Playercontroller")
    void SetMouseIsController(bool bIsActive);

  UFUNCTION()
  void SetControllerActiveThumbStick(float value);

  FVector GetCameraRightVector() const { return CameraRight; }

  FVector GetCameraForwardVector() const { return CameraForward; }


  UPROPERTY()
    class APlayableCharacter* PlayableCharacter;

  UPROPERTY()
    class ACameraManager* CameraManager;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  bool bMouseActive;

  UPROPERTY()
    float TurnXValue = 0.0f;

  UPROPERTY()
    float TurnYValue = 0.0f;

  //Rotation speed of the player towards gamepad
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RotationSpeed = 1.0;

  //Aceleration of the player.
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AcelPlayer = 0.1;

  UPROPERTY()
    bool bInputIsActive = false;

  /**
   * Events
   */

  UPROPERTY()
    FInitialize InitializeEvent;

  UPROPERTY()
    FPlayerShoot ShootEvent;

  UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable)
    FActiveAbility ActiveAbilityEvent;

  UPROPERTY()
    FThrowGrenadeAbility ThrowGrenadeAbility;

  UPROPERTY()
    FStopThrowingGrenadeAbility StopThrowingGrenadeAbility;

  UPROPERTY()
    FThrowShieldAbility ThrowShieldAbility;

  UPROPERTY()
    FStopThrowingShieldAbility StopThrowingShieldAbility;

  UPROPERTY()
    FThrowActivateTrapsAbility  ThrowActivateTrapsAbility;

  UPROPERTY()
    FStopThrowingActivateTrapsAbility StopThrowingActivateTrapsAbility;

  UPROPERTY()
    FThrowSparkleAbility ThrowSparkleAbility;

  UPROPERTY()
    FStopThrowingSparkleAbility StopThrowingSparkleAbility;

  UPROPERTY()
    FGodMode GodModeEvent;

  UPROPERTY()
    FGodMode GodModeTurretsEvent;

  UPROPERTY()
    FGetAllPlates GetAllPlatesEvent;

  UPROPERTY()
    FRefillUpgrades RefillUpgradesEvent;
  
  UPROPERTY()
    FKillEnemies KillEnemiesEvent;

  UPROPERTY()
    FPositionTurret PositionTurretEvent;

  UPROPERTY()
    FTreatTurret TreatTurretEvent;

  UPROPERTY()
    FStopTreatingTurret  StopTreatingTurretEvent;

  UPROPERTY()
    FUpgradeTurret UpgradeTurretEvent;

  UPROPERTY()
    FSelectNextUpgrade SelectNextUpgradeEvent;

  UPROPERTY()
    FSelectPreviousUpgrade SelectPreviousUpgradeEvent;

  UPROPERTY()
    FSelectNextAbility SelectNextAbilityEvent;

  UPROPERTY()
    FSelectPreviousAbility SelectPreviousAbilityEvent;

  UPROPERTY()
    FAutoMoveTurrets AutoMoveTurretsEvent;

  UPROPERTY()
    FDestroyPortalShoot DestroyPortalShootButton1Event;

  UPROPERTY()
    FDestroyPortalShoot DestroyPortalShootButton2Event;

  UPROPERTY(BlueprintAssignable)
    FSkipCinematic SkipCinematic;

  UPROPERTY(BlueprintAssignable)
  FSkipTutorial SkipTutorial;

  UPROPERTY(BlueprintAssignable)
    FPlayerIsMoving PlayerIsMovingEvent;

  UPROPERTY(BlueprintAssignable)
    FPlayerMovementStopped PlayerMovementStoppedEvent;

  UPROPERTY(VisibleAnywhere)
    bool IsDeactivated;

  UPROPERTY(EditAnywhere)
    float ControlMoveTurretsCooldownTime = 0.2;

private:

  template<typename FunctorType, typename... VarTypes>
  void SetupBinding(FName InputBindingName, const EInputEvent InputKeyEvent, FunctorType&& InFunctor);

  void ResetMoveTurretsCooldown();

private:

  UPROPERTY()
    bool bCanMoveTurrets = true;

  UPROPERTY()
    FTimerHandle TimerHandler;

  //Vectors and corrections for the player movement depending on the Yaw CameraAngle1 of the camera.
  FVector CameraForward;
  FVector CameraRight;
  float XAxisCorrection;
  float YAxisCorrection;
  float XAxisCorrection2;
  float YAxisCorrection2;
  float CameraYawAngle;

  //Handles the direction and speed of the player
  float LastForward = 0.0;
  float LastRight = 0.0;
  float CurrentVel = 0;
  float MaxVel = 1;
  FRotator PlayerRotation;
  FRotator GoalRotation;

};

template <typename FunctorType, typename ... VarTypes>
void ALunarPunkPlayerController::SetupBinding(FName InputBindingName, const EInputEvent InputKeyEvent,
  FunctorType&& InFunctor)
{
  FInputActionBinding InputAction(InputBindingName, InputKeyEvent);

  InputAction.ActionDelegate.GetDelegateForManualSet().BindLambda(InFunctor);

  InputComponent->AddActionBinding(InputAction);

}
