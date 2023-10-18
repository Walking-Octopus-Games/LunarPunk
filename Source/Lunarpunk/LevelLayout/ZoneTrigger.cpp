// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelLayout/ZoneTrigger.h"

#include "ForkComponent.h"
#include "Components/BoxComponent.h"
#include "Player/PlayableCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Turrets/Turret.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AZoneTrigger::AZoneTrigger()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;
  ForkComponent = CreateDefaultSubobject<UForkComponent>(TEXT("ForkComponent"));

  DefaultRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
  SetRootComponent(DefaultRootComponent);


  BlockCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision Component"));
  ZoneTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneTrigger"));
  BlockCollisionComponent->SetupAttachment(RootComponent);
  ZoneTrigger->SetupAttachment(RootComponent);
  FogEffect = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Fog Effect Component"));
  FogEffect->SetupAttachment(GetRootComponent());
  //CollisionComponent->SetCollisionObjectType(ECC_PhysicsBody);
}
//
//void AZoneTrigger::Initialize()
//{
//	//TriggerActivated.AddDynamic()
//}

// Called when the game starts or when spawned
void AZoneTrigger::BeginPlay()
{
  Super::BeginPlay();


  if (!IsValid(ZoneTrigger))
  {
    UE_LOG(LogTemp, Warning, TEXT("TurretConquestPlace.cpp: collision not found"));
  }
  else
  {
    ZoneTrigger->OnComponentBeginOverlap.AddDynamic(this, &AZoneTrigger::OnBeginOverlap);
    ZoneTrigger->OnComponentBeginOverlap.AddDynamic(ForkComponent, &UForkComponent::OnBeginOverlap);
  }

  PlayerReference = Cast<APlayableCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));


  
  //Deactivate the collision and the fog effect.
  //ZoneTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);



  if (IsValid(FogEffect))
  {
    FogEffect->SetVisibility(bPersistentFog);
  }

  ForkComponent->CloseDoors();

}

// Called every frame
void AZoneTrigger::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

}



void AZoneTrigger::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  if (IsValid(PlayerReference) && OtherComp->IsA(UCapsuleComponent::StaticClass()) && (PlayerReference == OtherActor || OtherActor->IsA<ATurret>()))
  {
    //TriggerActivated.Broadcast();
    DeactivateZoneTrigger();

  }
}


void AZoneTrigger::DeactivateZoneTrigger()
{
  //Notify that the trigger has been overlapped.
  TriggerActivated.Broadcast(bDelaySpawnersIfOverlapZoneTrigger, DelaySpawnersGetActivated);

  //Deactivate the collision and the fog effect.
  ZoneTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);

  FogEffect->SetVisibility(false);

  ForkComponent->ActivateForkComponent();
}

void AZoneTrigger::ActivateZoneTrigger()
{
  ZoneTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

  FogEffect->SetVisibility(true);

  if (IsValid(ForkComponent))
  {
    ForkComponent->OpenDoors();
  }
  //BlockCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}