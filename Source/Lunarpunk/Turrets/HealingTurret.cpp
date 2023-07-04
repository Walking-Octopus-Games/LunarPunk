// Fill out your copyright notice in the Description page of Project Settings.


#include "Turrets/HealingTurret.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Projectiles/Projectile.h"

#include "Components/SphereComponent.h"

#include "Enemies/BasicEnemy.h"

#include "HealthSystem/HealthComponent.h"

#include "NiagaraComponent.h"

#include "Particles/ParticleSystemComponent.h"


AHealingTurret::AHealingTurret()
{
  HealingZone = CreateDefaultSubobject<USphereComponent>(TEXT("HealingZone"));
  HealingEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("HealingEffect"));

  HealingZone->SetupAttachment(MyRootComponent);
  HealingEffect->SetupAttachment(HealingZone);
  HealingEffect->AutoAttachLocationRule = EAttachmentRule::SnapToTarget;
  HealingZone->SetSphereRadius(400.0f);
  //HealingZone->SetRelativeLocation(FVector(0, 0, 0));
  //HealingEffect->SetRelativeLocation(FVector(0, 0, -15.0));

  
}

void AHealingTurret::BeginPlay()
{
  Super::BeginPlay();

  TypeOfTurret = ETurretType::HealingTurret;
  HealingEffect->Deactivate();

}

void AHealingTurret::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
}

void AHealingTurret::StopTurret()
{
  DeactivateHealingEffect();
}

void AHealingTurret::HealAdjacentTurrets()
{
  TArray<AActor*> OverlapedActors;
  this->GetOverlappingActors(OverlapedActors, ATurret::StaticClass());

  if (OverlapedActors.Num() > 0)
  {
    //RestartHealingEffect();
    for (AActor* Actor : OverlapedActors)
    {
      ATurret* Turret = Cast<ATurret>(Actor);
      float HealthToRestore = Turret->HealthComponent->GetMaxHealth() * (HealingPercent / 100);
      if (Turret->GetState() == ETurretState::Deactivated)
      {
        Turret->GetDeactivatedEffectComponent()->Deactivate();
      }
      if (HealthToRestore >= Turret->HealthComponent->GetMaxHealth())
      {
        Turret->HealthComponent->ResetHealth();
      }
      else
      {
        float ActualHealth = Turret->HealthComponent->GetActualHealth();
        if (ActualHealth + HealthToRestore >= Turret->HealthComponent->GetMaxHealth())
        {
          Turret->HealthComponent->ResetHealth();
        }
        else
        {
          Turret->HealthComponent->ReceiveDamage(-HealthToRestore);
        }
      }

    }
  }
}

void AHealingTurret::RestartHealingEffect()
{
  GetWorld()->GetTimerManager().ClearTimer(HealingEffectTimer);
  GetWorld()->GetTimerManager().SetTimer(HealingEffectTimer, this, &AHealingTurret::DeactivateHealingEffect, 2.0f, true);
  HealingEffect->Activate();
}

void AHealingTurret::DeactivateHealingEffect()
{
  HealingEffect->Deactivate();
}

void AHealingTurret::UpgradeTurret(FUpgradesDataStruct* Upgrade, int Quantity)
{
    Super::UpgradeTurret(Upgrade, Quantity);

    float IncreaseHealing = GetIncreaseHealing(*Upgrade, Quantity);
    HealingPercent += HealingPercent * IncreaseHealing;

}


float AHealingTurret::GetIncreaseHealing(FUpgradesDataStruct Upgrade, int AddNext)
{
    if (Upgrade.Type == EUpgradeType::Power)
    {
        int* NumberOfUpgradesApplied = AppliedUpgrades.Find(Upgrade.Type);
        int NumberApplied;
        if (NumberOfUpgradesApplied)
        {
            NumberApplied = *NumberOfUpgradesApplied + AddNext;

        }
        else
        {
            NumberApplied = AddNext;
        }

        float valueOfIncrease = 0.0f;


        for (int i = 0; i < Upgrade.IncreasesInfo.Num(); i++)
        {
            if (Upgrade.IncreasesInfo[i].Type == TypeOfTurret)
            {
                for (int j = 0; j < Upgrade.IncreasesInfo[i].MaxRanges.Num(); j++)
                {
                    if (NumberApplied <= Upgrade.IncreasesInfo[i].MaxRanges[j])
                    {
                        valueOfIncrease = Upgrade.IncreasesInfo[i].IncreasesOfHealing[j];
                        break;
                    }
                }
                break;
            }
        }

        return valueOfIncrease;
    }
    return 0.0f;
}
