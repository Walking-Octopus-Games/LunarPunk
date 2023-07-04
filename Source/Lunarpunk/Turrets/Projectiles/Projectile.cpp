// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "HealthSystem/VampirismComponent.h"
#include "Components/SphereComponent.h"
#include "HealthSystem/DamageComponent.h"
#include "HealthSystem/HealthComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Pool/Pool.h"
#include "Player/PlayableCharacter.h"
#include "Enemies/IEnemy.h"
#include <Kismet/GameplayStatics.h>
#include "Turrets/ProjectileTurret.h"
#include "Enemies/AI/EnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AProjectile::AProjectile()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

  // Create Sphere CollisionComponent
  CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));

  // Set the sphere's collision radius
  CollisionComponent->InitSphereRadius(15.0f);

  // Set the root component to be the collision component
  RootComponent = CollisionComponent;

  // Create Projectile movement component
  ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
  ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
  ProjectileMovementComponent->bRotationFollowsVelocity = true;
  ProjectileMovementComponent->bShouldBounce = false;
  ProjectileMovementComponent->Bounciness = 0.0f;
  ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

  // Create projectile mesh
  ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
  static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
  if (Mesh.Succeeded())
  {
    ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
  }

  //Set projectile material
  static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
  if (Material.Succeeded())
  {
    ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
  }
  ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
  ProjectileMeshComponent->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
  ProjectileMeshComponent->SetupAttachment(RootComponent);

  //Create Projectile damage component
  DamageComponent = CreateDefaultSubobject<UDamageComponent>(TEXT("DamageComponent"));

  // Set Collision profile
  CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
  CollisionComponent->SetSimulatePhysics(false);

  //Simulate generate hit events
  CollisionComponent->SetNotifyRigidBodyCollision(false);
  CollisionComponent->SetGenerateOverlapEvents(false);

  // Event called when component hits something.
  CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);
  CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
  Super::BeginPlay();

  //Obtain the pool reference.
  MyPool = Cast<APool>(UGameplayStatics::GetActorOfClass(GetWorld(), APool::StaticClass()));

  if (!IsValid(MyPool))
  {
    UE_LOG(LogTemp, Warning, TEXT("Projectile.cpp: No se encontro la Pool"));
  }

}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
}

void AProjectile::ShootProjectile(const FVector& ShootLocation, FRotator ShootRotation, const float& Power, const float& MaxShootDistance, AProjectileTurret* Turret, const AActor* Target)
{
  if (IsValid(Turret))
  {
    FiringTurret = Turret;
  }

  SetDistanceOfFire(MaxShootDistance);

  //Sets location and rotation.
  SetActorLocation(ShootLocation);
  SetActorRotation(ShootRotation);

  //Reactivate the projectile
  Reactivate();

  //Shoot the projectile
  // Set the projectile's initial trajectory.
  DamageComponent->SetDamage(Power, Power);
  FVector LaunchDirection = ShootRotation.Vector();

  if (bIsParabolicProjectile && IsValid(Target))
  {
    ProjectileMovementComponent->SetVelocityInLocalSpace(CalculateVelocity(
      ShootLocation,
      Target
    ));
  }
  else
  {
    FireInDirection(LaunchDirection);
  }
}

// Function that initializes the projectile's velocity in the shoot direction.
void AProjectile::FireInDirection(const FVector& ShootDirection)
{
  //UE_LOG(LogTemp, Warning, TEXT("Shoot"));
  ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
  ProjectileMovementComponent->UpdateComponentVelocity();

}

FVector AProjectile::CalculateVelocity(const FVector& ShootLocation, const AActor* Target)
{
  float angleRad = (LaunchingAngle * PI / 180); // launch angle in radians
  FVector direction = Target->GetActorLocation() - ShootLocation;
  float Height = direction.Z;// height difference
  direction.Z = 0;
  float Distance = direction.Size();// distance
  float gravity = GetWorld()->GetGravityZ() * -1;
  float V = (Distance / cos(angleRad)) * FMath::Sqrt((gravity * 1) / (2 * (Distance * tan(angleRad) - Height)));
  FVector VelocityOutput = (FVector(V * cos(angleRad), 0, V * sin(angleRad)));


  // Show the projectile path
  /*FPredictProjectilePathParams PathParams;
  PathParams.StartLocation = ShootLocation;
  PathParams.LaunchVelocity = VelocityOutput;
  PathParams.ProjectileRadius = CollisionComponent->GetScaledSphereRadius(;

  FPredictProjectilePathResult PredictResult;
  bool a = UGameplayStatics::PredictProjectilePath(Target, PathParams, PredictResult);
  for (auto point : PredictResult.PathData)
  {
    DrawDebugPoint(GetWorld(), point.Location, 5.0f, FColor::Yellow, false, 0.5);
  }*/

  return VelocityOutput;
}

// Function called when the projectile hits something.
void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    SpawnImpactEffect(GetActorLocation());
    ReturnToPool();
}

// Function called when the projectile overlaps something
void AProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  if (!ActorsHitted.Contains(OtherActor))
  {

    if (TargetMode == EProjectileTargetMode::Default)
    {
      IEnemy* Enemy = Cast<IEnemy>(OtherActor);
      if ((Enemy != nullptr) && (OtherActor != this) && (OtherComp != NULL))
      {
        UHealthComponent* OverlapedHealthComponent = Enemy->GetHealthComponent();
        if (IsValid(OverlapedHealthComponent))
        {
          ApplyThrustImpact(OtherActor);
          DamageComponent->MakeDamage(OverlapedHealthComponent);
          if (IsValid(FiringTurret))
          {
            FiringTurret->TurretMakeDamage.Broadcast(FiringTurret->TypeOfTurret, DamageComponent->DamageStrength);
          }
          ActorsHitted.Add(OtherActor);
        }
        SpawnImpactEffect(GetActorLocation());
      }
    }

    if (TargetMode == EProjectileTargetMode::Enemy)
    {
      if (OtherActor->IsA<ATurret>() || OtherActor->IsA<APlayableCharacter>())
      {
        UHealthComponent* OverlappedHealthComponent = OtherActor->FindComponentByClass<UHealthComponent>();
        if (IsValid(OverlappedHealthComponent))
        {
          DamageComponent->MakeDamage(OverlappedHealthComponent);
          if (IsValid(FiringTurret))
          {
            FiringTurret->TurretMakeDamage.Broadcast(FiringTurret->TypeOfTurret, DamageComponent->DamageStrength);
          }
          ActorsHitted.Add(OtherActor);
        }

        SpawnImpactEffect(GetActorLocation());
      }
    }

    if (bDisappearAfterFirstHit)
    {
      SpawnImpactEffect(GetActorLocation());
      ReturnToPool();
    }
  }
}

void AProjectile::ApplyThrustImpact(AActor* EnemyActor)
{
  IEnemy* Enemy = Cast<IEnemy>(EnemyActor);
  UHealthComponent* OverlapedHealthComponent = Enemy->GetHealthComponent();

  if (IsValid(OverlapedHealthComponent))
  {
    UCharacterMovementComponent* EnemyCharacterMovementComponent = Cast<UCharacterMovementComponent>(EnemyActor->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
    if (IsValid(EnemyCharacterMovementComponent))
    {
      APawn* Pawn = Cast<APawn>(EnemyActor);
      AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(Pawn->GetController());

      if (IsValid(EnemyAIController))
      {
        DamageComponent->ApplyThrustImpact(GetActorForwardVector(), ThrustImpactForce, EnemyCharacterMovementComponent, EnemyAIController);
      }
    }
  }
}

float AProjectile::GetDamageStrength()
{
  return this->DamageComponent->DamageStrength;
}

void AProjectile::ReturnToPool()
{
  ActorsHitted.Empty();
  ReturnPoolEvent.Broadcast();
  this->ReturnPoolEvent.Clear();
  if (IsValid(MyPool))
  {
    SetActorLocation(MyPool->GetActorLocation() + FVector(200.0, 0.0, 0.0));
    Deactivate();
  }
}

void AProjectile::Deactivate()
{

  bIsActive = false;
  SetActorTickEnabled(false);
  SetActorHiddenInGame(true);

  TargetMode = EProjectileTargetMode::Default;

  FiringTurret = false;

  if (IsValid(ProjectileMovementComponent))
  {
    ProjectileMovementComponent->SetUpdatedComponent(NULL);
    ProjectileMovementComponent->Velocity = FVector(0.0, 0.0, 0.0);
    ProjectileMovementComponent->UpdateComponentVelocity();
    ProjectileMovementComponent->bSimulationEnabled = false;
    ProjectileMovementComponent->SetComponentTickEnabled(false);
    ProjectileMovementComponent->Deactivate();
    ProjectileMovementComponent->ProjectileGravityScale = 0.f;
  }

  if (IsValid(CollisionComponent))
  {

    CollisionComponent->SetNotifyRigidBodyCollision(false);
    CollisionComponent->SetGenerateOverlapEvents(false);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  }

  if (IsValid(ProjectileMeshComponent))
  {
    ProjectileMeshComponent->Deactivate();
  }


  if (IsValid(DamageComponent))
  {
    DamageComponent->Deactivate();
  }


}

void AProjectile::Reactivate()
{
  GetWorld()->GetTimerManager().SetTimer(FireTimerHandler, this, &AProjectile::ReturnToPool, TimeToReturnProjectileToPool, false);

  SetActorTickEnabled(true);
  SetActorHiddenInGame(false);
  bIsActive = true;

  if (IsValid(CollisionComponent))
  {
    CollisionComponent->SetNotifyRigidBodyCollision(true);
    CollisionComponent->SetGenerateOverlapEvents(true);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
  }

  if (IsValid(ProjectileMovementComponent))
  {
    ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
    ProjectileMovementComponent->bSimulationEnabled = true;
    ProjectileMovementComponent->SetComponentTickEnabled(true);
    ProjectileMovementComponent->Activate();
    if (bIsParabolicProjectile)
    {
        ProjectileMovementComponent->ProjectileGravityScale = 1.f;
    }
  }

  if (IsValid(ProjectileMeshComponent))
  {
    ProjectileMeshComponent->Activate(true);
  }

  if (IsValid(DamageComponent))
  {
    DamageComponent->Activate(true);
    DamageComponent->ResetComponent();
  }

  FReactivate.Broadcast(this);
}

void AProjectile::SetDistanceOfFire(const float& _DistanceOfFire)
{
  this->DistanceOfFire = _DistanceOfFire;
  TimeToReturnProjectileToPool = _DistanceOfFire / ProjectileMovementComponent->MaxSpeed;
}

