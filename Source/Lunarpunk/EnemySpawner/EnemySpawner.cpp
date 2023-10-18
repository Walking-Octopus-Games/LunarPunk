// Fill out your copyright notice in the Description page of Project Settings.


// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "Components/BoxComponent.h"
#include "Pool/Pool.h"
#include "Pool/IsPoolable.h"
#include "HealthSystem/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enemies/IEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Portal/Portal.h"
#include <ZigZagPawn.h>
#include <Enemies/BasicEnemy.h>
#include "LevelLayout/ConquestZone.h"
#include "FMODBlueprintStatics.h"


AEnemySpawner::AEnemySpawner()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
  Super::BeginPlay();


  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

  //Find the Pool
  if (MyPool == nullptr)
  {
    MyPool = Cast<APool>(UGameplayStatics::GetActorOfClass(GetWorld(), APool::StaticClass()));
  }


  //Sets the spawn area.
  SpawnArea = FindComponentByClass<UBoxComponent>();
  if (!SpawnArea)
  {
    UE_LOG(LogTemp, Warning, TEXT("EnemySpawner.cpp: No se ha establecido la Box Component en el Blueprint"));
  }

  if (NumWavesToRepeatAtEnd > WavesData->Waves.Num())
  {
    NumWavesToRepeatAtEnd = WavesData->Waves.Num();
  }
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);




  if (!bDeactivateForDebug)
  {


    if (bIsActive && !bStarted)
    {
      bStarted = true;
      StartNewWave();
      //¿Desactivar el tick?
    }
  }
}

void AEnemySpawner::StartNewWave()
{


  //Checks if can start a new wave
  if (WavesData->Waves.Num() > 0 && CurrentWaveCounter < WavesData->Waves.Num())
  {
    //Set the new current wave.
    CurrentWave = WavesData->Waves[CurrentWaveCounter];
    //Clear the maps of the previous wave.
    MaxEnemiesToSpawn.Empty();
    CurrentEnemiesSpawned.Empty();
    EnemiesSpawnedEachTime.Empty();
    ClassesToSpawn.Empty();


    //Fill the array of enemies classes to spawn.
    for (auto& Class : CurrentWave.EnemiesClasses)
    {
      //Checks if the classes implements the IIsPoolable Interface
      if (!Class.Key->ImplementsInterface(UIsPoolable::StaticClass()))
      {
        UE_LOG(LogTemp, Warning, TEXT("EnemySpawner.cpp: CUIDADO! CLASE SIN INTERFACE IS POOLABLE"));
      }
      else
      {
        ClassesToSpawn.Add(Class.Key);
      }

    }

    //Obtain the num of enemies we have to spawn of each class in the current wave.
    NumEnemiesToSpawnInCurrentWave(MaxEnemiesToSpawn);

    //Sets the current enemies spawned for each class to 0.
    for (auto& Class : ClassesToSpawn)
    {
      //Establecemos en 0 el número de enemigos spawneado de cada clase.
      CurrentEnemiesSpawned.Add(Class, 0);
      EnemiesSpawnedEachTime.Add(Class, 0);
    }

    //Sets the timer
    GetWorld()->GetTimerManager().SetTimer(TimerHandle,
      this,
      &AEnemySpawner::IterateWave,
      CurrentWave.TimeToSpawn,
      true,
      CurrentWave.TimeToStart);

    UE_LOG(LogTemp, Warning, TEXT("EnemySpawner.cpp: Empezamos ronda"));
    ++CurrentWaveCounter;
  }
  else
  {
    if (NumWavesToRepeatAtEnd == 0)
    {
      //Repeat the last waves.
      UE_LOG(LogTemp, Warning, TEXT("EnemySpanwer.cpp: Finalizamos el spawner"));
      GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
      //Destroy();
    }
    else
    {

      //Repeat the last waves.
      UE_LOG(LogTemp, Warning, TEXT("EnemySpanwer.cpp: Empezamos con las tres últimas"));
      //GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
      //Destroy();
      CurrentWaveCounter -= NumWavesToRepeatAtEnd;
      StartNewWave();
    }

  }

}



void AEnemySpawner::IterateWave()
{

  if (bIsActive)
  {
    //Takes the max num of enemies to spawn at once.
    for (auto& Class : ClassesToSpawn)
    {
      if (EnemiesSpawnedEachTime.Contains(Class))
      {
        EnemiesSpawnedEachTime[Class] = CurrentWave.MaxNumSpawnedEachTime[Class];
      }
      else
      {
        EnemiesSpawnedEachTime.Add(Class, CurrentWave.MaxNumSpawnedEachTime[Class]);
      }

    }

    //Cheacks if can spawn a how many.
    if (CanSpawnEnemy(EnemiesSpawnedEachTime))
    {

      for (auto& Class : ClassesToSpawn)
      {
        for (int n = 0; n < EnemiesSpawnedEachTime[Class]; ++n)
        {
          SpawnEnemy(Class);
        }
      }

    }
    else //If we can't spawn, then check if the Wave have finished.
    {
      bool bEnded = true;
      for (auto& Class : ClassesToSpawn)
      {
        if (CurrentEnemiesSpawned[Class] < MaxEnemiesToSpawn[Class])
        {
          bEnded = false;
        }
      }
      if (bEnded)
      {
        //Starts the next wave.
        //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("EnemySpawner.cpp: Oleada finalizada"));
        GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
        StartNewWave();

      }
    }
  }


}


void AEnemySpawner::SpawnEnemy(TSubclassOf<AActor> ClassToSpawn)
{


  //Sets the bounds of the spawn area.
  FBoxSphereBounds boxBounds = SpawnArea->CalcBounds(SpawnArea->GetComponentTransform());
  FTransform transform = FTransform();
  transform.SetLocation(FMath::RandPointInBox(boxBounds.GetBox()));

  //Takes the next available actor in the pool
  AActor* NextActor = MyPool->GetNextActor(ClassToSpawn);

  if (IsValid(NextActor))
  {
    NextActor->SetActorTransform(transform);

    if (NextActor->GetClass()->IsChildOf(EvolvedEnemy))
    {
      AZigZagPawn* mySphere = GetWorld()->SpawnActor<AZigZagPawn>(AZigZagPawn::StaticClass(), transform);

      if (mySphere != nullptr)
      {
        USphereComponent* SphereToFollow = mySphere->LateralSphere;
        ABasicEnemy* EnemyEvolved = Cast<ABasicEnemy>(NextActor);
        EnemyEvolved->AttachZigZagPawn(mySphere);
        EnemyEvolved->AttachSphere(SphereToFollow);

      }



    }

    //Reactivate the enemy
    if (NextActor->Implements<UIsPoolable>())
    {
      Cast<IIsPoolable>(NextActor)->Reactivate();
    }


    if (NextActor->Implements<UEnemy>())
    {
      IEnemy* NextActorCasted = Cast<IEnemy>(NextActor);

      //Set the HP variation for this wave.
      if (CurrentWave.HPPercentageVariation.Contains(ClassToSpawn))
      {
        NextActorCasted->SetMaxHealth(CurrentWave.HPPercentageVariation[ClassToSpawn]);
      }

      //Set the Damage variation for this wave.
      if (CurrentWave.StrengthPercentageVariation.Contains(ClassToSpawn))
      {
        NextActorCasted->SetMaxDamage(CurrentWave.StrengthPercentageVariation[ClassToSpawn]);
      }

      //Set the Speed variation for this wave.
      if (CurrentWave.SpeedPercentageVariation.Contains(ClassToSpawn))
      {
        NextActorCasted->SetMovementSpeed(CurrentWave.SpeedPercentageVariation[ClassToSpawn]);
      }

      //Set the Plates variation for this wave.
      if (CurrentWave.PlatesVariation.Contains(ClassToSpawn))
      {
        NextActorCasted->SetPlatesVariation(CurrentWave.PlatesVariation[ClassToSpawn]);
      }

      //Set the Upgrades variation for this wave.
      if (CurrentWave.UpgradesVariation.Contains(ClassToSpawn))
      {
          NextActorCasted->SetUpgradesVariation(CurrentWave.UpgradesVariation[ClassToSpawn]);
      }
    }




    ++CurrentEnemiesSpawned[ClassToSpawn];
    //UE_LOG(LogTemp, Warning, TEXT("EnemySpawner.cpp: Enemigo Spawneado!"));
  }
  else
  {
    UE_LOG(LogTemp, Warning, TEXT("EnemySpawner.cpp: No se pudo spawnear un enemigo! Probablemente no quedan enemigos en la Pool."));
  }


}


bool AEnemySpawner::CanSpawnEnemy(TMap<TSubclassOf<AActor>, int32>& enemiesTryToSpawn)
{


  bool bCanSpawn = false;
  for (auto& Class : ClassesToSpawn)
  {
    
     //Obtain the max enemies in screen of the current class of enemy
    int32 NumMaxEnemiesInScreen = -1;
    if (MaxEnemiesInScreen.Contains(Class))
    {
        NumMaxEnemiesInScreen = MaxEnemiesInScreen[Class];
    }

    int32 NumToReachMaxInScreen = 0;
    if (CurrentEnemiesSpawned[Class] < MaxEnemiesToSpawn[Class] && !MyPool->CheckMaxActorsInScreenReached(Class, NumToReachMaxInScreen, NumMaxEnemiesInScreen) && NumToReachMaxInScreen>0)
    {
      //Obtain the random num of enemies to spawn between 1 and MaxEnemiesToSpawnEachTime of the class if bSpawnRandomNumOfEnemiesEachTime is true.
      if (CurrentWave.bSpawnRandomNumOfEnemiesEachTime)
      {
        enemiesTryToSpawn[Class] = FMath::RandRange(1, FMath::Min(EnemiesSpawnedEachTime[Class], NumToReachMaxInScreen));
      }

      //Check if we excedees the MaxEnemiesToSpawn in the current wave
      TArray<int32> min = { enemiesTryToSpawn[Class], MaxEnemiesToSpawn[Class] - CurrentEnemiesSpawned[Class] };
      enemiesTryToSpawn[Class] = FMath::Max(0, FMath::Min<int32>(min));
      bCanSpawn = true;
    }
    else
    {
      enemiesTryToSpawn[Class] = 0;
    }

  }
  return bCanSpawn;
}



void AEnemySpawner::GetTotalSpawns(TMap<TSubclassOf<AActor>, int32>& ArraySpawns)
{

  TMap<TSubclassOf<AActor>, int32> NumEnemies;

  //Obtains the total enemies of each class to spawn in each wave from the percentage out of the total enemies to spawn in each wave.
  for (FWaveDataStruct& Wave : WavesData->Waves)
  {
    int TotalEnemiesInWave = 0;

    for (auto& elem : Wave.EnemiesClasses)
    {
      int32* value = NumEnemies.Find(elem.Key);
      if (value == nullptr)
      {
        NumEnemies.Add(elem.Key, elem.Value * Wave.TotalEnemies);
      }
      else
      {
        *value += elem.Value * Wave.TotalEnemies;
      }

      TotalEnemiesInWave += elem.Value * Wave.TotalEnemies;
    }

    //Check if we cand add another enemy. This is because int and float are involved at the same time.
    if (Wave.TotalEnemies - TotalEnemiesInWave > 0 && Wave.EnemiesClasses.Num() > 0)
    {

      NumEnemies[Wave.EnemiesClasses.begin().Key()] += Wave.TotalEnemies - TotalEnemiesInWave;
    }

  }

  //Check if it is neccesary to add more enemies of each class to the pool.
  //Only adds more enemies if this spawner needs more enemies than the stores in the ArraySpawns
  for (auto& ClassToSpawn : NumEnemies)
  {
    if (ArraySpawns.Contains(ClassToSpawn.Key))
    {
      if (ArraySpawns[ClassToSpawn.Key] < ClassToSpawn.Value)
      {
        ArraySpawns[ClassToSpawn.Key] = ClassToSpawn.Value;
      }
    }
    else
    {
      ArraySpawns.Add(ClassToSpawn.Key, ClassToSpawn.Value);
    }
  }

}


//Obtains the total enemies of each class to spawn in the current wavew from the percentage out of the total enemies to spawn in the current wave
void AEnemySpawner::NumEnemiesToSpawnInCurrentWave(TMap<TSubclassOf<AActor>, int32>& EnemiesToSpawn)
{

  int numEnemies = 0;
  for (const auto& Class : CurrentWave.EnemiesClasses)
  {
    numEnemies += Class.Value * CurrentWave.TotalEnemies;
    EnemiesToSpawn.Add(Class.Key, Class.Value * CurrentWave.TotalEnemies);
  }

  //Si por el truncamiento se nos ha quedado algún hueco, lo añadimos a la primera clase.
  if (CurrentWave.TotalEnemies - numEnemies > 0 && CurrentWave.EnemiesClasses.Num() > 0)
  {
    CurrentWave.EnemiesClasses.begin()->Value += CurrentWave.TotalEnemies - numEnemies;
  }

}

void AEnemySpawner::StopAudio()
{
  if (AmbientHordeAudioInstance.Instance)
  {
    AmbientHordeAudioInstance.Instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
    AmbientHordeAudioInstance.Instance->release();
    AmbientHordeAudioInstance.Instance = nullptr;
  }
}

void AEnemySpawner::ActivateSpawner(const TMap<TSubclassOf<AActor>, int32>& MaxEnemiesOfEachClassInScreen, AConquestZone* ConquestZoneOwner)
{
  bIsActive = true;

  if (MaxEnemiesOfEachClassInScreen.Num() > 0)
  {
      MaxEnemiesInScreen = MaxEnemiesOfEachClassInScreen;
  }

  if (ConquestZoneOwner && ConquestZoneOwner->bLimitEnemiesNumberWhenTurretsAreDeath)
  {
      ConquestZoneOwner->MaxEnemiesInConquestZoneChanged.AddUObject(this, &AEnemySpawner::UpdateMaxEnemiesInScreen);
  }

  if (!AmbientHordeAudioInstance.Instance)
  {
    AmbientHordeAudioInstance = UFMODBlueprintStatics::PlayEvent2D(GetWorld(), AmbientHordeAudioEvent, true);
    AmbientHordeAudioInstance.Instance->setParameterByName("HordeAmbient", 0.23f);
    AmbientHordeAudioInstance.Instance->setParameterByName("Direction", 0.30f);
  }
}


void AEnemySpawner::DeactivateSpawner()
{
  bIsActive = false;
  StopAudio();
}

void AEnemySpawner::BeginDestroy()
{
  Super::BeginDestroy();
  StopAudio();
}


void AEnemySpawner::UpdateMaxEnemiesInScreen(const TMap<TSubclassOf<AActor>, int32>& MaxEnemiesOfEachClassInScreen)
{
    MaxEnemiesInScreen = MaxEnemiesOfEachClassInScreen;
}