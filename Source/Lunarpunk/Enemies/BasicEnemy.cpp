// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/BasicEnemy.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "HealthSystem/DamageComponent.h"
#include "HealthSystem/HealthComponent.h"
#include "WalkToDestinationComponent.h"
#include "HealthSystem/PoisonComponent.h"
//#include "AI/EnemyAIController.h"
#include "AIController.h"
#include "AudioEnemyComponent.h"
#include "BrainComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Pool/Pool.h"
#include "Turrets/Turret.h"
#include "Player/PlayableCharacter.h"
#include "RewardsAndUpgrades/DropRewardsComponent.h"
#include "Components/AudioComponent.h"
#include "Managers/EntityManager.h"
#include "Managers/HUDManager.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ZigZagPawn.h"
#include "AI/AvoidEnemies.h"
#include "AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABasicEnemy::ABasicEnemy()
{
  // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;


  HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
  AddOwnedComponent(HealthComponent);

  PoisonComponent = CreateDefaultSubobject<UPoisonComponent>(TEXT("PoisonComponent"));
  AddOwnedComponent(PoisonComponent);

  DamageComponent = CreateDefaultSubobject<UDamageComponent>(TEXT("DamageComponent"));
  AddOwnedComponent(DamageComponent);


  //Range Collision to perform the attack
  DamageRangeCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("DamageRangeCollision"));

  DamageRangeCollision->SetCapsuleRadius(20.0f);
  DamageRangeCollision->SetCapsuleHalfHeight(50);
  DamageRangeCollision->SetWorldLocation(FVector(2.0, -8.0, -8.0));
  DamageRangeCollision->SetWorldRotation(FRotator(-90.0, 0.0, 0.0));

  AudioEnemyComponent = CreateDefaultSubobject<UAudioEnemyComponent>(TEXT("Audio Enemy Component"));

  //Create audio component.
  /*AudioComponent = CreateDefaultSubobject<UAudioComponent>(FName("Audio Component"));
  AudioComponent->SetVolumeMultiplier(VolumeSound);*/
  AvoidEnemiesComponent = CreateDefaultSubobject<UAvoidEnemies>(TEXT("Avoid Enemies Component"));

  EnemyAvoidSphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Avoid Sphere Collision"));
  EnemyAvoidSphereCollision->SetupAttachment(GetRootComponent());
  AvoidEnemiesComponent->AvoidSphereCollision = EnemyAvoidSphereCollision;
}

// Called when the game starts or when spawned
void ABasicEnemy::BeginPlay()
{
  Super::BeginPlay();

  AICharacterController = Cast<AAIController>(GetController());

  CapsuleComponentCollision = Cast<UCapsuleComponent>(GetComponentByClass(UCapsuleComponent::StaticClass()));

  RewardsComponent = FindComponentByClass<UDropRewardsComponent>();

  if (!IsValid(RewardsComponent))
  {
    UE_LOG(LogTemp, Warning, TEXT("EnemyBase.cpp: No se encontro la RewardsComponent"));
  }

  MeshComponent = FindComponentByClass<USkeletalMeshComponent>();

  if (!IsValid(MeshComponent))
  {
    UE_LOG(LogTemp, Warning, TEXT("EnemyBase.cpp: No se encontro la SkeletalMeshComponent"));
  }


  //Add the callback to the Damage Range collision and attacht it to the  foot.
  if (IsValid(DamageRangeCollision))
  {

    DamageRangeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DamageRangeCollision->OnComponentBeginOverlap.AddDynamic(this, &ABasicEnemy::OnOverlapBegin);

    if (MeshComponent)
    {
      FAttachmentTransformRules Rules = FAttachmentTransformRules(
        /*EAttachmentRule::KeepRelative,
        EAttachmentRule::KeepRelative,
        EAttachmentRule::KeepRelative,*/
        EAttachmentRule::SnapToTarget,
        true
      );

      DamageRangeCollision->AttachToComponent(MeshComponent, Rules, FName("index_03_l"));
      DamageRangeCollision->AttachToComponent(MeshComponent, Rules, FName("index_03_r"));

    }
    //DamageRangeCollision->OnComponentEndOverlap.AddDynamic(this, &ABasicEnemy::OnOverlapEnd);
  }

  MovementComponent = FindComponentByClass<UCharacterMovementComponent>();

  if (!MovementComponent)
  {
    UE_LOG(LogTemp, Warning, TEXT("MovementComponent no encontrada"));
  }
  else
  {
    InitialSpeed = MovementComponent->MaxWalkSpeed;
  }

  //Tutorial and Debug modes:

  if (bSetActiveAtBegin)
  {
    bIsActive = true;
  }

  if (bDeathBrainMode)
  {
    SetDeathBrainMode();
  }

}

void ABasicEnemy::Initialize(APool* Pool)
{
  MyPool = Pool;

  if (!IsValid(MyPool))
  {
    UE_LOG(LogTemp, Warning, TEXT("EnemyBase.cpp: No se encontro la Pool"));
  }


  LunarPunkGameMode = Cast<ALunarPunkGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
  if (IsValid(LunarPunkGameMode))
  {
    LunarPunkGameMode->HUDManager->SetShowDamagePointsEvent.AddDynamic(this, &ABasicEnemy::SetShowEnemyDamagePoints);
    PlayerPawn = LunarPunkGameMode->EntityManager->PlayerCharacter;
  }
  if (IsValid(HealthComponent))
  {
    AudioEnemyComponent->Initialize(HealthComponent);
  }
  else
  {
    UE_LOG(LogTemp, Warning, TEXT("EnemyBase.cpp: No se encontro HealthComponent"));
  }

}

// Called every frame
void ABasicEnemy::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);





  if (ZigZagPawnToFollow != nullptr)
  {
    if (IsValid(PlayerPawn))
    {
      if (ZigZagPawnToFollow->GetDistanceTo(PlayerPawn) <= 200.0f)
      {
        ZigZagPawnToFollow->Destroy();
        SphereToFollow = NULL;
      }
    }

  }





  /*
    if (SphereToFollow != nullptr) {
      if (this->GetDistanceTo(Cast<AActor>(SphereToFollow)) < 100.f) {
          this->ZigZagPawnToFollow->TimeToChangePosition();
      }
    }


  */





}

// Called to bind functionality to input
void ABasicEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
  Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Called to bind functionality to input
void ABasicEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

  if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))//&& OtherComp->IsA<UBoxComponent>())
  {

    if (OtherActor->IsA<ATurret>() || OtherActor->IsA<APlayableCharacter>())
    {
      UHealthComponent* overlappedHealthComponent = Cast<UHealthComponent>(OtherActor->GetComponentByClass(UHealthComponent::StaticClass()));

      if (IsValid(overlappedHealthComponent))
      {
        if (overlappedHealthComponent->bCanBeDamaged)// && !DamageComponent->bIsDamagingPerSecond)
        {
          DamageComponent->MakeDamage(overlappedHealthComponent);
          if (OtherActor->IsA<ATurret>())
          {
            //PlayHitTurretSound();
          }
          else
          {
            //PlayHitPlayerSound();
          }
          EndTryToHit();
          SpawnImpactEffect(DamageRangeCollision->GetComponentLocation(), OtherActor->GetActorLocation());
        }
      }
    }
  }

}

//void ABasicEnemy::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//
//  if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) )//&& OtherComp->IsA<UBoxComponent>())
//  {
//    
//    if (OtherActor->IsA<ATurret>() || OtherActor->IsA<APlayableCharacter>())
//    {
//      UHealthComponent* overlappedHealthComponent = Cast<UHealthComponent>(OtherActor->GetComponentByClass(UHealthComponent::StaticClass()));
//
//      if (IsValid(overlappedHealthComponent))
//      {
//        DamageComponent->StopDamagePerSecond(overlappedHealthComponent);
//      }
//    }
//  }
//
//}

void ABasicEnemy::ChangeMaterial(int index)
{
  USkeletalMeshComponent* mesh = Cast<USkeletalMeshComponent>(GetComponentByClass(USkeletalMeshComponent::StaticClass()));

  if (mesh)
  {
    --index;

    if (index >= Materials.Num())
    {
      index %= Materials.Num();
    }

    mesh->SetMaterial(0, Materials[index]);
    mesh->SetMaterial(1, Materials[index]);
  }
}

UHealthComponent* ABasicEnemy::GetHealthComponent()
{
  return HealthComponent;
}

UDamageComponent* ABasicEnemy::GetDamageComponent()
{
  return DamageComponent;
}

APool* ABasicEnemy::GetMyPool()
{
  return MyPool;
}

UDropRewardsComponent* ABasicEnemy::GetRewardsComponent()
{
  return RewardsComponent;
}

UWalkToDestinationComponent* ABasicEnemy::GetWalkToDestinationComponent()
{
  return WalkToDestinationComponent;
}

void ABasicEnemy::AttachSphere(USphereComponent* sphere)
{
  SphereToFollow = sphere;
}

void ABasicEnemy::AttachZigZagPawn(AZigZagPawn* zigzagpawn)
{

  ZigZagPawnToFollow = zigzagpawn;

  if (MeshComponent)
  {
    FAttachmentTransformRules Rules = FAttachmentTransformRules(
      /*EAttachmentRule::KeepRelative,
      EAttachmentRule::KeepRelative,
      EAttachmentRule::KeepRelative,*/
      EAttachmentRule::SnapToTarget,
      true
    );

    DamageRangeCollision->AttachToComponent(MeshComponent, Rules, FName("Bip001-L-Finger0Nub"));
    DamageRangeCollision->AttachToComponent(MeshComponent, Rules, FName("Bip001-R-Finger0Nub"));
  }

}



USphereComponent* ABasicEnemy::GetSphereToFollow()
{
  return SphereToFollow;
}


void ABasicEnemy::ReturnToPool()
{

  if (bNonReturnable)
  {
    Destroy();
  }
  else
  {
    if (IsValid(MyPool))
    {
      Deactivate();
      SetActorLocation(MyPool->GetActorLocation());
    }
  }


}

void ABasicEnemy::DeactivateForDeathAnim()
{
  if (IsValid(AICharacterController))
  {


    if (IsValid(EnemyAvoidSphereCollision))
    {
      EnemyAvoidSphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(GetController());
    if (IsValid(EnemyAIController))
    {
      EnemyAIController->DeleteBBValues();
      //EnemyAIController->PBlackboardComponent->ClearValue();
      //EnemyAIController->DeleteBBValues();
      if (IsValid(EnemyAIController->PBlackboardComponent))
      {
        EnemyAIController->PBlackboardComponent->ClearValue("TurretMovingPersistance");
      }
    }
    AICharacterController->StopMovement();
    AICharacterController->GetBrainComponent()->StopLogic(FString("Go to pool"));
    AICharacterController->GetBrainComponent()->Deactivate();
    AICharacterController->UnPossess();

  }

  if (IsValid(DamageRangeCollision))
  {
    DamageRangeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

  }

  if (IsValid(CapsuleComponentCollision))
  {
    CapsuleComponentCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

  }

}

void ABasicEnemy::Deactivate()
{
  bIsActive = false;
  bDeathBrainMode = false;

  SetActorTickEnabled(false);
  //PrimaryActorTick.bCanEverTick = false;
  //DeactivateForDeathAnim();

  if (IsValid(WalkToDestinationComponent))
  {
    WalkToDestinationComponent->bIsActive = false;
    WalkToDestinationComponent->ATargetToFollowFirst = nullptr;

  }

  if (IsValid(MeshComponent))
  {
    MeshComponent->ResetAnimInstanceDynamics(ETeleportType::TeleportPhysics);
    MeshComponent->SetVisibility(false);
    MeshComponent->Deactivate();
  }

  if (IsValid(AICharacterController))
  {
    if (AICharacterController != nullptr)
    {
      AICharacterController->GetBrainComponent()->StopLogic(FString("Go to pool"));
      AICharacterController->GetBrainComponent()->Deactivate();
      AICharacterController->UnPossess();
    }
  }

  GetCharacterMovement()->GravityScale = 0.f;

}

void ABasicEnemy::Reactivate()
{
  bIsActive = true;

  //PrimaryActorTick.bCanEverTick = true;
  SetActorTickEnabled(true);
  if (GetController() == nullptr)
  {
    AICharacterController->Possess(this);
  }
  AICharacterController->Reset();
  if (IsValid(EnemyAvoidSphereCollision))
  {
    EnemyAvoidSphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
  }

  // Restore Health
  if (IsValid(HealthComponent))
  {
    HealthComponent->ResetComponent();
  }

  if (IsValid(DamageComponent))
  {
    DamageComponent->ResetComponent();
  }

  if (IsValid(MovementComponent))
  {
    MovementComponent->MaxWalkSpeed = InitialSpeed;
  }

  if (IsValid(RewardsComponent))
  {
    RewardsComponent->ResetValues();
  }



  /* if (IsValid(DamageRangeCollision))
   {
       DamageRangeCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
   }*/

  if (IsValid(CapsuleComponentCollision))
  {
    CapsuleComponentCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CapsuleComponentCollision->SetCanEverAffectNavigation(true);
  }

  if (IsValid(WalkToDestinationComponent))
  {
    WalkToDestinationComponent->bIsActive = true;

  }


  if (IsValid(MeshComponent))
  {
    MeshComponent->ResetAnimInstanceDynamics(ETeleportType::ResetPhysics);
    MeshComponent->Activate(true);
    MeshComponent->SetVisibility(true);
  }


  if (IsValid(AICharacterController))
  {

    AICharacterController->GetBrainComponent()->Activate();
    AICharacterController->GetBrainComponent()->RestartLogic();
  }

  if (IsValid(LunarPunkGameMode->EntityManager))
  {
    FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LunarPunkGameMode->EntityManager->PlayerCharacter->GetActorLocation());
    RootComponent->SetWorldRotation(PlayerRot);

  }

  FReactivate.Broadcast(this);
  GetCharacterMovement()->GravityScale = 1.f;

  if (IsValid(MovementComponent) && IsValid(AvoidEnemiesComponent))
  {
    AvoidEnemiesComponent->OriginalVelocity = MovementComponent->MaxWalkSpeed;
  }
}


void ABasicEnemy::SetMaxHealth(float PercentageVariationMaxHealth)
{
  if (IsValid(HealthComponent))
  {
    HealthComponent->SetMaxHealth(HealthComponent->GetBaseMaxHealth() * (1 + PercentageVariationMaxHealth));
  }
}

void ABasicEnemy::SetMaxDamage(float PercentageVariationMaxDamage)
{
  if (IsValid(DamageComponent))
  {
    DamageComponent->SetDamage(DamageComponent->BaseDPSStrength * (1 + PercentageVariationMaxDamage), DamageComponent->BaseDamageStrength * (1 + PercentageVariationMaxDamage));
  }
}


void ABasicEnemy::SetMovementSpeed(float PercentageVariationSpeed)
{
  if (MovementComponent)
  {
    MovementComponent->MaxWalkSpeed *= (1 + PercentageVariationSpeed);
  }
}


void ABasicEnemy::TryToHit()
{

  if (DamageRangeCollision)
  {
    DamageRangeCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    //DamageRangeCollision->SetHiddenInGame(false);

    //DamageRangeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  }

}


void ABasicEnemy::EndTryToHit()
{
  if (DamageRangeCollision)
  {

    DamageRangeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    //DamageRangeCollision->SetHiddenInGame(true);
  }
}


void ABasicEnemy::PlayHitTurretSound()
{
  if (HitTurretsSounds.Num() > 0)
  {
    int32 N = 0;
    if (HitTurretsSounds.Num() > 1)
    {
      N = FMath::RandRange(0, HitTurretsSounds.Num() - 1);
    }

    if (HitTurretsSounds[N])
    {
      UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitTurretsSounds[N], GetActorLocation(), VolumeSound);
      /*AudioComponent->SetSound(HitTurretsSounds[N]);
      AudioComponent->Play();*/
    }
  }

}

void ABasicEnemy::PlayHitPlayerSound()
{
  if (HitPlayerSounds.Num() > 0)
  {
    int32 N = 0;
    if (HitPlayerSounds.Num() > 1)
    {
      N = FMath::RandRange(0, HitPlayerSounds.Num() - 1);
    }

    if (HitPlayerSounds[N])
    {
      UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitPlayerSounds[N], GetActorLocation(), VolumeSound);
      /*AudioComponent->SetSound(HitPlayerSounds[N]);
      AudioComponent->Play();*/
    }
  }
}

void ABasicEnemy::SetShowEnemyDamagePoints(bool bShowDamagePoints)
{
  if (bShowDamagePoints && HealthComponent)
  {
    if (!HealthComponent->DamageReceived.Contains(this, FName("DrawDamageWidget")))
    {
      HealthComponent->DamageReceived.AddDynamic(this, &ABasicEnemy::DrawDamageWidget);
    }
  }
  else
  {
    if (HealthComponent->DamageReceived.Contains(this, FName("DrawDamageWidget")))
    {
      HealthComponent->DamageReceived.Remove(this, FName("DrawDamageWidget"));
    }
  }
}


void ABasicEnemy::SetDeathBrainMode()
{
  if (IsValid(AICharacterController))
  {
    AICharacterController->GetBrainComponent()->StopLogic(FString("DeathBrainMode"));
    AICharacterController->GetBrainComponent()->Deactivate();
  }
}


void ABasicEnemy::SetPlatesVariation(int32 PlatesVariation)
{

  if (IsValid(RewardsComponent))
  {
    RewardsComponent->NumPlatesToDrop = PlatesVariation;
    RewardsComponent->bRandomPlates = false;
  }

}

void ABasicEnemy::SetUpgradesVariation(float UpgradesVariation)
{

  if (IsValid(RewardsComponent))
  {
    RewardsComponent->UpgradesProb = UpgradesVariation;

  }

}