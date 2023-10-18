// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayableCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "HealthSystem/HealthComponent.h"
#include "Granade.h"
#include "RewardsAndUpgrades/GetRewardsComponent.h"
#include "HealthSystem/DamageComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "LunarPunkPlayerController.h"
#include "Abilities/DestroyPortalAbilityComponent.h"
#include "Abilities/GranadeAbilityComponent.h"
#include "Abilities/ShieldAbilityComponent.h"
#include "Abilities/SparkAbilityComponent.h"
#include "Abilities/TrapActivatorAbilityComponent.h"
#include "Abilities/ShootAbilityComponent.h"
#include "Managers/HUDManager.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "Managers/GameFrameworks/LunarPunkGameState.h"
#include "ProfilingDebugging/CookStats.h"
#include "Turrets/TurretInteractionComponent.h"
#include "AimAssist.h"
#include "AudioPlayerComponent.h"

// Sets default values
APlayableCharacter::APlayableCharacter()
{
  // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

  InteractionCollider = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionCollider"));
  InteractionCollider->InitSphereRadius(InteractionColliderRadius);
  InteractionCollider->SetupAttachment(RootComponent);

  AbilityCollider = CreateDefaultSubobject<USphereComponent>(TEXT("AbilityCollider"));
  AbilityCollider->InitSphereRadius(AbilityColliderRadius);
  AbilityCollider->SetupAttachment(RootComponent);

  DamageComponent = CreateDefaultSubobject<UDamageComponent>(TEXT("DamageComponent"));

  HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

  RewardsComponent = CreateDefaultSubobject<UGetRewardsComponent>(TEXT("RewardsComponent"));

  //AudioComponent = CreateDefaultSubobject<UAudioComponent>(FName("ShootAudioComponent"));

  PlayerAudioComponent = CreateDefaultSubobject<UAudioPlayerComponent>(FName("PlayerAudioComponent"));

  DestroyPortalAbilityComponent = CreateDefaultSubobject<UDestroyPortalAbilityComponent>(FName("DestroyPortalAbilityComponent "));

  GrenadeAbilityComponent = CreateDefaultSubobject<UGranadeAbilityComponent>(FName("GrenadeAbilityComponent"));

  ShieldAbilityComponent = CreateDefaultSubobject<UShieldAbilityComponent >(FName("ShieldAbilityComponent"));

  SparkAbilityComponent = CreateDefaultSubobject<USparkAbilityComponent>(FName("SparkAbilityComponent"));

  TrapActivatorAbilityComponent = CreateDefaultSubobject<UTrapActivatorAbilityComponent>(FName("TrapActivatorAbilityComponent"));

  ShootAbilityComponent = CreateDefaultSubobject<UShootAbilityComponent>(FName("ShootAbilityComponent"));

  AimAssistComponent = CreateDefaultSubobject<UAimAssist>(FName("AimAssistComponent"));

  TurretInteractionComponent = CreateDefaultSubobject<UTurretInteractionComponent>(FName("TurretInteractionComponent"));

  MeshShield = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMeshComponent"));
  MeshShield->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
  MeshShield->SetVisibility(false);
  MeshShield->SetupAttachment(RootComponent);

  ShootEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ShootEffectComponent"));
  RayShieldEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RayShieldEffectComponent"));
  RayDestroyPortalEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RayDestroyPortalEffectComponent"));

  AbilityEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AbilityEffectComponent"));
  GranadeEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GranadeEffectComponent"));
  
  AbilityEffectComponent->SetupAttachment(RootComponent);
  GranadeEffectComponent->SetupAttachment(AbilityEffectComponent);

  ShieldEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ShieldEffect"));
  ShieldEffect->SetupAttachment(AbilityEffectComponent);
  ShieldEffect->AutoAttachLocationRule = EAttachmentRule::SnapToTarget;
}

void APlayableCharacter::Initialize(ALunarPunkPlayerController* _LPPLayerController)
{
  ActualPlayerStatus = EPlayerState::Default;


  // --- Input menus --- //
  LPPlayerController = _LPPLayerController;
  LPPlayerController->Initialize(this);
  FInputModeGameAndUI InputMode;
  InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
  InputMode.SetHideCursorDuringCapture(false);

  GEngine->GetGamePlayer(GetWorld(), 0)->PlayerController->SetInputMode(InputMode);
  //--- End --- //

  GameMode = Cast<ALunarPunkGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
  if (IsValid(GameMode))
  {
    
      ALunarPunkGameState* LunarPunkGameState = Cast<ALunarPunkGameState>(UGameplayStatics::GetGameState(GetWorld()));
      if (IsValid(LunarPunkGameState))
      {
        LunarPunkGameState->OpenZone.AddDynamic(HealthComponent, &UHealthComponent::RestoreAllHealth);
      }

      GameMode->Initialize();
      HealthComponent->DamageReceived.AddDynamic(GameMode->HUDManager, &AHUDManager::ShowDamageEffectInHUD);
      HealthComponent->UpdateHealth.AddDynamic(GameMode->HUDManager, &AHUDManager::UpdatePlayerLifeInHUD);
      HealthComponent->ActorDeath.AddDynamic(LPPlayerController, &ALunarPunkPlayerController::DeactivateMovement);
      LPPlayerController->GodModeEvent.AddDynamic(GameMode, &ALunarPunkGameMode::GodMode);
      LPPlayerController->GodModeTurretsEvent.AddDynamic(GameMode, &ALunarPunkGameMode::GodModeTurrets);

      DestroyPortalAbilityComponent->Initialize(LPPlayerController, this);
      ShieldAbilityComponent->Initialize(LPPlayerController, this);
      ShootAbilityComponent->Initialize(LPPlayerController, this);
      GrenadeAbilityComponent->Initialize(LPPlayerController, this);
      SparkAbilityComponent->Initialize(LPPlayerController, this);
      TrapActivatorAbilityComponent->Initialize(LPPlayerController, this);
      TurretInteractionComponent->Initialize();
      //Select Grenade abbility by default
      _LPPLayerController->ThrowGrenadeAbility.Broadcast();
      // INIT AFTER CONTROLLER
      PlayerAudioComponent->Initialize(LPPlayerController);

      AbilityCollider->SetSphereRadius(AbilityColliderRadius);

      MeshShield->SetVisibility(false);
      if (IsValid(AbilityEffectComponent))
      {
        AbilityEffectComponent->DeactivateImmediate();
        AbilityEffectComponent->SetRelativeLocation(FVector(0, 0, 0));
      }

      TurretPositionIndicator = Cast<UDecalComponent>(GetDefaultSubobjectByName("TurretPositioningIndicator"));

      AbilityZoneDecalRef = Cast<UDecalComponent>(GetDefaultSubobjectByName("AbilityZoneDecal"));
      AbilityZoneDecalRef->SetVisibility(false);
  }
}

void APlayableCharacter::PostInitializeComponents()
{
  Super::PostInitializeComponents();
}

// Called when the game starts or when spawned
void APlayableCharacter::BeginPlay()
{
  Super::BeginPlay();

  Initialize(Cast<ALunarPunkPlayerController>(Controller));
}

// Called every frame
void APlayableCharacter::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
  Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UHealthComponent* APlayableCharacter::GetHealthComponent()
{
  HealthComponent = Cast<UHealthComponent>(GetComponentByClass(UHealthComponent::StaticClass()));
  if (IsValid(HealthComponent))
  {
    return HealthComponent;
  }

  return nullptr;
}

void APlayableCharacter::RestartShieldEffect()
{
  GetWorld()->GetTimerManager().ClearTimer(ShieldEffectTimer);
  GetWorld()->GetTimerManager().SetTimer(ShieldEffectTimer, this, &APlayableCharacter::DeactivateShieldEffectt, ShieldEffectDuration, true);
  ShieldEffect->Activate();
}


void APlayableCharacter::DeactivateShieldEffectt()
{
  ShieldEffect->Deactivate();
}