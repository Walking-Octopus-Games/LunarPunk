// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Abilities/DestroyPortalAbilityComponent.h"

#include "NiagaraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Managers/EntityManager.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "Player/CameraManager.h"
#include "Player/LunarPunkPlayerController.h"
#include "Player/PlayableCharacter.h"
#include "Portal/Portal.h"
#include "Managers/AbilitiesManger.h"
#include "HealthSystem/HealthComponent.h"

UDestroyPortalAbilityComponent::UDestroyPortalAbilityComponent()
{
    AbilityType = EAbilityType::DestroyPortal;
}

void UDestroyPortalAbilityComponent::ThrowAbility()
{
}

void UDestroyPortalAbilityComponent::Button1Pressed(bool bIsPressed)
{
  bButton1Pressed = bIsPressed;

  CheckIfAbilityStart();
}

void UDestroyPortalAbilityComponent::Button2Pressed(bool bIsPressed)
{
  bButton2Pressed = bIsPressed;

  CheckIfAbilityStart();

}


void UDestroyPortalAbilityComponent::Initialize(ALunarPunkPlayerController* _PlayableController, APlayableCharacter* _PlayableCharacter)
{
  Super::Initialize(_PlayableController, _PlayableCharacter);


  _PlayableController->DestroyPortalShootButton1Event.AddDynamic(this, &UDestroyPortalAbilityComponent::Button1Pressed);
  _PlayableController->DestroyPortalShootButton2Event.AddDynamic(this, &UDestroyPortalAbilityComponent::Button2Pressed);
}

void UDestroyPortalAbilityComponent::StopAbility()
{
  ResetDestroyPortal();
}

void UDestroyPortalAbilityComponent::StartDestroyPortal()
{

  if (bButton1Pressed && bButton2Pressed && GameMode->EntityManager->GetPortals().Num() > 0)
  {

    for (auto Portal : GameMode->EntityManager->GetPortals())
    {
      if (Portal->Index == IndexPortalToDestroy)
      {
        PortalToDestroy = Portal;

        const FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), PortalToDestroy->GetActorLocation());
        GetOwner()->SetActorRotation(NewRotation);

        if (IsValid(GameMode->CameraManager))
        {
          GameMode->CameraManager->StartShakingCamera(UpDownShakeSpeed, RightLeftShakeSpeed, UpDownAngle, RightLeftAngle, bShouldVibrate);
        }
        PlayableCharacter->LPPlayerController->IsDeactivated = true;
        GetWorld()->GetTimerManager().ClearTimer(DestroyPortalTimerHandle);
        GetWorld()->GetTimerManager().SetTimer(DestroyPortalTimerHandle, this, &UDestroyPortalAbilityComponent::DestroyPortal, 1.0f, false, timeToDestroyPortal);
        SetAvailableToDestroyPortal.Broadcast(true, timeToDestroyPortal);
        break;
      }

    }
  }
}

void UDestroyPortalAbilityComponent::DestroyPortal()
{

  if (PortalToDestroy->bAddLifeToPlayerWhenDie)
  {
      UHealthComponent* HealthComponentPlayer = PlayableCharacter->HealthComponent;
      if (HealthComponentPlayer)
      {
            HealthComponentPlayer->SetMaxHealth(HealthComponentPlayer->GetMaxHealth() + PortalToDestroy->LifeToAdd, false);
      }
  }
  
  DestroyPortalEvent.Broadcast();
  if (PortalToDestroy->DestroyPortal())
  {
      GameMode->EntityManager->GetPortals().Remove(PortalToDestroy);
  }
  ShowRayEffect(PlayableCharacter->RayDestroyPortalEffectComponent);
  PlayableCharacter->ActualPlayerStatus = EPlayerState::Default;
  PlayableCharacter->LPPlayerController->IsDeactivated = false;
  bButton1Pressed = false;
  bButton2Pressed = false;

}

void UDestroyPortalAbilityComponent::ResetDestroyPortal()
{
  if (IsValid(GameMode->CameraManager))
  {
    GameMode->CameraManager->StopShakingCamera();
  }
  SetAvailableToDestroyPortal.Broadcast(false, timeToDestroyPortal);
  PlayableCharacter->LPPlayerController->IsDeactivated = false;
  GetWorld()->GetTimerManager().ClearTimer(DestroyPortalTimerHandle);

}

void UDestroyPortalAbilityComponent::CheckIfAbilityStart()
{
  if (bButton1Pressed)
  {
    UE_LOG(LogTemp, Warning, TEXT("BUTTON1 PRESSED: true"));
  }
  else
  {
    UE_LOG(LogTemp, Warning, TEXT("BUTTON1 PRESSED: false"));
  }

  if (bButton2Pressed)
  {
    UE_LOG(LogTemp, Warning, TEXT("BUTTON2 PRESSED: true"));
  }
  else
  {
    UE_LOG(LogTemp, Warning, TEXT("BUTTON2 PRESSED: false"));
  }

  if (bButton1Pressed && bButton2Pressed)
  {
    StartDestroyPortal();
  }
  else
  {
    StopAbility();
  }
}
