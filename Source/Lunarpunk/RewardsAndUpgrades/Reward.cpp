// Fill out your copyright notice in the Description page of Project Settings.


#include "Reward.h"
#include "GetRewardsComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include <Kismet/GameplayStatics.h>
#include "GameFramework/Character.h"
#include "Player/PlayableCharacter.h"
#include "Pool/Pool.h"
#include "Managers/PlatesManager.h"
#include "Plate.h"
#include "DrawDebugHelpers.h"
#include "FMODBlueprintStatics.h"
#include "UpgradesData.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "Managers/UpgradesManager.h"


// Sets default values
AReward::AReward()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

  Collision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
  Collision->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AReward::BeginPlay()
{
  Super::BeginPlay();

  StaticMesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));

  //Search the components and de player and check if they're not null.
  if (IsValid(Collision))
  {
    Collision->OnComponentBeginOverlap.AddDynamic(this, &AReward::OnBeginOverlap);
    //Collision->OnComponentEndOverlap.AddDynamic(this, &AReward::OnEndOverlap);
  }
  else
  {
    UE_LOG(LogTemp, Warning, TEXT("GetRewardsComponent.cpp: Collision not found."));
  }

  Player = Cast<APlayableCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayableCharacter::StaticClass()));

  if (Player == nullptr)
  {
    UE_LOG(LogTemp, Warning, TEXT("GetRewardsComponent.cpp: Player not found"));
  }

  //Obtain the pool reference.
  MyPool = Cast<APool>(UGameplayStatics::GetActorOfClass(GetWorld(), APool::StaticClass()));

  if (!IsValid(MyPool))
  {
    UE_LOG(LogTemp, Warning, TEXT("Reward.cpp: Pool not found"));
  }

  GameMode = Cast<ALunarPunkGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

}

// Called every frame
void AReward::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);


  if (GameMode->GetPlatesManager()->bRewardsCanDisapeared)
  {
    CurrentTime += DeltaTime;

    if (CurrentTime >= Lifetime)
    {
      bIsActive = false;
      ReturnToPool();
    }
  }


}


UClass* AReward::GetTypeOfReward()
{

  return AReward::StaticClass();
}



void AReward::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  //UE_LOG(LogTemp, Warning, TEXT("Cojo Reward."));
  if ((OtherActor != NULL) && OtherComp->GetClass() == UCapsuleComponent::StaticClass() && (OtherActor == Player))
  {

    GoToHUD(0);
    ReturnToPool();
    ReceiveByPlayerAudioInstance = UFMODBlueprintStatics::PlayEventAttached(ReceiveByPlayerAudioEvent, OtherActor->GetRootComponent(), NAME_None, OtherActor->GetActorLocation(), EAttachLocation::KeepWorldPosition, true, true, true);

  }
}


void AReward::AddToHUD(int32 QuantityToAdd)
{

  ReceiveByHUDAudioInstance = UFMODBlueprintStatics::PlayEventAttached(ReceiveByHUDAudioEvent, GetRootComponent(), NAME_None, GetActorLocation(), EAttachLocation::KeepWorldPosition, true, true, true);
  bIsActive = false;
  //Deactivate();
}


void AReward::BeingDrawIn(const FTransform& TransformActorDrawingIn, float Force)
{

  //The rewards go to the position of the actor which is drawing in
  FVector Direction = TransformActorDrawingIn.GetLocation() + FVector(0.0, 0.0, 20.0) - GetActorLocation();
  Direction.Normalize();
  Direction *= 20 * Force;
  AddActorWorldOffset(Direction);

};

void AReward::ReturnToPool()
{
  if (IsValid(MyPool))
  {
    Deactivate();
    SetActorLocation(MyPool->GetActorLocation());
  }
}

void AReward::Deactivate()
{
  SetActorTickEnabled(false);
  SetActorHiddenInGame(true);
  
  //Set false when added to hud
  //bIsActive = false;
  CurrentTime = 0;

  if (IsValid(StaticMesh))
  {
    StaticMesh->SetVisibility(false);
    StaticMesh->Deactivate();
  }

  if (IsValid(Collision))
  {
    Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  }


}


void AReward::Reactivate()
{
  SetActorTickEnabled(true);
  SetActorHiddenInGame(false);
  bIsActive = true;
  if (IsValid(Collision))
  {
    Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
  }

  if (IsValid(StaticMesh))
  {
    StaticMesh->Activate();
    StaticMesh->SetVisibility(true);
  }

  FReactivate.Broadcast(this);
}



