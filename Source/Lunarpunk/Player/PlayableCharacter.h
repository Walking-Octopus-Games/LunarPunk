// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "PlayableCharacter.generated.h"


class UHealthComponent;
class UNiagaraComponent;
class ALunarPunkPlayerController;
class UTurretInteractionComponent;
class UShootAbilityComponent;
class USparkAbilityComponent;
class UShieldAbilityComponent;
class UTrapActivatorAbilityComponent;
class UDestroyPortalAbilityComponent;
class UGranadeAbilityComponent;
class UAimAssist;
class UAudioPlayerComponent;
class UStaticMeshComponent;
class UDecalComponent;

UENUM()
enum class EPlayerState : uint8
{
  LaunchingAbility,
  Shooting,
  TurretInteraction,
  PositioningTurret,
  DestroyingPortal,
  Cinematic,
  Default
};

UCLASS()
class LUNARPUNK_API APlayableCharacter : public ACharacter
{
  GENERATED_BODY()

public:
  // Sets default values for this character's properties
  APlayableCharacter();

  // Called every frame
  virtual void Tick(float DeltaTime) override;

  // Called to bind functionality to input
  // Input: UInputComponent* PlayerInputComponent 
  virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


  UPROPERTY(EditAnywhere, Category = Interaction)
    class USphereComponent* InteractionCollider = nullptr;

  UPROPERTY(EditAnywhere, category = Interaction)
    float InteractionColliderRadius = 200.0f;

  UPROPERTY(EditAnywhere, Category = "Abilities")
    USphereComponent* AbilityCollider = nullptr;

  UPROPERTY(EditAnywhere, category = "Abilities")
    float AbilityColliderRadius = 500.0f;

  UPROPERTY(EditAnywhere)
      UDecalComponent* AbilityZoneDecalRef;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EPlayerState ActualPlayerStatus = EPlayerState::Default;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UDestroyPortalAbilityComponent* DestroyPortalAbilityComponent;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UGranadeAbilityComponent* GrenadeAbilityComponent;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USparkAbilityComponent* SparkAbilityComponent;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTrapActivatorAbilityComponent* TrapActivatorAbilityComponent;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UShieldAbilityComponent* ShieldAbilityComponent;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UShootAbilityComponent* ShootAbilityComponent;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTurretInteractionComponent* TurretInteractionComponent;

  /*Reference point where the shoot starts*/
  UPROPERTY(BlueprintReadWrite, Category = Gameplay)
    UStaticMeshComponent* StartShootReferencePoint;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    UAimAssist* AimAssistComponent;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Shot")
    UNiagaraComponent* ShootEffectComponent;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
    class UHealthComponent* HealthComponent;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
    class UDamageComponent* DamageComponent;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
    class UGetRewardsComponent* RewardsComponent;

  UPROPERTY()
    ALunarPunkPlayerController* LPPlayerController;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class ACameraActor* CameraActor;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAudioPlayerComponent* PlayerAudioComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMeshComponent* MeshShield;

  // Component of the niagara shoot effect
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
    class UNiagaraComponent* RayShieldEffectComponent;

  // Component of the niagara shoot effect
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
    class UNiagaraComponent* RayDestroyPortalEffectComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UNiagaraComponent* AbilityEffectComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
      UNiagaraComponent* GranadeEffectComponent;

  UPROPERTY()
    UDecalComponent* TurretPositionIndicator;

  UPROPERTY()
    class ALunarPunkGameMode* GameMode;

  UFUNCTION()
    void Initialize(ALunarPunkPlayerController* _LPPLayerController);

  UFUNCTION()
    UAimAssist* GetAimAssistComponent()
  {
    return AimAssistComponent;
  }

  UFUNCTION()
    UHealthComponent* GetHealthComponent();

  virtual void PostInitializeComponents() override;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilities | Effects")
    UParticleSystemComponent* ShieldEffect;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilities | Effects")
    float ShieldEffectDuration=2.f;

  /* Handle to manage the timer */
  UPROPERTY()
    FTimerHandle ShieldEffectTimer;


  void RestartShieldEffect();

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

private:

  void DeactivateShieldEffectt();
};
