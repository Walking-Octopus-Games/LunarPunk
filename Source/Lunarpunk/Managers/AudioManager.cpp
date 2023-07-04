// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/AudioManager.h"
#include "EntityManager.h"
#include "FMODBlueprintStatics.h"
#include "EnemySpawner/EnemySpawner.h"
#include "HealthSystem/HealthComponent.h"
#include "Player/AudioPlayerComponent.h"
#include "Player/LunarPunkPlayerController.h"
#include "Portal/Portal.h"
#include "Turrets/AudioTurretComponent.h"
#include "Turrets/Turret.h"


// Sets default values
AAudioManager::AAudioManager()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAudioManager::BeginPlay()
{
  Super::BeginPlay();
}

// Called every frame
void AAudioManager::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
  /* UE_LOG(LogTemp, Warning, TEXT("Actual Layer: %d"), ActualLayer);*/
}

void AAudioManager::Initialize(ALunarPunkGameMode* LunarPunkGameMode)
{
  GameMode = LunarPunkGameMode;
}

void AAudioManager::StopMusic()
{
  if (ActualMusicInstance.Instance != nullptr)
  {
    ActualMusicInstance.Instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);

  }
  if (ActualAmbientInstance.Instance != nullptr)
  {
    ActualAmbientInstance.Instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
  }
  if (IsValid(GameMode) && IsValid(GameMode->EntityManager))
  {
    for (AEnemySpawner* EnemySpawner : GameMode->EntityManager->EnemySpawners)
    {
      if (EnemySpawner->AmbientHordeAudioInstance.Instance)
      {
        EnemySpawner->AmbientHordeAudioInstance.Instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
      }
    }
  }

  if (IsValid(GameMode) && IsValid(GameMode->EntityManager))
  {
    for (APortal* Portal : GameMode->EntityManager->Portals)
    {
      if (Portal->PortalAudioEventInstance)
      {
        Portal->PortalAudioEventInstance->Stop();
        Portal->PortalAudioEventInstance->Release();
      }
    }
  }

  if (IsValid(GameMode) && IsValid(GameMode->EntityManager) && IsValid(GameMode->EntityManager->PlayerCharacter))
  {
    GameMode->EntityManager->PlayerCharacter->PlayerAudioComponent->StopAudio();
  }

  if (IsValid(GameMode) && IsValid(GameMode->EntityManager))
  {

    for (ATurret* Turret : GameMode->EntityManager->Turrets)
    {
      Turret->AudioTurretComponent->StopAudio();
    }
  }
}

void AAudioManager::PauseMusic()
{
  if (ActualMusicInstance.Instance != nullptr)
  {
    ActualMusicInstance.Instance->setPaused(true);
  }

  if (ActualAmbientInstance.Instance != nullptr)
  {
    ActualAmbientInstance.Instance->setPaused(true);
  }
  if (IsValid(GameMode) && IsValid(GameMode->EntityManager))
  {
    for (AEnemySpawner* EnemySpawner : GameMode->EntityManager->EnemySpawners)
    {
      if (IsValid(EnemySpawner))
      {

        if (EnemySpawner->AmbientHordeAudioInstance.Instance)
        {
          EnemySpawner->AmbientHordeAudioInstance.Instance->setPaused(true);
        }

      }
    }
  }

  if (IsValid(GameMode) && IsValid(GameMode->EntityManager))
  {
    for (APortal* Portal : GameMode->EntityManager->Portals)
    {
      if (Portal->PortalAudioEventInstance)
      {
        Portal->PortalAudioEventInstance->SetPaused(true);
      }
    }
  }

  if (IsValid(GameMode) && IsValid(GameMode->EntityManager) && IsValid(GameMode->EntityManager->PlayerCharacter))
  {
    GameMode->EntityManager->PlayerCharacter->PlayerAudioComponent->PauseAudio();
  }

  if (IsValid(GameMode) && IsValid(GameMode->EntityManager))
  {

    for (ATurret* Turret : GameMode->EntityManager->Turrets)
    {
      Turret->AudioTurretComponent->PauseAudio();
    }
  }
}

void AAudioManager::RestartMusic()
{
  if (ActualMusicInstance.Instance != nullptr)
  {
    ActualMusicInstance.Instance->setPaused(false);
  }

  if (ActualAmbientInstance.Instance != nullptr)
  {
    ActualAmbientInstance.Instance->setPaused(false);
  }
  if (IsValid(GameMode) && IsValid(GameMode->EntityManager))
  {
    for (AEnemySpawner* EnemySpawner : GameMode->EntityManager->EnemySpawners)
    {
      if (EnemySpawner->AmbientHordeAudioInstance.Instance)
      {
        EnemySpawner->AmbientHordeAudioInstance.Instance->setPaused(false);
      }
    }
  }

  if (IsValid(GameMode) && IsValid(GameMode->EntityManager))
  {
    for (APortal* Portal : GameMode->EntityManager->Portals)
    {
      if (Portal->PortalAudioEventInstance)
      {
        Portal->PortalAudioEventInstance->SetPaused(false);
      }
    }
  }

  if (IsValid(GameMode) && IsValid(GameMode->EntityManager) && IsValid(GameMode->EntityManager->PlayerCharacter))
  {
    GameMode->EntityManager->PlayerCharacter->PlayerAudioComponent->RestartAudio();
  }

  if (IsValid(GameMode) && IsValid(GameMode->EntityManager))
  {
    for (ATurret* Turret : GameMode->EntityManager->Turrets)
    {
      Turret->AudioTurretComponent->RestartAudio();
    }
  }
}

void AAudioManager::StartLevel1Music()
{
  GameMode->EntityManager->PlayerCharacter->HealthComponent->ActorDeath.AddDynamic(this, &AAudioManager::StopMusic);

  ActualMusicInstance = UFMODBlueprintStatics::PlayEvent2D(GetWorld(), Level1AmbientMusic, true);
  ActualAmbientInstance = UFMODBlueprintStatics::PlayEvent2D(GetWorld(), Level1Ambient, true);
  ActualLayer = 0;
  UFMODBlueprintStatics::EventInstanceSetParameter(ActualMusicInstance, FName("Layers"), ActualLayer);
}

void AAudioManager::StartLevel2Music()
{
  GameMode->EntityManager->PlayerCharacter->HealthComponent->ActorDeath.AddDynamic(this, &AAudioManager::StopMusic);

  ActualMusicInstance = UFMODBlueprintStatics::PlayEvent2D(GetWorld(), Level2AmbientMusic, true);
  ActualAmbientInstance = UFMODBlueprintStatics::PlayEvent2D(GetWorld(), Level2Ambient, true);
  ActualLayer = 0;
  UFMODBlueprintStatics::EventInstanceSetParameter(ActualMusicInstance, FName("Layers"), ActualLayer);
}

void AAudioManager::AddLayer()
{
  ++ActualLayer;
  UFMODBlueprintStatics::EventInstanceSetParameter(ActualMusicInstance, FName("Layers"), ActualLayer);
}

void AAudioManager::MinusLayer()
{
  --ActualLayer;
  UFMODBlueprintStatics::EventInstanceSetParameter(ActualMusicInstance, FName("Layers"), ActualLayer);
}

void AAudioManager::BeginDestroy()
{
  StopMusic();
  Super::BeginDestroy();
}



