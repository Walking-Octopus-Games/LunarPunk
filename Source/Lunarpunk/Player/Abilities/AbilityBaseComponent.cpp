// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Abilities/AbilityBaseComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/AbilitiesManger.h"
#include "Managers/PlatesManager.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "Player/LunarPunkPlayerController.h"
#include "Player/PlayableCharacter.h"
#include "Components/DecalComponent.h"

// Sets default values for this component's properties
UAbilityBaseComponent::UAbilityBaseComponent()
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;

  // ...
}

// Called when the game starts
void UAbilityBaseComponent::BeginPlay()
{
  Super::BeginPlay();

  // ...

}

void UAbilityBaseComponent::Initialize(ALunarPunkPlayerController* _PlayableController, APlayableCharacter* _PlayableCharacter)
{
  GameMode = Cast<ALunarPunkGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
  PlayableCharacter = _PlayableCharacter;
  AbilitiesManager = GameMode->AbilitiesManager;


  _PlayableController->ActiveAbilityEvent.AddDynamic(this, &UAbilityBaseComponent::Button2Pressed);
}

void UAbilityBaseComponent::ThrowAbility()
{
  AbilityThrowed.Broadcast(this->AbilityType);
}

void UAbilityBaseComponent::StopAbility()
{
}

bool UAbilityBaseComponent::AssertThrowAbility()
{


  if (!bButton2Pressed && IsValid(GameMode->GetPlatesManager()) && AbilityType == AbilitiesManager->GetCurrentAbility()
    && GameMode->GetPlatesManager()->TrySpendPlates(CostAbility))
  {
    return true;
  }

  if (!bButton2Pressed && AbilityType == AbilitiesManager->GetCurrentAbility())
  {
    AbilityNotThrown.Broadcast();
  }

  return false;
}


void UAbilityBaseComponent::Button2Pressed(bool bIsPressed)
{
  bButton2Pressed = bIsPressed;
  PlayableCharacter->AbilityZoneDecalRef->SetVisibility(bIsPressed);

  ThrowAbility();
}


// Called every frame
void UAbilityBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // ...
}


void UAbilityBaseComponent::ShowRayEffect(UNiagaraComponent* NiagaraEffectComponent)
{
  FVector ForwardPlayer = GetOwner()->GetActorForwardVector();
  ForwardPlayer.Normalize();

  if (IsValid(NiagaraEffectComponent) && NiagaraEffectComponent->GetAsset())
  {
    UNiagaraComponent* NiagaraEffectComponentCopy = NiagaraEffectComponent;
    NiagaraEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetOwner(), NiagaraEffectComponent->GetAsset(), GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
    NiagaraEffectComponent->SetNiagaraVariableVec3(FString("HitVector"), GetOwner()->GetActorLocation() + TraceRange * ForwardPlayer);
    NiagaraEffectComponent = NiagaraEffectComponentCopy;
  }
}