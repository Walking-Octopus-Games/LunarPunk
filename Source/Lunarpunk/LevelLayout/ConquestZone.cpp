// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelLayout/ConquestZone.h"
#include "Kismet/GameplayStatics.h"
#include "TurretConquestPlace.h"
#include "ZoneTrigger.h"
#include "Components/BoxComponent.h"
#include "Player/PlayableCharacter.h"
#include "Portal/Portal.h"
#include "EnemySpawner/EnemySpawner.h"
#include "HealthSystem/HealthComponent.h"
#include "Enemies/IEnemy.h"
#include "Player/Abilities/DestroyPortalAbilityComponent.h"
#include "Player/Abilities/ShootAbilityComponent.h"
#include "Turrets/Turret.h"


// Sets default values
AConquestZone::AConquestZone()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void AConquestZone::BeginPlay()
{
  Super::BeginPlay();

  UWorld* World = GetWorld();

  if (IsValid(World))
  {



    //Get all the turrets conquest places and store only the asociated ones.
    TArray<AActor*> AllTurretsConquestPlaces;
    UGameplayStatics::GetAllActorsOfClass(World, ATurretConquestPlace::StaticClass(), AllTurretsConquestPlaces);
    ATurretConquestPlace* TurretPlaceCasted;

    for (AActor* TurretPlace : AllTurretsConquestPlaces)
    {
      TurretPlaceCasted = Cast<ATurretConquestPlace>(TurretPlace);
      if (IsValid(TurretPlaceCasted) && TurretPlaceCasted->IndexOwnerConquestZone == Index)
      {
        TurretsConquestPlaces.Add(TurretPlaceCasted);
        TurretPlaceCasted->TurretPlaced.AddDynamic(this, &AConquestZone::OnTurretPlaced);
        TurretPlaceCasted->TurretRetired.AddDynamic(this, &AConquestZone::OnTurretRetired);
      }
    }

    NumTurretsConquestPlacesAsociated = TurretsConquestPlaces.Num();


    //Get all the Zone triggers asociated:
     //Get all the turrets conquest places and store only the asociated ones.
    TArray<AActor*> AllZoneTriggers;
    UGameplayStatics::GetAllActorsOfClass(World, AZoneTrigger::StaticClass(), AllZoneTriggers);
    AZoneTrigger* ZoneTriggerCasted;

    for (AActor* ZoneTrigger : AllZoneTriggers)
    {
      ZoneTriggerCasted = Cast<AZoneTrigger>(ZoneTrigger);
      if (IsValid(ZoneTriggerCasted) && ZoneTriggerCasted->IndexOwnerConquestZone == Index)
      {
        ZoneTriggersAsociated.Add(ZoneTriggerCasted);
        ZoneTriggerCasted->TriggerActivated.AddDynamic(this, &AConquestZone::ManageZoneTriggerOverlapped);
      }
    }

    //Find the portal asociated.
    TArray<AActor*> AllPortals;
    UGameplayStatics::GetAllActorsOfClass(World, APortal::StaticClass(), AllPortals);
    APortal* PortalCasted;
    for (AActor* Portal : AllPortals)
    {
      PortalCasted = Cast<APortal>(Portal);
      if (IsValid(PortalCasted))
      {
          if (PortalCasted->IsMovablePortal && PortalCasted->Index <= Index && Index <= PortalCasted->Index + PortalCasted->TotalHits)
          {
              PortalCasted->PortalDestroy.AddDynamic(this, &AConquestZone::ConquestZoneCompleted);
          } 
          else if (!PortalCasted->IsMovablePortal && PortalCasted->Index == Index)
          {
              PortalCasted->PortalDestroy.AddDynamic(this, &AConquestZone::ConquestZoneCompleted);
          }
      }
    }


    //Find the collision and add the neccesary delegates.
    ConquestArea = FindComponentByClass<UBoxComponent>();
    if (!IsValid(ConquestArea))
    {
      UE_LOG(LogTemp, Warning, TEXT("ConquestZone.cpp: collision not found"));
    }
    else
    {
      ConquestArea->OnComponentBeginOverlap.AddDynamic(this, &AConquestZone::OnBeginOverlap);
      ConquestArea->OnComponentEndOverlap.AddDynamic(this, &AConquestZone::OnEndOverlap);
      DisableConquestZone();

    }

    PlayerReference = Cast<APlayableCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));
    if (!IsValid(PlayerReference))
    {
      UE_LOG(LogTemp, Warning, TEXT("ConquestZone.cpp: Player not found"));
    }


    GetEnemySpawnersAsociated();

    CurrentTimeToEndResting = RestingTime;



    //Manage the enemies number when turrets die
    TArray<AActor*> AllTurretsInLevel;
    UGameplayStatics::GetAllActorsOfClass(World, ATurret::StaticClass(), AllTurretsInLevel);
    ATurret* TurretCasted;

    for (AActor* Turret : AllTurretsInLevel)
    {
        TurretCasted = Cast<ATurret>(Turret);
        if (IsValid(TurretCasted))
        {
           
            TurretCasted->TurretDeath.AddDynamic(this, &AConquestZone::OnTurretDeath);
            TurretCasted->TurretReactivated.AddDynamic(this, &AConquestZone::OnTurretReactivated);
            ++TurretsInLevel;
        }
    }

    InitialMaxEnemiesInScreen = MaxEnemiesInScreen;

  }
}

// Called every frame
void AConquestZone::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);


}


void AConquestZone::EnableConquestZone()
{
  ConquestArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
  ShowZoneActivatedEffect();
  /*if (bPlayerIsInZone)
  {
    ActivatePlayerCanChargeShoot();
  }*/
}

void AConquestZone::DisableConquestZone()
{
  ConquestArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  RemoveZoneActivatedEffect();
  /* if (bPlayerIsInZone)
   {
     DeactivatePlayerCanChargeShoot();
     bPlayerIsInZone = false;
   }*/
}



void AConquestZone::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{


  if (OtherActor == PlayerReference)
  {
    ActivatePlayerCanChargeShoot();
    bPlayerIsInZone = true;
    //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "CARGA EL DISPARO!!!!!!");
  }


}


void AConquestZone::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

  if (OtherActor == PlayerReference)
  {
    DeactivatePlayerCanChargeShoot();
    bPlayerIsInZone = false;
  }

}





void AConquestZone::OnTurretPlaced()
{
  ++CurrentTurretsPlacedInConquestPlaces;
  if (CurrentTurretsPlacedInConquestPlaces == NumTurretsConquestPlacesAsociated)
  {
    EnableConquestZone();
  }
}

void AConquestZone::OnTurretRetired()
{
  --CurrentTurretsPlacedInConquestPlaces;
  if (CurrentTurretsPlacedInConquestPlaces == NumTurretsConquestPlacesAsociated - 1)
  {
    DisableConquestZone();
  }
}


void AConquestZone::ActivatePlayerCanChargeShoot()
{
  //bPlayerCanChargeShoot = true;
  if (IsValid(PlayerReference) && PlayerReference->ActualPlayerStatus != EPlayerState::DestroyingPortal && PlayerReference->ActualPlayerStatus != EPlayerState::PositioningTurret)
  {
    PlayerReference->ActualPlayerStatus = EPlayerState::DestroyingPortal;
    //PlayerReference->SetDestroyPortalMode();
    PlayerReference->DestroyPortalAbilityComponent->IndexPortalToDestroy = Index;
    PlayerReference->ShootAbilityComponent->DisableAbility();
    ShowPlayerCanShoot.Broadcast();
  }
}

void AConquestZone::DeactivatePlayerCanChargeShoot()
{
  //bPlayerCanChargeShoot = false;
  if (IsValid(PlayerReference) && PlayerReference->ActualPlayerStatus == EPlayerState::DestroyingPortal)
  {
    PlayerReference->ActualPlayerStatus = EPlayerState::Default;
    PlayerReference->DestroyPortalAbilityComponent->ResetDestroyPortal();
    PlayerReference->ShootAbilityComponent->EnableAbility();

    //PlayerReference->UnsetDestroyPortalMode();
    HidePlayerCanShoot.Broadcast();
  }
}


void  AConquestZone::UpdateTimeToNextWave()
{
  --CurrentTimeToEndResting;
  RefreshHUDTimeToNextWave();

  if (!bShowingRestingTime && CurrentTimeToEndResting <= TimeToShowRestingTime)
  {
      ShowTimeToNextWaveInHUD();
  }

  if (CurrentTimeToEndResting == 0)
  {
    HideTimeToNextWaveInHUD();
    ActivateEnemySpawnersAsociated();

    /*for (AZoneTrigger* ZoneTrigger : ZoneTriggersAsociated)
    {
      ZoneTrigger->DeactivateZoneTrigger();
    }*/

    // If the time ends the game choses a random path for her to go
    if (ZoneTriggersAsociated.Num() > 1)
    {
      int32 IndexOfZoneTriggerToActivate = FMath::RandRange(0, ZoneTriggersAsociated.Num() - 1);
      ZoneTriggersAsociated[IndexOfZoneTriggerToActivate]->DeactivateZoneTrigger();
    }
    else
    {
      if (ZoneTriggersAsociated.Num() > 0)
      {
        ZoneTriggersAsociated[0]->DeactivateZoneTrigger();
      }
    }

    GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
  }

}


void AConquestZone::OpenConquestZone()
{


  //ShowTimeToNextWaveInHUD();

  GetWorld()->GetTimerManager().SetTimer(TimerHandle,
    this,
    &AConquestZone::UpdateTimeToNextWave,
    1.0,
    true,
    0.0
  );


  for (AZoneTrigger* ZoneTrigger : ZoneTriggersAsociated)
  {
    ZoneTrigger->ActivateZoneTrigger();
  }
  //ActivateEnemySpawnersAsociated();

  ZoneStarted.Broadcast(Index);
}


void AConquestZone::PauseConquestZone()
{
    GetWorld()->GetTimerManager().PauseTimer(TimerHandle);
    if (bShowingRestingTime)
    {
        HideTimeToNextWaveInHUD();
    }
    
}


void AConquestZone::ResumeConquestZone()
{
    GetWorld()->GetTimerManager().UnPauseTimer(TimerHandle);
    //ShowTimeToNextWaveInHUD();
}



void AConquestZone::ForceBeginEnemiesSpawners()
{
  HideTimeToNextWaveInHUD();
  ActivateEnemySpawnersAsociated();
  GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void AConquestZone::ConquestZoneCompleted(bool IsLastHit, int IndexPortal)
{
    if (Index == IndexPortal)
    {
        for (ATurretConquestPlace* Place : TurretsConquestPlaces)
        {
            Place->CloseConquestPlace();
        }

        DisableConquestZone();
        ConquestArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        RemoveZoneActivatedEffect();
        DeactivatePlayerCanChargeShoot();
        DeactivateEnemySpawnersAsociated();
        DeactivateAllEnemiesInMap();
        //bPlayerIsInZone = false;


        //Announces that the sector closes and says which is the next sector to be opened.
        ZoneCompleted.Broadcast(Index + 1);
    }
}





void AConquestZone::GetEnemySpawnersAsociated()
{
  TArray<AActor*> AllEnemySpawners;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawner::StaticClass(), AllEnemySpawners);

  AEnemySpawner* EnemySpawnerCasted;
  for (AActor* EnemySpawner : AllEnemySpawners)
  {
    EnemySpawnerCasted = Cast<AEnemySpawner>(EnemySpawner);
    if (IsValid(EnemySpawnerCasted) && EnemySpawnerCasted->OwnerSectorIndex == Index)
    {
      EnemySpawners.Add(EnemySpawnerCasted);
    }
  }

  if (EnemySpawners.Num() == 0)
  {
    UE_LOG(LogTemp, Warning, TEXT("Sector.cpp: No se han encontrado EnemySpanwers asociados a este sector."))
  }
}


void AConquestZone::ActivateEnemySpawnersAsociated()
{
  for (AEnemySpawner* EnemySpawner : EnemySpawners)
  {
    if (IsValid(EnemySpawner))
    {
      if (EnemySpawner->WavesData->Waves.Num() > 0)
      {
        //EnemySpawner->WavesData->Waves[0].TimeToStart = RestingTime;
        EnemySpawner->WavesData->Waves[0].TimeToStart = 0.1;
      }
      EnemySpawner->ActivateSpawner(MaxEnemiesInScreen, this);
    }

  }
}

void AConquestZone::DeactivateEnemySpawnersAsociated()
{
  for (AEnemySpawner* EnemySpawner : EnemySpawners)
  {
    if (IsValid(EnemySpawner))
    {
      EnemySpawner->DeactivateSpawner();
    }

  }
}



float AConquestZone::GetCurrentTimeEndResting()
{
  return CurrentTimeToEndResting;
}



void AConquestZone::DeactivateAllEnemiesInMap() const
{
  TArray<AActor*> EnemiesInMap;
  UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UEnemy::StaticClass(), EnemiesInMap);
  for (AActor* AEnemyActor : EnemiesInMap)
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

void AConquestZone::ManageZoneTriggerOverlapped(bool ShouldDelay, float DelayTime)
{
    if (ShouldDelay)
    {
        FTimerHandle TimerActivateSpawners;
        GetWorld()->GetTimerManager().SetTimer(TimerActivateSpawners, this, &AConquestZone::ForceBeginEnemiesSpawners,DelayTime, false);
    }
    else
    {
        ForceBeginEnemiesSpawners();
    }
}


void AConquestZone::OnTurretDeath()
{
    ++TurretsDeathInLevel;

    if (TurretsDeathInLevel == TurretsInLevel- AliveTurretsToChangeMaxEnemiesInScreen)
    {
        
        for (auto it = MaxEnemiesInScreen.begin(); it != MaxEnemiesInScreen.end(); ++it)
        {
            if (MaxEnemiesInScreen.Contains(it->Key))
            {
                it->Value = MaxEnemiesInScreenWhenTurretsAreDeath[it->Key];
            }
        }
        MaxEnemiesInConquestZoneChanged.Broadcast(MaxEnemiesInScreen);
    }
}


void AConquestZone::OnTurretReactivated()
{
    --TurretsDeathInLevel;

    if (TurretsDeathInLevel == TurretsInLevel - AliveTurretsToRestoreRegularEnemiesNumber)
    {
        for (auto it = MaxEnemiesInScreen.begin(); it != MaxEnemiesInScreen.end(); ++it)
        {
            if (InitialMaxEnemiesInScreen.Contains(it->Key))
            {
                it->Value = InitialMaxEnemiesInScreen[it->Key];
            }
            
        }
        MaxEnemiesInConquestZoneChanged.Broadcast(MaxEnemiesInScreen);
    }
}