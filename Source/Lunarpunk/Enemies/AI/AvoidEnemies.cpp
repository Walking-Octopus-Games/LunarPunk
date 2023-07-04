// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/AvoidEnemies.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Enemies/BasicEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UAvoidEnemies::UAvoidEnemies()
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;



  // ...
}

void UAvoidEnemies::DecreaseEnemiesSpeed()
{
  TArray<UCharacterMovementComponent*> ToRemove;
  for (UCharacterMovementComponent* Charm : OverlappedEntryCharacters)
  {
    if (Charm->MaxWalkSpeed > MinVelocity)
    {
      Charm->MaxWalkSpeed = Charm->MaxWalkSpeed - AvoidanceSpeed;
    }
    else
    {
      ToRemove.Add(Charm);
    }
  }
  for (UCharacterMovementComponent* Charm : ToRemove)
  {
    OverlappedEntryCharacters.Remove(Charm);
  }
}

void UAvoidEnemies::OnBeginOverlapAvoidRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult)
{
  ABasicEnemy* OtherEnemy = Cast<ABasicEnemy>(OtherActor);

  if (OtherEnemy && OtherEnemy != GetOwner() && !OverlappedEnemies.Contains(OtherEnemy) && OtherComp->IsA(UCapsuleComponent::StaticClass()))
  {
    OverlappedEnemies.Add(OtherEnemy);
    UCharacterMovementComponent* Charm = Cast<UCharacterMovementComponent>(OtherActor->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
    if (Charm)
    {
      OverlappedEntryCharacters.Add(Charm);
      //MaxSpeedConstant = Charm->MaxWalkSpeed;
      /*  Charm->Velocity = Charm->Velocity - AvoidanceSpeed;
        Charm->MaxAcceleration = Charm->MaxAcceleration - AvoidanceSpeed;*/
        //Charm->MaxWalkSpeed = AvoidanceSpeed;
      CharacterComponent = Charm;
      //Charm->Velocity = Charm->Velocity - AvoidanceSpeed;
      GetWorld()->GetTimerManager().ClearTimer(DecreaseEnemySpeed);
      /*while (Charm->Velocity != MinVelocity)
      {*/
      GetWorld()->GetTimerManager().SetTimer(DecreaseEnemySpeed, this, &UAvoidEnemies::DecreaseEnemiesSpeed, 0.5f, false);
      //}

    }
  }

}

void UAvoidEnemies::OnEndOverlapAvoidRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
  ABasicEnemy* OtherEnemy = Cast<ABasicEnemy>(OtherActor);

  if (OtherEnemy && OverlappedEnemies.Contains(OtherEnemy) && OtherComp->IsA(UCapsuleComponent::StaticClass()))
  {
    OverlappedEnemies.Remove(OtherEnemy);
    UCharacterMovementComponent* Charm = Cast<UCharacterMovementComponent>(OtherActor->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
    if (Charm)
    {
      if (OverlappedEntryCharacters.Contains(Charm))
      {
        OverlappedEntryCharacters.Remove(Charm);
      }
      UAvoidEnemies* AvoidEnemies = Cast<UAvoidEnemies>(OtherActor->GetComponentByClass(UAvoidEnemies::StaticClass()));
      Charm->MaxWalkSpeed = AvoidEnemies->OriginalVelocity;
    }
  }

}


// Called when the game starts
void UAvoidEnemies::BeginPlay()
{
  Super::BeginPlay();

  UCharacterMovementComponent* Charm = Cast<UCharacterMovementComponent>(GetOwner()->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
  if (Charm)
  {
    OriginalVelocity = Charm->MaxWalkSpeed;
  }
  // ...
  if (AvoidSphereCollision)
  {
    AvoidSphereCollision->OnComponentBeginOverlap.AddDynamic(this, &UAvoidEnemies::OnBeginOverlapAvoidRange);
    AvoidSphereCollision->OnComponentEndOverlap.AddDynamic(this, &UAvoidEnemies::OnEndOverlapAvoidRange);

    //AvoidSphereCollision->SetSphereRadius(AvoidanceRadius);

    if (!bAvoidOtherEnemies)
    {
      AvoidSphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
  }

}


// Called every frame
void UAvoidEnemies::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // ...
  //Manage the turret avoidance
  if (bAvoidOtherEnemies)
  {
    FVector ResultantOffset = FVector(0.f, 0.f, 0.f);

    for (ABasicEnemy* OthereEnemy : OverlappedEnemies)
    {
      FVector Direction = (OthereEnemy->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
      Direction.Z = 0;
      FVector OwnVel = GetOwner()->GetVelocity().GetSafeNormal();
      OwnVel.Z = 0;
      FVector RotatedRightDirection = FRotator(0.0, 90.0f, 0.0).RotateVector(Direction);
      float AngleForward = FVector::DotProduct(OwnVel, Direction);
      float AngleRight = FVector::DotProduct(OwnVel, RotatedRightDirection);

      if (AngleForward > 0)
      {
        if (AngleRight > 0)
        {
          ResultantOffset += (RotatedRightDirection - Direction) * AvoidanceOffset * DeltaTime;
        }
        else
        {
          ResultantOffset -= (RotatedRightDirection + Direction) * AvoidanceOffset * DeltaTime;
        }
      }

    }

    GetOwner()->AddActorWorldOffset(ResultantOffset);

  }
}

