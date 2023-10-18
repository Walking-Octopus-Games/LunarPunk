// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelLayout/TurretConquestPlace.h"
#include "Turrets/Turret.h"
#include "Components/BoxComponent.h"
#include "Turrets/AI/MovementTurretComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"


// Sets default values
ATurretConquestPlace::ATurretConquestPlace()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATurretConquestPlace::BeginPlay()
{
  Super::BeginPlay();


  TurretPlaceCollision = FindComponentByClass<UBoxComponent>();
  if (!IsValid(TurretPlaceCollision))
  {
    UE_LOG(LogTemp, Warning, TEXT("TurretConquestPlace.cpp: collision not found"));
  }
  else
  {
    TurretPlaceCollision->OnComponentBeginOverlap.AddDynamic(this, &ATurretConquestPlace::OnBeginOverlap);
    TurretPlaceCollision->OnComponentEndOverlap.AddDynamic(this, &ATurretConquestPlace::OnEndOverlap);
  }

  PlaceMesh = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(FName("PlaceBase")));

  if (!IsValid(PlaceMesh))
  {
    UE_LOG(LogTemp, Warning, TEXT("TurretConquestPlace.cpp: mesh not found"));
  }

  ActivatedEffect = Cast<UNiagaraComponent>(GetDefaultSubobjectByName(FName("ActivatedVisualEffect")));



}

// Called every frame
void ATurretConquestPlace::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (bGoingToCenter && IsValid(TurretPlacedReference) && PlaceMesh)
  {
    FVector CenterDirection = PlaceMesh->GetComponentLocation() + Offset - TurretPlacedReference->GetActorLocation();
    if (CenterDirection.SizeSquared() >= 100.0)
    {
      TurretPlacedReference->AddActorWorldOffset(CenterDirection * AdjustToCenterSpeed * DeltaTime);
    }
    else
    {
      bGoingToCenter = false;
    }

  }

}



void ATurretConquestPlace::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

  if (!bTurretIsPlaced && OtherActor->IsA<ATurret>())
  {

    TurretPlaced.Broadcast();
    ShowZoneActivatedEffect();
    if (ActivatedEffect)
    {
        ActivatedEffect->ActivateSystem();
    }
    RemoveTurretGhost();
    TurretPlacedReference = Cast<ATurret>(OtherActor);
    if (TurretPlacedReference->State == ETurretState::Moving)
    {
      TurretPlacedReference->TurretMovementComponent->AIController->StopMovement();
      TurretPlacedReference->TurretMovementComponent->MovementState = ETurretMovementState::Landed;
      TurretPlacedReference->StopMovementEffect();
      TurretPlacedReference->TurretMovementComponent->TurretStopMovingEvent.Broadcast();

    }
    TurretPlacedReference->TurretDeath.AddDynamic(this, &ATurretConquestPlace::PlacedTurretDeath);
    TurretPlacedReference->TurretReactivated.AddDynamic(this, &ATurretConquestPlace::PlacedTurretReactivated);

    bTurretIsPlaced = true;

    TurretPlacedReference->State = ETurretState::Conquest;
    //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Turret posicionada");

    bGoingToCenter = true;
  }


}


void ATurretConquestPlace::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

  if (TurretPlacedReference != nullptr && OtherActor == TurretPlacedReference)
  {
    TurretRetired.Broadcast();
    RemoveZoneActivatedEffect();
    if (ActivatedEffect)
    {
        ActivatedEffect->DeactivateImmediate();
    }
    ShowTurretGhost();
    TurretPlacedReference->TurretDeath.Remove(this, FName("PlacedTurretDeath"));
    TurretPlacedReference->TurretReactivated.Remove(this, FName("PlacedTurretReactivated"));
    TurretPlacedReference = nullptr;
    bTurretIsPlaced = false;
    //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Turret retirada");
    bGoingToCenter = false;
  }

}




void ATurretConquestPlace::PlacedTurretDeath()
{
  //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Murio torreta posicionada");
  TurretRetired.Broadcast();
  SetTurretDeathBaseMaterial(true);
  //RemoveZoneActivatedEffect();
  if (ActivatedEffect)
  {
      ActivatedEffect->DeactivateImmediate();
  }
  bGoingToCenter = false;
}

void ATurretConquestPlace::PlacedTurretReactivated()
{
  //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Revivio torreta posicionada");
  TurretPlaced.Broadcast();
  ShowZoneActivatedEffect();
  SetTurretDeathBaseMaterial(false);
  if (ActivatedEffect)
  {
      ActivatedEffect->ActivateSystem();
  }
  bGoingToCenter = true;
}



void ATurretConquestPlace::CloseConquestPlace()
{
  if (IsValid(TurretPlacedReference))
  {
    TurretPlacedReference->TurretDeath.Remove(this, FName("PlacedTurretDeath"));
    TurretPlacedReference->TurretReactivated.Remove(this, FName("PlacedTurretReactivated"));
    TurretPlacedReference->State = ETurretState::Set;
    TurretPlacedReference = nullptr;
  }

  RemoveZoneActivatedEffect();
  if (ActivatedEffect)
  {
      ActivatedEffect->DeactivateImmediate();
  }
  TurretPlaceCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}



void ATurretConquestPlace::DisableTurretPlace()
{
  RemoveZoneActivatedEffect();
  if (ActivatedEffect)
  {
      ActivatedEffect->DeactivateImmediate();
  }
  
  TurretPlaceCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  RemoveTurretGhost();
}


void ATurretConquestPlace::EnableTurretPlace()
{
  //ShowZoneActivatedEffect();
  ShowTurretGhost();
  TurretPlaceCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}