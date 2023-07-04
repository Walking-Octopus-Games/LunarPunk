// Fill out your copyright notice in the Description page of Project Settings.

#include "Pool.h"

#include "Granade.h"
#include "Kismet/GameplayStatics.h"
#include "EnemySpawner/HowManySpawns.h"
#include "Managers/EntityManager.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"


// Sets default values
APool::APool()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

}

void APool::InitializePool()
{
  //Sets the pool location
  PoolLocation = GetActorLocation();


  //Find the actors with the interface IHowManySpawns. These are the actors which requires the pool.
  TArray<AActor*> ActorsThatSpawnsArray;
  UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UHowManySpawns::StaticClass(), ActorsThatSpawnsArray);

  for (AActor* actor : ActorsThatSpawnsArray)
  {
    IHowManySpawns* ActorThatSpawns = Cast<IHowManySpawns>(actor);
    if (ActorThatSpawns != nullptr)
    {
      //Fill the array with the classes to Spawn and the number of actors required for each class.
      ActorThatSpawns->GetTotalSpawns(ClassesToSpawn);

    }
  }

  ALunarPunkGameMode* GameMode = Cast<ALunarPunkGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

  if (GameMode && GameMode->EntityManager)
  {

      //Check if all of the classes stored implements the interface IIsPoolable. If not, then removed it from the array.
      for (auto& Class : ClassesToSpawn)
      {
          //Si alguna de las clases añadidas no implementa la interfaz, la borramos del array y lanzamos un aviso.
          if (!Class.Key->ImplementsInterface(UIsPoolable::StaticClass()))
          {
              ClassesToSpawn.Remove(Class.Key);
              UE_LOG(LogTemp, Warning, TEXT("Pool.cpp: Se eliminó un tipo de clase que no es pooleable"));

          }
          //If the value of any class to spawn is greater than the max in the screen, then redefine the number.
          if (GameMode->EntityManager->MaxActorsInScrren.Contains(Class.Key) && Class.Value > GameMode->EntityManager->MaxActorsInScrren[Class.Key])
          {
              Class.Value = GameMode->EntityManager->MaxActorsInScrren[Class.Key];
          }
      }

  }
  //Set the Spawn infor to spawn all the actors in the pool.
  UWorld* world = GetWorld();
  FActorSpawnParameters SpawnInfo;
  SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
  //SpawnInfo.Owner = nullptr;
  //SpawnInfo.Instigator = GetInstigator();
  //SpawnInfo.ObjectFlags |= RF_Transient;

  //Do all the spawns for each class
  for (auto& Class : ClassesToSpawn)
  {
    for (int i = 0; i < Class.Value; ++i)
    {
      //Set the pool index and add the actor to the Spawns Multimap with his class Key.
      AActor* SpawnedActor = world->SpawnActor<AActor>(Class.Key, PoolLocation, FRotator().ZeroRotator, SpawnInfo);
      IIsPoolable* SpawnedActorCasted = Cast<IIsPoolable>(SpawnedActor);
      //Comprobamos que la case sea también un IIsPoolable

      if (SpawnedActor != nullptr && SpawnedActorCasted != nullptr)
      {
        SpawnedActorCasted->Index = i;
        SpawnedActorCasted->Deactivate();
        Spawns.Add(Class.Key, SpawnedActor);
      }
      else
      {
        UE_LOG(LogTemp, Warning, TEXT("Pool.cpp: No se ha podido Spawnear el actor."));
      }
    }

  }

  PoolCreated.Broadcast();
}

// Called when the game starts or when spawned
void APool::BeginPlay()
{
  Super::BeginPlay();
  //SuscribeToReactivateDelegate();
}



// Called every frame
void APool::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

}


AActor* APool::GetNextActor(TSubclassOf<AActor> ClassToSpawn)
{

  //Array that contains all the actors of the class ClassToSpawn
  TArray<AActor*> Actors;
  Spawns.MultiFind(ClassToSpawn, Actors, true);

  //Looks for the first actor available in the array and returns it.
  for (AActor* SelectedActor : Actors)
  {

    if (IsValid(SelectedActor) && SelectedActor->Implements<UIsPoolable>())
    {
      IIsPoolable* SelectedActorCasted = Cast<IIsPoolable>(SelectedActor);
      if (!SelectedActorCasted->bIsActive)
      {
        //Adds the index and set the actor to Active state. Note: Probably the index is not neccesary.
        /*IndexesSpawnedActors.Add(ClassToSpawn, SelectedActorCasted->Index);*/
        SelectedActorCasted->bIsActive = true;
        return SelectedActor;
      }
    }

  }
  //If there's not available actor, then returns nullptr and throws a message.
  //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("No quedan enemigos en la pool"));
  return nullptr;

  //Nota: había que gestionarlo de alguna otra manera.

}

TArray<AActor*> APool::GetAllActors(TSubclassOf<AActor> ClassToSpawn)
{
  TArray<AActor*> Actors;
  Spawns.MultiFind(ClassToSpawn, Actors, true);
  return Actors;
}


int32 APool::NumActiveActorsOfClass(TSubclassOf<AActor> Class)
{
    TArray<AActor*> Actors;
    Spawns.MultiFind(Class, Actors, true);
    int32 NumActiveActors = 0;

    for (AActor* Actor : Actors)
    {
        if (IIsPoolable* ActorCasted = Cast<IIsPoolable>(Actor))
        {
            if (ActorCasted->bIsActive)
            {
                ++NumActiveActors;
            }
        }
    }

    return NumActiveActors;
}


bool APool::CheckMaxActorsInScreenReached(TSubclassOf<AActor> Class, int32 OverrideMax)
{
    ALunarPunkGameMode* GameMode = Cast<ALunarPunkGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

    if (GameMode && GameMode->EntityManager)
    {
        //TMap<TSubclassOf<AActor>, int32> MaxActorsInScrren = GameMode->EntityManager->MaxActorsInScrren;
        // 
        //Check if the override max is valid. Then try to override the max enemies value only if this value is lower than the
        //max enemies global value.
        if (OverrideMax == -1)
        {
            if (GameMode->EntityManager->MaxActorsInScrren.Contains(Class))
            {
                OverrideMax = GameMode->EntityManager->MaxActorsInScrren[Class];
            }
            else
            {
                return false;
            }
        }
        else
        {
            if (GameMode->EntityManager->MaxActorsInScrren.Contains(Class) && OverrideMax > GameMode->EntityManager->MaxActorsInScrren[Class])
            {
                OverrideMax = GameMode->EntityManager->MaxActorsInScrren[Class];
            }
          
        }

        if (NumActiveActorsOfClass(Class) < OverrideMax)
        {
            return false;
        }
        else 
        {
            return true;
        }

        
    }

    return false;


}