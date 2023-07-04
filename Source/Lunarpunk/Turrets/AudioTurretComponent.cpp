// Fill out your copyright notice in the Description page of Project Settings.


#include "Turrets/AudioTurretComponent.h"
#include "Turret.h"
#include "AI/MovementTurretComponent.h"
#include "HealthSystem/HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"

// Sets default values for this component's properties
UAudioTurretComponent::UAudioTurretComponent()
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;

  // ...
}


// Called when the game starts
void UAudioTurretComponent::BeginPlay()
{
  Super::BeginPlay();

  // ...

}


// Called every frame
void UAudioTurretComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // ...
}



void UAudioTurretComponent::Initialize()
{
  TurretOwner = Cast<ATurret>(GetOwner());
  GameMode = Cast<ALunarPunkGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
  AudioManager = GameMode->AudioManager;
  TurretOwner->ShootEvent.AddDynamic(this, &UAudioTurretComponent::PlayShootEvent);
  TurretOwner->Repairing.AddDynamic(this, &UAudioTurretComponent::PlayRepairEvent);
  TurretOwner->StopRepairing.AddDynamic(this, &UAudioTurretComponent::StopRepairEvent);
  TurretOwner->HealthComponent->DamageReceived.AddDynamic(this, &UAudioTurretComponent::PlayDamageEvent);
  //TurretOwner->TurretReactivated.AddDynamic(this, &UAudioTurretComponent::PlayReactivateEvent);
  TurretOwner->TurretLeft.AddDynamic(this, &UAudioTurretComponent::PlayLeaveTurretEvent);
  TurretOwner->TurretPickedUp.AddDynamic(this, &UAudioTurretComponent::PlayPickUpEvent);
  TurretOwner->TurretDeath.AddDynamic(this, &UAudioTurretComponent::PlayDeathTurretEvent);
  TurretOwner->UpgradeAdded.AddDynamic(this, &UAudioTurretComponent::PlayUpgradeAddedEvent);
  TurretOwner->TurretMovementComponent->TurretMovingEvent.AddDynamic(this, &UAudioTurretComponent::PlayTurretMovementeEvent);
  TurretOwner->TurretMovementComponent->TurretStopMovingEvent.AddDynamic(this, &UAudioTurretComponent::StopTurretMovementeEvent);
}
void UAudioTurretComponent::PlayTurretMovementeEvent()
{
  if (MovementInstance == nullptr || !MovementInstance->IsPlaying())
  {
    MovementInstance = UFMODBlueprintStatics::PlayEventAttached(TurretMovementEvent, GetOwner()->GetRootComponent(), NAME_None, GetOwner()->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
    //RepairInstance->SetVolume(AudioManager->MasterVolume);
  }
}

void UAudioTurretComponent::StopTurretMovementeEvent()
{
  if (IsValid(MovementInstance))
  {

    MovementInstance->Stop();
    MovementInstance->Release();
  }
}
void UAudioTurretComponent::PlayShootEvent()
{
  UFMODAudioComponent* ShootAudio = UFMODBlueprintStatics::PlayEventAttached(ShootAudioEvent, GetOwner()->GetRootComponent(), NAME_None, GetOwner()->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);

  //ShootAudio->SetVolume(AudioManager->MasterVolume);
}

void UAudioTurretComponent::PlayRepairEvent()
{
  if (RepairInstance == nullptr || !RepairInstance->IsPlaying())
  {
    RepairInstance = UFMODBlueprintStatics::PlayEventAttached(RepairAudioEvent, GetOwner()->GetRootComponent(), NAME_None, GetOwner()->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
    //RepairInstance->SetVolume(AudioManager->MasterVolume);
  }
}

void UAudioTurretComponent::StopRepairEvent()
{
  if (IsValid(RepairInstance))
  {
    RepairInstance->Stop();
    RepairInstance->Release();

    if (TurretOwner->HealthComponent->GetActualHealth() == TurretOwner->HealthComponent->GetMaxHealth())
    {
      ReactivateInstance = UFMODBlueprintStatics::PlayEventAttached(ReactivateAudioEvent, GetOwner()->GetRootComponent(), NAME_None, GetOwner()->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
      //Reactivate->SetVolume(AudioManager->MasterVolume);
      //Reactivate->Play();
    }
  }
}

void UAudioTurretComponent::PlayPickUpEvent()
{
  PickUpEventInstance = UFMODBlueprintStatics::PlayEventAttached(PickUpAudioEvent, GetOwner()->GetRootComponent(), NAME_None, GetOwner()->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
}

void UAudioTurretComponent::PlayDamageEvent(float Damage)
{
  DamageEventInstance = UFMODBlueprintStatics::PlayEventAttached(DamageAudioEvent, GetOwner()->GetRootComponent(), NAME_None, GetOwner()->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
}

void UAudioTurretComponent::PlayReactivateEvent()
{
  ReactivateEventInstance = UFMODBlueprintStatics::PlayEventAttached(ReactivateAudioEvent, GetOwner()->GetRootComponent(), NAME_None, GetOwner()->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
}

void UAudioTurretComponent::PlayLeaveTurretEvent()
{
  LeaveTurretInstance = UFMODBlueprintStatics::PlayEventAttached(LeaveTurretAudioEvent, GetOwner()->GetRootComponent(), NAME_None, GetOwner()->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
}

void UAudioTurretComponent::PlayDeathTurretEvent()
{
  DeathAudioInstance = UFMODBlueprintStatics::PlayEventAttached(DeathAudioEvent, GetOwner()->GetRootComponent(), NAME_None, GetOwner()->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
}

void UAudioTurretComponent::StopUpgradeAudio()
{
  if (IsValid(UpgradeAddedAudioInstance))
  {
    UpgradeAddedAudioInstance->Stop();
    UpgradeAddedAudioInstance->Release();
  }
}

void UAudioTurretComponent::PlayUpgradeAddedEvent(EUpgradeType UpgradeType)
{
  StopUpgradeAudio();
  switch (UpgradeType)
  {
    case EUpgradeType::Cadence:
      UpgradeAddedAudioInstance = UFMODBlueprintStatics::PlayEventAttached(CadenceUpgradeAudioEvent, GetOwner()->GetRootComponent(), NAME_None, GetOwner()->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
      break;
    case EUpgradeType::Power:
      UpgradeAddedAudioInstance = UFMODBlueprintStatics::PlayEventAttached(DamageUpgradeAudioEvent, GetOwner()->GetRootComponent(), NAME_None, GetOwner()->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
      break;
    case EUpgradeType::Range:
      UpgradeAddedAudioInstance = UFMODBlueprintStatics::PlayEventAttached(RangeUpgradeAudioEvent, GetOwner()->GetRootComponent(), NAME_None, GetOwner()->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
      break;
    case EUpgradeType::Health:
      UpgradeAddedAudioInstance = UFMODBlueprintStatics::PlayEventAttached(HealthUpgradeAudioEvent, GetOwner()->GetRootComponent(), NAME_None, GetOwner()->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
      break;
    default:
      break;
  }

}


void UAudioTurretComponent::BeginDestroy()
{
  StopAudio();
  Super::BeginDestroy();
}


void UAudioTurretComponent::StopAudio()
{
  StopUpgradeAudio();
  StopTurretMovementeEvent();

  if (IsValid(RepairInstance))
  {
    RepairInstance->Stop();
    RepairInstance->Release();
  }

  if (IsValid(DeathAudioInstance))
  {
    DeathAudioInstance->Stop();
    DeathAudioInstance->Release();
  }

  if (IsValid(LeaveTurretInstance))
  {
    LeaveTurretInstance->Stop();
    LeaveTurretInstance->Release();
  }

  if (IsValid(ReactivateEventInstance))
  {
    ReactivateEventInstance->Stop();
    ReactivateEventInstance->Release();
  }

  if (IsValid(DamageEventInstance))
  {
    DamageEventInstance->Stop();
    DamageEventInstance->Release();
  }

  if (IsValid(PickUpEventInstance))
  {
    PickUpEventInstance->Stop();
    PickUpEventInstance->Release();
  }
}

void UAudioTurretComponent::PauseAudio()
{
  if (IsValid(ReactivateEventInstance))
  {
    ReactivateEventInstance->SetPaused(true);
  }
  if (IsValid(MovementInstance))
  {
    MovementInstance->SetPaused(true);
  }
}

void UAudioTurretComponent::RestartAudio()
{
  if (IsValid(ReactivateEventInstance))
  {
    ReactivateEventInstance->SetPaused(false);
  }
  if (IsValid(ReactivateEventInstance))
  {
    MovementInstance->SetPaused(false);
  }
}
