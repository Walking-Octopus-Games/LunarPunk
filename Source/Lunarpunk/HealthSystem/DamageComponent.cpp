// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageComponent.h"
#include "HealthComponent.h"
#include "VampirismComponent.h"
#include "Player/PlayableCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Enemies/IEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Turrets/Turret.h"
//#include <Lunarpunk/Enemies/AI/EnemyAIController.h>
//#include "Enemies/AI/DistanceEnemy/DistanceEnemyAIController.h"
#include "AIController.h"
#include "BrainComponent.h"

// Sets default values for this component's properties
UDamageComponent::UDamageComponent()
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;

  // ...

}


// Called when the game starts
void UDamageComponent::BeginPlay()
{
  Super::BeginPlay();

  // ...
  /*SelfHealthComponent = Cast<UHealthComponent>(GetOwner()->GetComponentByClass(UHealthComponent::StaticClass()));
  if (!IsValid(SelfHealthComponent) && PercentageOfLifeRecoveredWhenKillingEnemy > 0)
  {
    UE_LOG(LogTemp, Warning, TEXT("0 life will be recovered. The actor must have a health component to have Vampirism"));
  }*/

  //AActor* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

  //if (IsValid(Player))
  //{
  //  VampirismComponent = Cast<UVampirismComponent>(Player->GetComponentByClass(UVampirismComponent::StaticClass()));

  //  if (!IsValid(VampirismComponent))
  //  {
  //    //UE_LOG(LogTemp, Log, TEXT("Vamprism inactive"));
  //  }
  //}



}


bool UDamageComponent::IsTheTargetAtTheCorrectDistance(AActor* Actor)
{
  if (MaxDistanceToDamage == 0.0f || MaxDistanceToDamage <= GetOwner()->GetDistanceTo(Actor))
  {
    return true;
  }
  else
  {
    return false;
  }
}


void UDamageComponent::DamageComponent(UHealthComponent* HealthComponentToDamage, float Damage)
{
  if (IsValid(HealthComponentToDamage))
  {
    if (IsTheTargetAtTheCorrectDistance(HealthComponentToDamage->GetOwner()))
    {
      HealthComponentToDamage->ReceiveDamage(Damage);

      if (IsValid(VampirismComponent) && HealthComponentToDamage->GetHealth() <= 0)
      {
        VampirismComponent->HitVampirism();
      }
       
      DamageReceiveEvent.Broadcast();

      //DrawEnemyDamagePointWidget(HealthComponentToDamage);
    }
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("Health component must be valid"));
  }
}

//void UDamageComponent::DrawEnemyDamagePointWidget(const UHealthComponent* HealthComponentToDamage)
//{
//  if (ALunarpunkGameModeBase::bShowEnemyDamagePoints)
//  {
//    //IEnemy* Enemy = Cast<IEnemy>(HealthComponentToDamage->GetOwner());
//    //if (Enemy != nullptr)
//    //{
//    //  Enemy->DrawDamageWidget(DamageStrength);
//    //}
//
//    ATurret* Turret = Cast<ATurret>(HealthComponentToDamage->GetOwner());
//    if (IsValid(Turret))
//    {
//      Turret->DrawDamageWidget(DamageStrength);
//    }
//
//
//  }
//}

// Called every frame
void UDamageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // ...
  if (bIsDamagingPerSecond)
  {
    //TArray<UHealthComponent*> ComponentsDamage = ActualHealthComponentsBeingDamage;
    bIsDamagingPerSecond = false;
    for (UHealthComponent* ActualHealthComponentBeingDamage : ActualHealthComponentsBeingDamage)//ComponentsDamage)
    {
      if (IsValid(ActualHealthComponentBeingDamage) && ActualHealthComponentBeingDamage->bCanBeDamaged)
      {
        if (!bIsDamagingPerSecond)
        {
          bIsDamagingPerSecond = true;
        }
        this->DamageComponent(ActualHealthComponentBeingDamage, DPSStrength * DeltaTime);
      }
    }
  }

}

void UDamageComponent::MakeDamage(UHealthComponent* HealthComponent)
{
  if (IsValid(HealthComponent) && IsValid(HealthComponent->GetOwner()))
  {
    this->DamageComponent(HealthComponent, DamageStrength);


  }

}

void UDamageComponent::MakeDamagePerSecond(UHealthComponent* HealthComponent)
{
  if (IsValid(HealthComponent))
  {

    ActualHealthComponentsBeingDamage.Add(HealthComponent);
    bIsDamagingPerSecond = true;
  }
}

void UDamageComponent::StopDamagePerSecond(UHealthComponent* HealthComponent)
{
  if (IsValid(HealthComponent) && ActualHealthComponentsBeingDamage.Contains(HealthComponent))
  {
    //bIsDamagingPerSecond = false;
    ActualHealthComponentsBeingDamage.Remove(HealthComponent);
  }
}

void UDamageComponent::SetDamage(float NewDPSS, float NewDamageStrength)
{
  DamageStrength = NewDamageStrength;
  DPSStrength = NewDPSS;
}

void UDamageComponent::ResetComponent()
{
  DamageStrength = BaseDamageStrength;
  DPSStrength = BaseDPSStrength;
  ActualHealthComponentsBeingDamage.Empty();
}

void UDamageComponent::ApplyThrustImpact(FVector Forward, float Force, UCharacterMovementComponent* ActorHitted, AAIController* AIEnemyController)
{
  //AIEnemyController->StopMovement();

  Forward.Normalize(1.0f);
  const FVector ForceToApplyInDirection = Forward * Force;

  //AIEnemyController->ApplyThrustImpact(ForceToApplyInDirection, true);
  ActorHitted->Launch(ForceToApplyInDirection);
}


