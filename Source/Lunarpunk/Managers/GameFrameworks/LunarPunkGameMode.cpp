// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "LunarPunkGameState.h"
#include "Managers/EntityManager.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/UpgradesManager.h"
#include "RewardsAndUpgrades/UpgradesData.h"
#include "Managers/PlatesManager.h"
#include "Managers/AbilitiesManger.h"
#include "Managers/AbilitiesData.h"
#include "Managers/AudioManager.h"
#include "HealthSystem/HealthComponent.h"
#include "Player/PlayableCharacter.h"
#include "Managers/HUDManager.h"
#include "Player/CameraManager.h"
#include "Player/LunarPunkPlayerController.h"
#include "Turrets/Turret.h"
#include "Managers/StatsManager.h"


ALunarPunkGameMode::ALunarPunkGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
  PrimaryActorTick.bCanEverTick = true;

  //#if UE_BUILD_SHIPPING
  //  bDeactivateVFX = false;
  //  bDeactivateAI = false;
  //#endif
  static ConstructorHelpers::FObjectFinder<UUpgradesData> MyUpgradesDataAsset(TEXT("UpgradesData'/Game/Blueprints/Rewards/UpgradesData.UpgradesData'"));

  if (MyUpgradesDataAsset.Succeeded())
  {
    UpgradesDataAsset = MyUpgradesDataAsset.Object;
  }
}

void ALunarPunkGameMode::BeginPlay()
{
  Super::BeginPlay();

}

void ALunarPunkGameMode::Initialize()
{
  StartGameModeInitializeEvent.Broadcast();

  APlayableCharacter* PlayableCharacter = Cast <APlayableCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

  if (PlayableCharacter)
  {
    ALunarPunkPlayerController* PlayerController = Cast<ALunarPunkPlayerController>(PlayableCharacter->GetController());

    if (PlayerController)
    {
      //Initialize the main managers of the game. They have to be initialized in the correct order. Beaware to modify this order.
      InitAudioManager();
      InitHUDManager(PlayerController, PlayableCharacter);
      InitEntityManager(PlayableCharacter, PlayerController);
      HUDManager->PostInitialize(PlayerController, this); //Things to do in the HUD manager after Entity Manager Initialization
      CameraManager = Cast<ACameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));
      LPGameState = GetGameState<ALunarPunkGameState>();
      LPGameState->Initialize(this);
      InitUpgradesManager(PlayerController);
      InitPlatesManager(PlayerController, EntityManager);
      InitAbilitiesManager(PlayerController);
      InitStatsManager(PlayerController);
      EndGameModeInitializeEvent.Broadcast();
    }

  }
}

void ALunarPunkGameMode::InitAudioManager()
{
  AudioManager = GetWorld()->SpawnActor<AAudioManager>();
  AudioManager->Initialize(this);
}

void ALunarPunkGameMode::InitEntityManager(APlayableCharacter* PlayableCharacter, ALunarPunkPlayerController* PlayerController)
{
  if (!EntityManager)
  {
    //EntityManager = NewObject<UEntityManager>();
    EntityManager= NewObject<UEntityManager>(this, EntityManagerClass);
  }

  EntityManager->Initialize(GetWorld(), PlayableCharacter, PlayerController);
}

void ALunarPunkGameMode::InitUpgradesManager(ALunarPunkPlayerController* PlayerController)
{
  UpgradesManager = NewObject<UUpgradesManager>();

  if (IsValid(UpgradesManager) && UpgradesDataAsset != nullptr)
  {
    // Use the data asset here
    UpgradesManager->Initialize(this, UpgradesDataAsset, PlayerController);
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT(" UPGRADES DATA ASSET NOT FOUND"));
  }

}

void ALunarPunkGameMode::InitPlatesManager(ALunarPunkPlayerController* PlayerController, UEntityManager* EntityManagerReference)
{
  PlatesManager = Cast<APlatesManager>(GetWorld()->SpawnActor(PlatesManagerClass));
  PlatesManager->Initialize(PlayerController, EntityManagerReference);
}

void ALunarPunkGameMode::InitAbilitiesManager(ALunarPunkPlayerController* PlayerController)
{

  AbilitiesManager = NewObject<UAbilitiesManger>();

  if (IsValid(AbilitiesManager))
  {
    AbilitiesManager->Initialize( PlayerController);
  }
}

void ALunarPunkGameMode::InitHUDManager(ALunarPunkPlayerController* PlayerController, APlayableCharacter* PlayableCharacter)
{
  HUDManager = PlayerController->GetHUD<AHUDManager>();
  if (HUDManager)
  {
    HUDManager->Initialize(PlayableCharacter);
  }
}

void ALunarPunkGameMode::PostLogin(APlayerController* NewPlayer)
{
  Super::PostLogin(NewPlayer);
}

void ALunarPunkGameMode::StartPlay()
{
  Super::StartPlay();
}

void ALunarPunkGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
  Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}


void ALunarPunkGameMode::GodMode()
{
  bIsGodModeActivated = !bIsGodModeActivated;
  if (HUDManager)
  {
    HUDManager->ShowGodModeAnimation(bIsGodModeActivated);
  }
  if (EntityManager)
  {
    EntityManager->PlayerCharacter->GetHealthComponent()->SetGodModeEnabled(bIsGodModeActivated);
  }
}


void ALunarPunkGameMode::GodModeTurrets()
{
    bIsGodModeTurretsActivated = !bIsGodModeTurretsActivated;
    if (HUDManager)
    {
        HUDManager->ShowGodModeTurretsAnimation(bIsGodModeTurretsActivated);
    }
    if (EntityManager)
    {
        for (ATurret* Turret : EntityManager->Turrets)
        {
            Turret->FindComponentByClass<UHealthComponent>()->SetGodModeEnabled(bIsGodModeTurretsActivated);
        }
    }
}

void ALunarPunkGameMode::InitStatsManager(ALunarPunkPlayerController* PlayerController)
{
  StatsManager = NewObject<UStatsManager>();
  if (StatsManager)
  {
    StatsManager->Initialize(this, PlayerController);
  }
}
