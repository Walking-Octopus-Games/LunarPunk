// Fill out your copyright notice in the Description page of Project Settings.


#include "Sector.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Enemies/IEnemy.h"
#include "EnemySpawner/EnemySpawner.h"
#include "HealthSystem/HealthComponent.h"
#include "Player/PlayableCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"
#include "Portal/Portal.h"
#include "Turrets/Turret.h"

// Sets default values
ASector::ASector()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

}



// Called when the game starts or when spawned
void ASector::BeginPlay()
{
  Super::BeginPlay();

  SectorCollision = FindComponentByClass<UBoxComponent>();

  if (IsValid(SectorCollision))
  {
    SectorCollision->OnComponentBeginOverlap.AddDynamic(this, &ASector::OnBeginOverlap);
    SectorCollision->OnComponentEndOverlap.AddDynamic(this, &ASector::OnEndOverlap);
    //Put off the collision until it'll be neccesary (when the sector is opened).
    SectorCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  }
  else
  {
    UE_LOG(LogTemp, Warning, TEXT("Sector.cpp: No se ha encontrado el BoxCollision en el blueprint."));
  }



  SectorExit = FindComponentByClass<UCapsuleComponent>();
  if (IsValid(SectorExit))
  {
      SectorExit->OnComponentBeginOverlap.AddDynamic(this, &ASector::OnBeginOverlapExit);
      //Put off the collision until it'll be neccesary (when the sector is opened).
      SectorExit->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  }
  else
  {
      UE_LOG(LogTemp, Warning, TEXT("Sector.cpp: No se ha encontrado el BoxCollision en el blueprint."));
  }


  TurretsZone = FindComponentByClass<USphereComponent>();
  if (!IsValid(TurretsZone))
  {
    UE_LOG(LogTemp, Warning, TEXT("Sector.cpp: No se ha encontrado el SphereCollision en el blueprint."))
  }

  //Obtain the enemy spawners with the samen index.
  GetEnemySpawnersAsociated();

  CurrentTimeToEndResting = RestingTime;
}

// Called every frame
void ASector::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (bActive)
  {
    //Control when the sector is empty and count the time.
    if (bStarted && !bEnemiesInSector)
    {
      CurrentTimeWithoutEnemies += DeltaTime;
      //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Tiempo sin enemigos %f"), CurrentTimeWithoutEnemies));
    }

    //Count the resting time
    if (!bStarted)
    {
      if (CurrentTimeToEndResting >= 0)
      {
        CurrentTimeToEndResting -= DeltaTime;
      }

    }

    //Win condition
    if (CurrentTimeWithoutEnemies >= TimeToConquer)
    {
      DeactivateAllEnemiesInMap();
      CloseSector();
      //UE_LOG(LogTemp, Warning, TEXT("Sector.cpp: TIEMPO SIN ENEMIGOS CUMPLIDO."))
    }
  }

}

void ASector::DeactivateAllEnemiesInMap() const
{
  TArray<AActor*> EnemiesInMap;
  UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UEnemy::StaticClass(), EnemiesInMap);
  for (AActor* AEnemyActor : EnemiesInMap)
  {
      //Check if the enemy is the Portal. If not, try to kill all the active enemies.
      const APortal* Portal = Cast <APortal>(AEnemyActor);
      if (Portal == nullptr)
      {
          if (Cast<IIsPoolable>(AEnemyActor)->bIsActive == true)
          {
              IEnemy* Enemy = Cast<IEnemy>(AEnemyActor);
              if (Enemy != nullptr)
              {

                  UHealthComponent* HealthComponent = Enemy->GetHealthComponent();
                  if (IsValid(HealthComponent))
                  {
                      HealthComponent->ReceiveDamage(HealthComponent->GetHealth());
                  }
              }
          }
          
      }
  }
}
void ASector::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  IEnemy* ActorOverlaping = Cast<IEnemy>(OtherActor);

  if (ActorOverlaping != nullptr)
  {
    //The action into the sector begins when the first enemy enters in it.
    if (!bStarted)
    {
      bStarted = true;
      //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Entra el primer enemigo.");

    }

    //Clear the time without enemies if one of them enters in the sector.
    if (!bEnemiesInSector)
    {
      CurrentTimeWithoutEnemies = 0;
    }
    bEnemiesInSector = true;
    ++CurrentEnemiesInSector;
    //GEngine->AddOnScreenDebugMessage(- 1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Enemigos: %i"), CurrentEnemiesInSector));
  }

}


void ASector::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

    if (IsValid(OtherActor))
    {
        IEnemy* ActorOverlaping = Cast<IEnemy>(OtherActor);

        if (ActorOverlaping != nullptr)
        {
            --CurrentEnemiesInSector;
            //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, FString::Printf(TEXT("Enemigos: %i"), CurrentEnemiesInSector));
            if (CurrentEnemiesInSector == 0)
            {
                bEnemiesInSector = false;
            }
        }

    }

}


void ASector::OnBeginOverlapExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    
    if (Cast<APlayableCharacter>(OtherActor))
    {
        UpWall();
    }
    

}


void ASector::OpenSector()
{
  //Activate the collision and the enemy spawnaers asociated.
  SectorCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
  ActivateEnemySpawnersAsociated();
  bActive = true;

  //Move the turrets to the front line.
  if (Index != 0)
  {
    TArray<AActor*> Turrets;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATurret::StaticClass(), Turrets);
    FVector Location = TurretsZone->GetComponentLocation();
    for (AActor* Turret : Turrets)
    {
        ATurret* TurretCasted = Cast<ATurret>(Turret);
      if (IsValid(TurretCasted))
      {
          if (TurretCasted->State != ETurretState::PickedUp)
          {
            Turret->SetActorLocation(Location);
            Location += FVector(0.0, 200.0, 0.0);
          }
        
      }
    }
  }
  ChangeMaterial(1);

}


void ASector::CloseSector()
{

  //SectorCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  DeactivateEnemySpawnersAsociated();
  CurrentTimeWithoutEnemies = 0;
  ChangeMaterial(0);
  bStarted = false;
  bActive = false;
  //Activate the exit of the sector
  SectorExit->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

  //Announces that the sector closes and says which is the next sector to be opened.
  SectorClosed.Broadcast(Index + 1);

 /* if (!IsOverlappingActor(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
  {
      UpWall();
  }*/


  //If this is the last sector, manage the end of the level.
  if (bIsLastSector)
  {
    OnLastSectorCompleted();
  }
  else
  {
      TArray<UActorComponent*> Arrows = GetComponentsByTag(UArrowComponent::StaticClass(), FName("Arrow"));
      for (auto& Arrow : Arrows)
      {
          Cast<UArrowComponent>(Arrow)->SetVisibility(true);
      }
  }
}

void ASector::EnemyGotOut()
{
  --CurrentEnemiesInSector;
  if (CurrentEnemiesInSector == 0)
  {
    bEnemiesInSector = false;
  }
}



void ASector::GetEnemySpawnersAsociated()
{
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawner::StaticClass(), EnemySpawners);
  if (EnemySpawners.Num() == 0)
  {
    UE_LOG(LogTemp, Warning, TEXT("Sector.cpp: No se han encontrado EnemySpanwers asociados a este sector."))
  }
}


void ASector::ActivateEnemySpawnersAsociated()
{
  for (AActor* EnemySpawner : EnemySpawners)
  {
    if (IsValid(EnemySpawner))
    {
      //Search the enemy spawners with the same index as the sector, activate them and set the time to start spawning enemies.
      AEnemySpawner* EnemySpawnerCasted = Cast<AEnemySpawner>(EnemySpawner);
      if (IsValid(EnemySpawnerCasted) && EnemySpawnerCasted->OwnerSectorIndex == Index)
      {
        //if (EnemySpawnerCasted->WavesData->Waves.Num() > 0)
        //{
        //  EnemySpawnerCasted->WavesData->Waves[0].TimeToStart = RestingTime;
        //}
        ////EnemySpawnerCasted->ActivateSpawner();

      }
    }

  }
}

void ASector::DeactivateEnemySpawnersAsociated()
{
    for (AActor* EnemySpawner : EnemySpawners)
    {
        if (IsValid(EnemySpawner))
        {
            //Search the enemy spawners with the same index as the sector and deactivate them
            AEnemySpawner* EnemySpawnerCasted = Cast<AEnemySpawner>(EnemySpawner);
            if (IsValid(EnemySpawnerCasted) && EnemySpawnerCasted->OwnerSectorIndex == Index)
            {
                EnemySpawnerCasted->DeactivateSpawner();

            }
        }

    }
}



float ASector::GetTimeToConquer()
{
  /*UE_LOG(LogTemp, Warning, TEXT("Time CurrentTimeWithoutEnemies in ASECTOR: %f"), CurrentTimeWithoutEnemies);
  UE_LOG(LogTemp, Warning, TEXT("Time win in ASECTOR: %f"),  TimeToWin);*/
  return CurrentTimeWithoutEnemies / TimeToConquer;
}

float ASector::GetCurrentTimeEndResting()
{
  return CurrentTimeToEndResting;
}
