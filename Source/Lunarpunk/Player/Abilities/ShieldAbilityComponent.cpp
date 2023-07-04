// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Abilities/ShieldAbilityComponent.h"

#include "NiagaraComponent.h"
#include "Enemies/BasicEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayableCharacter.h"
#include "HealthSystem/HealthComponent.h"
#include "Managers/AbilitiesManger.h"
#include "Managers/EntityManager.h"
#include "Managers/PlatesManager.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "Turrets/Turret.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"

UShieldAbilityComponent::UShieldAbilityComponent()
{
  AbilityType = EAbilityType::Shield;
}

void UShieldAbilityComponent::Initialize(ALunarPunkPlayerController* _PlayableController, APlayableCharacter* _PlayableCharacter)
{
  Super::Initialize(_PlayableController, _PlayableCharacter);
}

void UShieldAbilityComponent::ThrowAbility()
{
  if (AssertThrowAbility())
  {
    ThrowShieldEvent.Broadcast();
    GetWorld()->GetTimerManager().ClearTimer(ShieldTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(ShieldTimerHandle, this, &UShieldAbilityComponent::ActiveShields, 1.0f, false, 0.6);
    Super::ThrowAbility();
  }
}

void UShieldAbilityComponent::StopAbility()
{
}

void UShieldAbilityComponent::Button2Pressed(bool bIsPressed)
{
  Super::Button2Pressed(bIsPressed);
}

void UShieldAbilityComponent::ActiveShields()
{
  FVector ForwardPlayer = GetOwner()->GetActorForwardVector();
  ForwardPlayer.Normalize();

  TArray<AActor*> OutActors;
  PlayableCharacter->AbilityCollider->GetOverlappingActors(OutActors, ATurret::StaticClass());

  PlayableCharacter->RestartShieldEffect();

  for (AActor* OutActor : OutActors)
  {
    ATurret* Turret = Cast<ATurret>(OutActor);

    if (Turret->HealthComponent->GetActualHealth() == 0)
    {
      Turret->RestartTurret();
    }
    else
    {
      Turret->ActivateShield();
    }
  }

  PlayableCharacter->MeshShield->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Block);
  PlayableCharacter->MeshShield->SetVisibility(true);
  PlayableCharacter->GetHealthComponent()->SetCanBeDamaged(false);
  GetWorld()->GetTimerManager().ClearTimer(ShieldTimerHandle);
  GetWorld()->GetTimerManager().SetTimer(ShieldTimerHandle, this, &UShieldAbilityComponent::DeactivateShield, 1.0f, false, TimeShield);
}

void UShieldAbilityComponent::DeactivateShield()
{
  PlayableCharacter->GetHealthComponent()->SetCanBeDamaged(true);
  PlayableCharacter->MeshShield->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
  PlayableCharacter->MeshShield->SetVisibility(false);
  DeactivateShieldEvent.Broadcast();
}
