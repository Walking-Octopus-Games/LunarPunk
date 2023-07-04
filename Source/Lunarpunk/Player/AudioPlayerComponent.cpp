// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AudioPlayerComponent.h"

#include "FMODBlueprintStatics.h"
#include "LunarPunkPlayerController.h"
#include "PlayableCharacter.h"
#include "Abilities/DestroyPortalAbilityComponent.h"
#include "Abilities/GranadeAbilityComponent.h"
#include "Abilities/ShieldAbilityComponent.h"
#include "Abilities/ShootAbilityComponent.h"
#include "Abilities/SparkAbilityComponent.h"
#include "HealthSystem/HealthComponent.h"

// Sets default values for this component's properties
UAudioPlayerComponent::UAudioPlayerComponent()
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;

  // ...
}


// Called when the game starts
void UAudioPlayerComponent::BeginPlay()
{
  Super::BeginPlay();

  // ...

}


// Called every frame
void UAudioPlayerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // ...
}



void UAudioPlayerComponent::Initialize(ALunarPunkPlayerController* LunarPunkPlayerController)
{

  PlayableCharacter = LunarPunkPlayerController->PlayableCharacter;
  PlayableCharacter->ShootAbilityComponent->PlayerShooting.AddDynamic(this, &UAudioPlayerComponent::PlayShootEvent);
  PlayableCharacter->HealthComponent->DamageReceived.AddDynamic(this, &UAudioPlayerComponent::PlayDamageEvent);
  PlayableCharacter->HealthComponent->HealthRestored.AddDynamic(this, &UAudioPlayerComponent::PlayHealthResetEvent);
  PlayableCharacter->HealthComponent->ActorDeath.AddDynamic(this, &UAudioPlayerComponent::PlayDeathEvent);
  PlayableCharacter->DestroyPortalAbilityComponent->SetAvailableToDestroyPortal.AddDynamic(this, &UAudioPlayerComponent::PlaySuperShootEvent);
  PlayableCharacter->DestroyPortalAbilityComponent->DestroyPortalEvent.AddDynamic(this, &UAudioPlayerComponent::PlayEndSuperShootEvent);
  PlayableCharacter->SparkAbilityComponent->ThrowSparkleEvent.AddDynamic(this, &UAudioPlayerComponent::PlaySparkleAudio);
  PlayableCharacter->GrenadeAbilityComponent->ThrowGrenadeEvent.AddDynamic(this, &UAudioPlayerComponent::PlayGrenadaAudio);
  PlayableCharacter->ShieldAbilityComponent->ThrowShieldEvent.AddDynamic(this, &UAudioPlayerComponent::PlayShieldAudio);
  PlayableCharacter->ShieldAbilityComponent->DeactivateShieldEvent.AddDynamic(this, &UAudioPlayerComponent::DeactivateShieldAudio);

  PlayableCharacter->ShieldAbilityComponent->AbilityNotThrown.AddDynamic(this, &UAudioPlayerComponent::PlayAbilityNotAvailable);
  PlayableCharacter->GrenadeAbilityComponent->AbilityNotThrown.AddDynamic(this, &UAudioPlayerComponent::PlayAbilityNotAvailable);
  PlayableCharacter->SparkAbilityComponent->AbilityNotThrown.AddDynamic(this, &UAudioPlayerComponent::PlayAbilityNotAvailable);

}

void UAudioPlayerComponent::PlayDamageEvent(float Damage)
{
  DamageReceiveInstance = UFMODBlueprintStatics::PlayEventAttached(DamageAudioEvent, PlayableCharacter->GetRootComponent(), NAME_None, PlayableCharacter->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
}

void UAudioPlayerComponent::PlayShootEvent()
{
  ShootAudioInstance = UFMODBlueprintStatics::PlayEventAttached(ShootAudioEvent, PlayableCharacter->GetRootComponent(), NAME_None, PlayableCharacter->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
}

void UAudioPlayerComponent::PlaySuperShootEvent(bool bIsAvailable, float MaxTime)
{
  if (bIsAvailable)
  {
    SuperShotAudioInstance = UFMODBlueprintStatics::PlayEventAttached(SuperShotAudioEvent, PlayableCharacter->GetRootComponent(), NAME_None, PlayableCharacter->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
  }
  else
  {
    if (IsValid(SuperShotAudioInstance))
    {
      SuperShotAudioInstance->StudioInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
    }
  }
}

//void UAudioPlayerComponent::PlayIsMovingEvent()
//{
//  if (!IsValid(PlayerStepsAudioInstance) || !PlayerStepsAudioInstance->IsPlaying())
//  {
//    PlayerStepsAudioInstance = UFMODBlueprintStatics::PlayEventAttached(PlayerStepsAudioEvent, PlayableCharacter->GetRootComponent(), NAME_None, PlayableCharacter->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
//  }
//}


void UAudioPlayerComponent::BeginDestroy()
{
  StopAudio();

  Super::BeginDestroy();
}

void UAudioPlayerComponent::PlayStopMovingEvent()
{
  if (IsValid(PlayerStepsAudioInstance))
  {
    PlayerStepsAudioInstance->StudioInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
    PlayerStepsAudioInstance->Release();
  }
}



void UAudioPlayerComponent::PlayEndSuperShootEvent()
{
  if (IsValid(SuperShotAudioInstance))
  {
    SuperShotAudioInstance->StudioInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
  }
  EndSuperShotInstance = UFMODBlueprintStatics::PlayEventAttached(EndSuperShotAudioEvent, PlayableCharacter->GetRootComponent(), NAME_None, PlayableCharacter->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);

}

void UAudioPlayerComponent::PlaySparkleAudio()
{
  SparkleAudioInstance = UFMODBlueprintStatics::PlayEventAttached(SparkleAudioEvent, PlayableCharacter->GetRootComponent(), NAME_None, PlayableCharacter->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
}

void UAudioPlayerComponent::PlayGrenadaAudio()
{
  GrenadeAudioInstance = UFMODBlueprintStatics::PlayEventAttached(GrenadeAudioEvent, PlayableCharacter->GetRootComponent(), NAME_None, PlayableCharacter->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
}

void UAudioPlayerComponent::PlayShieldAudio()
{
  ShieldAudioInstance = UFMODBlueprintStatics::PlayEventAttached(ShieldAudioEvent, PlayableCharacter->GetRootComponent(), NAME_None, PlayableCharacter->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
}

void UAudioPlayerComponent::DeactivateShieldAudio()
{
  if (IsValid(ShieldAudioInstance) && ShieldAudioInstance->IsPlaying())
  {
    ShieldAudioInstance->Stop();
    ShieldAudioInstance->Release();
  }
}

void UAudioPlayerComponent::PlayAbilityNotAvailable()
{
  AbilityNotAvailableAudioInstance = UFMODBlueprintStatics::PlayEventAttached(AbilityNotAvailableAudioEvent, PlayableCharacter->GetRootComponent(), NAME_None, PlayableCharacter->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);

}

void UAudioPlayerComponent::PlayHealthResetEvent()
{
  ResetHealthAudioInstance = UFMODBlueprintStatics::PlayEventAttached(ResetHealthAudioEvent, PlayableCharacter->GetRootComponent(), NAME_None, PlayableCharacter->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
}

void UAudioPlayerComponent::PlayDeathEvent()
{
  PlayerDeathAudioInstance = UFMODBlueprintStatics::PlayEventAttached(PlayerDeathAudioEvent, PlayableCharacter->GetRootComponent(), NAME_None, PlayableCharacter->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);

}

void UAudioPlayerComponent::StopAudio()
{
  PlayStopMovingEvent();

  if (IsValid(ShootAudioInstance))
  {
    ShootAudioInstance->Stop();
    ShootAudioInstance->Release();
  }

  if (IsValid(EndSuperShotInstance))
  {
    EndSuperShotInstance->Stop();
    EndSuperShotInstance->Release();
  }

  if (IsValid(SuperShotAudioInstance))
  {
    SuperShotAudioInstance->StudioInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
    SuperShotAudioInstance->StudioInstance->release();
  }

  if (IsValid(PlayerStepsAudioInstance))
  {
    PlayerStepsAudioInstance->Stop();
    PlayerStepsAudioInstance->Release();;
  }

  if (IsValid(DamageReceiveInstance))
  {
    DamageReceiveInstance->Stop();
    DamageReceiveInstance->Release();
  }

  if (IsValid(ShieldAudioInstance))
  {
    ShieldAudioInstance->Stop();
    ShieldAudioInstance->Release();
  }

  if (IsValid(GrenadeAudioInstance))
  {
    GrenadeAudioInstance->Stop();
    GrenadeAudioInstance->Release();
  }

  if (IsValid(SparkleAudioInstance))
  {
    SparkleAudioInstance->Stop();
    SparkleAudioInstance->Release();;
  }
  if (IsValid(AbilityNotAvailableAudioInstance))
  {
    AbilityNotAvailableAudioInstance->Stop();
    AbilityNotAvailableAudioInstance->Release();;
  }

  if (IsValid(ResetHealthAudioInstance))
  {
    ResetHealthAudioInstance->Stop();
    ResetHealthAudioInstance->Release();;
  }
}

void UAudioPlayerComponent::PauseAudio()
{
  if (IsValid(SuperShotAudioInstance))
  {
    SuperShotAudioInstance->StudioInstance->setPaused(true);
  }

  if (IsValid(ShieldAudioInstance))
  {
    ShieldAudioInstance->SetPaused(true);
  }
}

void UAudioPlayerComponent::RestartAudio()
{
  if (IsValid(SuperShotAudioInstance))
  {
    SuperShotAudioInstance->StudioInstance->setPaused(false);
  }

  if (IsValid(ShieldAudioInstance))
  {
    ShieldAudioInstance->SetPaused(false);
  }
}
