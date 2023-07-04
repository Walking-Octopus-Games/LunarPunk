// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AudioEnemyComponent.h"

#include "FMODBlueprintStatics.h"
#include "HealthSystem/HealthComponent.h"

// Sets default values for this component's properties
UAudioEnemyComponent::UAudioEnemyComponent()
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;

  // ...
}


// Called when the game starts
void UAudioEnemyComponent::BeginPlay()
{
  Super::BeginPlay();

  // ...

}

void UAudioEnemyComponent::PlayDamageReceive()
{
  DamageReceiveInstance = UFMODBlueprintStatics::PlayEventAttached(DamageReceiveAudioEvent, GetOwner()->GetRootComponent(), NAME_None, GetOwner()->GetActorLocation(), EAttachLocation::SnapToTarget, true, true, true);
}

void UAudioEnemyComponent::Initialize(UHealthComponent* HealthComponent)
{
  HealthComponent->ActorDeath.AddDynamic(this, &UAudioEnemyComponent::PlayDamageReceive);
}


// Called every frame
void UAudioEnemyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // ...
}

void UAudioEnemyComponent::BeginDestroy()
{
  if (DamageReceiveInstance != nullptr && DamageReceiveInstance->IsPlaying())
  {
    DamageReceiveInstance->Stop();
    DamageReceiveInstance->Release();
  }

  Super::BeginDestroy();
}

