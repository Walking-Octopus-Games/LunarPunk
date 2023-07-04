// Fill out your copyright notice in the Description page of Project Settings.


#include "Turrets/Projectiles/BeaconAreaProjectile.h"
#include "HealthSystem/DamageComponent.h"

ABeaconAreaProjectile::ABeaconAreaProjectile()
{
  PrimaryActorTick.bCanEverTick = true;
  UE_LOG(LogTemp, Warning, TEXT("Beacon"));
}

void ABeaconAreaProjectile::BeginPlay()
{
  Super::BeginPlay();
  UE_LOG(LogTemp, Warning, TEXT("Beacon Begin"));

  ProjectileMesh = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName("BeaconAreaProjectileMesh"));
  ProjectileMesh->OnComponentBeginOverlap.AddDynamic(this, &ABeaconAreaProjectile::OnOverlapBegin);
}

void ABeaconAreaProjectile::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
  this->GetActorScale();
  FVector GrowthRate = FVector(Speed, Speed, 0) * DeltaTime;
  UE_LOG(LogTemp, Warning, TEXT("Beacon Grow"));
  this->SetActorScale3D(GetActorScale() + GrowthRate);
  if (GetActorScale().X > MaxTam)
  {
    FinishEvent.Broadcast();
    Destroy();
  }
}

void ABeaconAreaProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  IEnemy* Enemy = Cast<IEnemy>(OtherActor);
  if ((Enemy != nullptr) && (OtherActor != this) && (OtherComp != NULL))
  {
    UHealthComponent* OverlapedHealthComponent = Enemy->GetHealthComponent();

    ApplyThrustImpact(OtherActor);

    this->DamageComponent->MakeDamage(OverlapedHealthComponent);
  }
}

void ABeaconAreaProjectile::ReturnToPool()
{
  Super::ReturnToPool();
}

void ABeaconAreaProjectile::Reactivate()
{
  Super::Reactivate();
}