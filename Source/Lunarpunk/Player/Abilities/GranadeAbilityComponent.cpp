// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Abilities/GranadeAbilityComponent.h"

#include "Granade.h"
#include "Managers/AbilitiesManger.h"
#include "Managers/EntityManager.h"
#include "NiagaraComponent.h"
#include "TimerManager.h"
#include "NiagaraFunctionLibrary.h"
#include "Managers/PlatesManager.h"
#include "Enemies/AI/AIControllerZigZag.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DestructibleComponent.h"
#include "Player/PlayableCharacter.h"
#include "Player/CameraManager.h"
#include "Utilities/UtilitiesLunarpunk.h"
#include "Components/AudioComponent.h"
#include "Enemies/BreakableWall.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "Pool/Pool.h"
#include "Components/DecalComponent.h"


UGranadeAbilityComponent::UGranadeAbilityComponent()
{
  AbilityType = EAbilityType::Granade;
  DamageComponent = CreateDefaultSubobject<UDamageComponent>(TEXT("GrenadeDamageComponent"));
  AudioComponent = CreateDefaultSubobject<UAudioComponent>(FName("ExplosionAudioComponent"));
}

void UGranadeAbilityComponent::Initialize(ALunarPunkPlayerController* _PlayableController, APlayableCharacter* _PlayableCharacter)
{
  Super::Initialize(_PlayableController, _PlayableCharacter);
}


void UGranadeAbilityComponent::Button2Pressed(bool bIsPressed)
{
    Super::Button2Pressed(bIsPressed);
}

void UGranadeAbilityComponent::ThrowAbility()
{
    if(AssertThrowAbility())
    {
        ThrowGrenadeAnimation();

        if (GetOwner())
        {
            FVector SparksOrigin = PlayableCharacter->GetActorLocation().GetSafeNormal();
                //
            SparksOrigin.Z -= Height;
            ThrowGrenadeEvent.Broadcast();
            

            //SpawnNiagaraEffect(SparksOrigin);
            PlayableCharacter->AbilityCollider->GetOverlappingActors(EnemiesInRange, ABasicEnemy::StaticClass());

            PlayableCharacter->AbilityCollider->GetOverlappingActors(WallsInRange, ABreakableWall::StaticClass());

            FTimerHandle TimerShake;
            GameMode->CameraManager->StartShakingCamera(UpDownShakeSpeed, RightLeftShakeSpeed, UpDownAngle, RightLeftAngle, bShouldVibrate);
            
            GetWorld()->GetTimerManager().SetTimer(TimerShake, this, &UGranadeAbilityComponent::StopAbility, ShakeDuration, false);

            for (auto& Class : WallsInRange) 
            {

                ABreakableWall* Wall = Cast<ABreakableWall>(Class);

                if ( Wall && Wall->bCanBeBroken) //Class->IsA(ABreakableWall::StaticClass()))
                {
                    //Class.me
                    FVector PushDirectionWall = Class->GetActorLocation() - SparksOrigin;
                    PushDirectionWall.Normalize(1.0f);
                    FVector ForceToApplyInDirectionWall = PushDirectionWall * ThrustImpactForceWall;



                    //UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Class->GetOwner()->GetRootComponent());
                    //ABreakableWall* Wall = Cast<ABreakableWall>(Class);
                    //Wall->DestructibleComponent->AddForce(ForceToApplyInDirectionWall);
                    Wall->DestructibleComponent->SetSimulatePhysics(true);
                    Wall->bEnableHardSleeping = false;
                    Wall->BreackWall();


                    Wall->DestructibleComponent->AddImpulse(ForceToApplyInDirectionWall, NAME_None, false);
                    Wall->WallBroken.Broadcast();

                    FTimerHandle TimerHandle;
                    WallToDestroy = Class;
                    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UGranadeAbilityComponent::DestroyWall, 3.0f, false);
                    //Wall->DestructibleComponent->ApplyDamage(1.f, Wall->DestructibleComponent->GetComponentLocation(), PushDirectionWall, ThrustImpactForce);
                    //Wall->DestroyWall(1.f, Wall->DestructibleComponent->GetComponentLocation(), PushDirectionWall, ThrustImpactForce);

                    //Class->Destroy();
                }

            }




            for (auto& Class : EnemiesInRange) {
                if (Class->IsA(ABasicEnemy::StaticClass())) {
                    UHealthComponent* EnemyHealthComponent = Cast<UHealthComponent>(Class->GetComponentByClass(UHealthComponent::StaticClass()));

                    IEnemy* HitActorCasted = Cast<IEnemy>(Class);
                    UHealthComponent* HittedHealthComponent = HitActorCasted->GetHealthComponent();
                    if (IsValid(HittedHealthComponent))
                    {
                        HittedHealthComponent->ReceiveDamage(DamageGranade);
                    }

                    FVector PushDirection = Class->GetActorLocation() - SparksOrigin;
                    IEnemy* Enemy = Cast<IEnemy>(Class);
                    UHealthComponent* OverlapedHealthComponent = Enemy->GetHealthComponent();
                    UCharacterMovementComponent* EnemyCharacterMovementComponent = Cast<UCharacterMovementComponent>(Class->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
                    if (IsValid(EnemyCharacterMovementComponent))
                    {
                        APawn* Pawn = Cast<APawn>(Class);
                        AAIController* EnemyAIController = Cast<AAIController>(Pawn->GetController());
                        AAIControllerZigZag* ZigZagEnemyAIController = Cast<AAIControllerZigZag>(Pawn->GetController());

                        if (IsValid(ZigZagEnemyAIController))
                        {

                            ZigZagEnemyAIController->StopMovement();
                            PushDirection.Normalize(1.0f);
                            const FVector ForceToApplyInDirection = PushDirection * ThrustImpactForce;
                            EnemyCharacterMovementComponent->Launch(ForceToApplyInDirection);

                        }
                        else if (IsValid(EnemyAIController))
                        {
                            DamageComponent->ApplyThrustImpact(PushDirection, ThrustImpactForce, EnemyCharacterMovementComponent, EnemyAIController);
                        }
                    }
                }
            }
        }
        ThrowGrenadeEvent.Broadcast();
        ShowGranadePath();
        Super::ThrowAbility();
    }
}

void UGranadeAbilityComponent::SpawnNiagaraEffect(FVector ExplosionOriginLocation)
{
 

  

}

void UGranadeAbilityComponent::DestroyWall()
{
    if (WallToDestroy && Cast<ABreakableWall>(WallToDestroy)->isTutorialWall == true)
    {
        Cast<ABreakableWall>(WallToDestroy)->WallToActive->CanBeBreak();
        WallToDestroy->Destroy();
    }

    if (WallToDestroy)
    {

        WallToDestroy->Destroy();
    }
    //isTutorialWall
    

    
    
}

void UGranadeAbilityComponent::StopAbility()
{
  GameMode->CameraManager->StopShakingCamera();
}


