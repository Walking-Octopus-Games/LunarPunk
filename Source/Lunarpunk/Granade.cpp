
#include "Granade.h"
#include "TimerManager.h"


#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Pool/Pool.h"


// Sets default values
AGranade::AGranade()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

  CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
  CollisionComponent->InitSphereRadius(15.0f);
  RootComponent = CollisionComponent;

  MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

  static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
  if (Mesh.Succeeded())
  {
    MeshComponent->SetStaticMesh(Mesh.Object);
  }
  MeshComponent->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));

  MeshComponent->SetupAttachment(RootComponent);
  MeshComponent->SetSimulatePhysics(false);

  ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
  ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);

  ProjectileMovementComponent->bIsSliding = true;
  ProjectileMovementComponent->BounceVelocityStopSimulatingThreshold = 100.0f;
  ProjectileMovementComponent->Bounciness = 0.3f;
  ProjectileMovementComponent->bShouldBounce = true;
  //ProjectileMovementComponent->InitialSpeed = GranadeDistance * 100;
  ProjectileMovementComponent->MaxSpeed = 1000.0f;
  ProjectileMovementComponent->ProjectileGravityScale = 1.0f;
  ProjectileMovementComponent->bRotationFollowsVelocity = true;

  ExplosionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent2"));
  ExplosionComponent->InitSphereRadius(0.0f);

  ExplosionComponent->SetupAttachment(MeshComponent);

  DamageComponent = CreateDefaultSubobject<UDamageComponent>(TEXT("DamageComponent"));
}


// Called when the game starts or when spawned
void AGranade::BeginPlay()
{
  Super::BeginPlay();

  ProjectileMovementComponent->InitialSpeed = GranadeDistance * 100;
  UE_LOG(LogTemp, Warning, TEXT("xxxx, %f "), ProjectileMovementComponent->InitialSpeed);

  //CollisionComponent->OnComponentHit.AddDynamic(this, &AGranade::OnHit);
  //CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AGranade::OnCollision);
  //GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGranade::Explode, Timetoxplode, false);
  Pool = Cast<APool>(UGameplayStatics::GetActorOfClass(GetWorld(), APool::StaticClass()));

  if (!IsValid(Pool))
  {
    UE_LOG(LogTemp, Error, TEXT("PlayableCharacter.cpp: No se ha encontrado Pool"));
  }

}

void AGranade::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
  Explode();
}

// Called every frame
void AGranade::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
}
void AGranade::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
  Explode();
}

void AGranade::Explode()
{
  //stop granada movement
  //ProjectileMovementComponent->MaxSpeed = 0.0f;

  //ExplosionComponent->InitSphereRadius(ExplosionRadius)
  //ExplosionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));

  //CollisionComponent = ExplosionComponent;

  for (float i = 0.0f; i <= ExplosionRadius; i = i + 1)
  {
    ExplosionComponent->SetSphereRadius(i);
    //UE_LOG(LogTemp, Warning, TEXT("Some warning message"));
  }
  //Update the explosion component when detect overlap

  //ExplosionComponent->OnComponentBeginOverlap.AddDynamic(this, &AGranade::OnOverlapBegin);
  //ExplosionComponent->SetSimulatePhysics(true);
  ExplosionComponent->SetGenerateOverlapEvents(true);

  TArray<AActor*> OverlappingActors;
  ExplosionComponent->GetOverlappingActors(OverlappingActors);

  if (OverlappingActors.Num() > 0)
  {


    for (auto& Class : OverlappingActors)
    {
      UHealthComponent* OverlapedHealthComponent = Cast<UHealthComponent>(Class->GetComponentByClass(UHealthComponent::StaticClass()));
      if (IsValid(OverlapedHealthComponent) && Class->IsA(ABasicEnemy::StaticClass()))
      {

        this->DamageComponent->MakeDamage(OverlapedHealthComponent);
      }

    }
  }
  else
  {
    // There are no actors inside the collision component
  }
  //UNiagaraComponent* explosion =    explosion.

  //ExplosionComponent->bHiddenInGame;
  UGameplayStatics::SpawnSoundAtLocation(this, ExplosionSound, ExplosionComponent->GetRelativeLocation());

  if (IsValid(NS_Explosion))
  {
    ShowParticle();
    //ReturnToPool();
    UE_LOG(LogTemp, Warning, TEXT("Sonido"));
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, NS_Explosion, ExplosionComponent->GetRelativeLocation(), ExplosionComponent->GetRelativeRotation());
  }
  else
  {
    UE_LOG(LogTemp, Warning, TEXT("NO SONIDO"));
  }
  //SetActorHiddenInGame(true);
}





void AGranade::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  UE_LOG(LogTemp, Warning, TEXT("Some warning message"));
  if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
  {
    UHealthComponent* OverlapedHealthComponent = Cast<UHealthComponent>(OtherActor->GetComponentByClass(UHealthComponent::StaticClass()));
    if (IsValid(OverlapedHealthComponent))
    {

      this->DamageComponent->MakeDamage(OverlapedHealthComponent);
    }

  }
}

void AGranade::ReturnToPool()
{
  if (IsValid(Pool))
  {
    SetActorLocation(Pool->GetActorLocation());
    Deactivate();
  }
}

void AGranade::setGranadeDistance(float value)
{
  GranadeDistance = value * 500.0f;



}

float AGranade::getGranadeDistance()
{
  return GranadeDistance;
}




void AGranade::FireInDirection(const FVector& ShootDirection)
{
  //UE_LOG(LogTemp, Warning, TEXT("Shoot"));
  ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
  ProjectileMovementComponent->UpdateComponentVelocity();

}


void AGranade::Reactivate()

{
  bIsActive = true;
  PrimaryActorTick.bCanEverTick = true;
  //SetActorHiddenInGame(false);

  if (IsValid(CollisionComponent))
  {
    CollisionComponent->SetNotifyRigidBodyCollision(true);
    CollisionComponent->SetGenerateOverlapEvents(true);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
  }


  if (IsValid(MeshComponent))
  {

    MeshComponent->Activate();
    //MeshComponent->SetVisibility(true);
  }

  if (IsValid(ProjectileMovementComponent))
  {
    ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
    //ProjectileMovementComponent->Velocity = ProjectileVelocity;
    ProjectileMovementComponent->InitialSpeed = GranadeDistance;
    //ProjectileMovementComponent->UpdateComponentVelocity();
    ProjectileMovementComponent->bSimulationEnabled = true;
    ProjectileMovementComponent->SetComponentTickEnabled(true);
    ProjectileMovementComponent->Activate();



  }


  if (IsValid(ExplosionComponent))
  {
    ExplosionComponent->Activate();
  }

  if (IsValid(DamageComponent))
  {
    DamageComponent->ResetComponent();
  }


  UE_LOG(LogTemp, Warning, TEXT("GranadeDistanceeeee, %f "), GranadeDistance);
  UE_LOG(LogTemp, Warning, TEXT("GranadeDistanceeeee2, %f "), getGranadeDistance());

  FReactivate.Broadcast(this);


}

void AGranade::Deactivate()
{
  bIsActive = false;
  SetActorTickEnabled(false);
  //SetActorHiddenInGame(true);

  PrimaryActorTick.bCanEverTick = false;
  if (IsValid(ProjectileMovementComponent))
  {
    ProjectileMovementComponent->SetUpdatedComponent(NULL);
    ProjectileMovementComponent->Velocity = FVector(0.0, 0.0, 0.0);
    ProjectileMovementComponent->UpdateComponentVelocity();
    ProjectileMovementComponent->bSimulationEnabled = false;
    ProjectileMovementComponent->SetComponentTickEnabled(false);
    ProjectileMovementComponent->Deactivate();
  }


  if (IsValid(CollisionComponent))
  {
    CollisionComponent->SetNotifyRigidBodyCollision(false);
    CollisionComponent->SetGenerateOverlapEvents(false);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  }

  if (IsValid(MeshComponent))
  {

    MeshComponent->Deactivate();
    //MeshComponent->SetVisibility(false);
  }

  if (IsValid(ExplosionComponent))
  {
    ExplosionComponent->Deactivate();
  }

  if (IsValid(DamageComponent))
  {
    DamageComponent->Deactivate();
  }
}

void AGranade::OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
  if (bIsActive)
  {
    Explode();
  }
}











