// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Abilities/ShootAbilityComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Enemies/BreakableWall.h"
#include "Enemies/IEnemy.h"
#include "HealthSystem/DamageComponent.h"
#include "HealthSystem/HealthComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Managers/AbilitiesManger.h"
#include "Managers/EntityManager.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "Player/PlayableCharacter.h"
#include "RewardsAndUpgrades/RewardObject.h"
#include "Traps/Trap.h"

#define INFINITE_FLOAT (float)0x7F800000

UShootAbilityComponent::UShootAbilityComponent()
{
  AbilityType = EAbilityType::Shot;


}

void UShootAbilityComponent::EnableAbility()
{
  bCanShoot = true;
}

void UShootAbilityComponent::DisableAbility()
{
  GetWorld()->GetTimerManager().ClearTimer(TimerHandler);
  bCanShoot = false;
}

void UShootAbilityComponent::ShootCooldown()
{
  bCanShoot = true;
  GetWorld()->GetTimerManager().ClearTimer(TimerHandler);
}

void UShootAbilityComponent::StartFireRateTimer()
{
  if (MaxShootTimeRange > 0)
  {
    bCanShoot = false;
    ActualShootTime = FMath::RandRange(MinShootTimeRange, MaxShootTimeRange);
    GetWorld()->GetTimerManager().SetTimer(TimerHandler, this, &UShootAbilityComponent::ShootCooldown, ActualShootTime, false, ActualShootTime);
  }
}

void UShootAbilityComponent::Initialize(ALunarPunkPlayerController* _PlayableController, APlayableCharacter* _PlayableCharacter)
{
  Super::Initialize(_PlayableController, _PlayableCharacter);

  //Set the shooting coldown timer
  bCanShoot = true;

  _PlayableController->ShootEvent.AddDynamic(this, &UShootAbilityComponent::Shoot);
}

void UShootAbilityComponent::StopAbility()
{
}

void UShootAbilityComponent::Shoot(bool _IsPressed)
{
  IsPressed = _IsPressed;
  if (bCanShoot && IsPressed)
  {

    

    FVector ForwardPlayer = GetOwner()->GetActorForwardVector();
    ForwardPlayer.Normalize();
    TArray<FVector> ShootDirections = { ForwardPlayer };

    if (bMultipleShot)
    {
      ShootDirections.Add(GetShootDirection(GetOwner()->GetActorRotation(), FRotator(0, 30, 0)));
      ShootDirections.Add(GetShootDirection(GetOwner()->GetActorRotation(), FRotator(0, -30, 0)));
    }

    for (const FVector ShootDirection : ShootDirections)
    {
      ShootImpactCheck(ShootDirection);
    }

    StartFireRateTimer();

  }

}

void UShootAbilityComponent::ThrowAbility()
{
}

void UShootAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType,
  FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
  if (bCanShoot && IsPressed)
  {
    Shoot(IsPressed);
  }
  else if (!IsPressed) {
      PlayerStopShooting.Broadcast();
  }
  
}


FVector UShootAbilityComponent::GetShootDirection(const FRotator ActualRotation, const FRotator Rotation)
{
  return FRotationMatrix(ActualRotation + Rotation).GetScaledAxis(EAxis::X);
}



void UShootAbilityComponent::ShootImpactCheck(FVector ShootDirection)
{
  FVector LineStart = PlayableCharacter->StartShootReferencePoint->GetComponentLocation() + 50.0f * ShootDirection;
  FVector LineEnd = LineStart + INFINITE_FLOAT * ShootDirection;
  FHitResult Hit;
  TArray<AActor*> ActorsToIgnore;
  TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
  ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel3));
  ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Destructible));


  bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
    GetWorld(),
    LineStart,
    LineEnd,
    ShotRadius,
    ObjectTypes,
    false,
    ActorsToIgnore,
    EDrawDebugTrace::None,
    Hit,
    true,
    FLinearColor::Red,
    FLinearColor::Green,
    3.0f
  );


  //Try to hit an enemy
  if (bHit)
  {
    ShowShootEffectComponent(ShootDirection, LineStart, Hit);

    ShowHitEffect(ShootDirection, LineStart, Hit);

    ActionsTriggeredByHit(Hit);
    PlayerShooting.Broadcast();
  }
  else //If not, hit the world and draw the ray
  {
    FCollisionQueryParams TraceParams;

    if (GetWorld()->LineTraceSingleByChannel(Hit, LineStart, LineEnd, ECC_WorldDynamic, TraceParams))
    {

      ShowShootEffectComponent(ShootDirection, LineStart, Hit);

      ShowHitEffect(ShootDirection, LineStart, Hit);
      PlayerShooting.Broadcast();
      //PlayShootAnimation();
    }
  }
  
}

void UShootAbilityComponent::ShowShootEffectComponent(FVector ShootDirection, FVector LineStart, FHitResult Hit)
{
  if (IsValid(PlayableCharacter->ShootEffectComponent) && PlayableCharacter->ShootEffectComponent->GetAsset() && !GameMode->EntityManager->bDeactivateVFX)
  {
    UNiagaraComponent* ShootEffectComponentCopy = PlayableCharacter->ShootEffectComponent;
    PlayableCharacter->ShootEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetOwner(), PlayableCharacter->ShootEffectComponent->GetAsset(), LineStart, GetOwner()->GetActorRotation());
    PlayableCharacter->ShootEffectComponent->SetNiagaraVariableVec3(FString("HitVector"), LineStart + Hit.Distance * ShootDirection);
    PlayableCharacter->ShootEffectComponent = ShootEffectComponentCopy;
  }
}

void UShootAbilityComponent::ShowHitEffect(FVector ShootDirection, FVector LineStart, FHitResult Hit) const
{
  if (HitEffect && IsValid(Hit.GetActor()) && !GameMode->EntityManager->bDeactivateVFX)
  {
    //UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, Hit.GetActor()->GetActorLocation());
    UNiagaraComponent* HitEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, LineStart + Hit.Distance * ShootDirection);
    HitEffectComponent->SetVariableFloat(FName("R"), Color.R);
    HitEffectComponent->SetVariableFloat(FName("G"), Color.G);
    HitEffectComponent->SetVariableFloat(FName("B"), Color.B);
    HitEffectComponent->SetVariableFloat(FName("A"), Color.A);
    HitEffectComponent->SetVariableLinearColor(FName("Color"), Color);
    
  }
}

void UShootAbilityComponent::ActionsTriggeredByHit(const FHitResult& Hit) const
{

  UE_LOG(LogTemp, Warning, TEXT("PlayableCharacter.cpp: Shoot Function: Something hitted"));

  IEnemy* HittedEnemy = Cast<IEnemy>(Hit.GetActor());
  ARewardObject* RewardObject = Cast<ARewardObject>(Hit.GetActor());
  ABreakableWall* BreakableWall = Cast<ABreakableWall>(Hit.GetActor());
  ATrap* Trap = Cast<ATrap>(Hit.GetActor());

  if (RewardObject != nullptr)
  {
    UE_LOG(LogTemp, Warning, TEXT("Reward object hitted"));
    RewardObject->HitReward();
  }

  if (BreakableWall != nullptr)
  {
      UE_LOG(LogTemp, Warning, TEXT("Reward object hitted"));
      BreakableWall->HitWall();
  }


  if (HittedEnemy != nullptr)
  {
    UE_LOG(LogTemp, Warning, TEXT("PlayableCharacter.cpp: Shoot Function: Enemy Valid"));
    UHealthComponent* EnemyHealthComponent = HittedEnemy->GetHealthComponent();
    if (IsValid(EnemyHealthComponent))
    {
      UE_LOG(LogTemp, Warning, TEXT("PlayableCharacter.cpp: Shoot Function: Enemy HealthComponent Valid"));
      PlayableCharacter->DamageComponent->MakeDamage(EnemyHealthComponent);
      PlayerMakeDamage.Broadcast(PlayableCharacter->DamageComponent->DamageStrength);
      UE_LOG(LogTemp, Warning, TEXT("PlayableCharacter.cpp: Shoot Function: Enemy Damaged"));
    }
  }

  if (Trap)
  {
    Trap->HitTrap();
  }
}
