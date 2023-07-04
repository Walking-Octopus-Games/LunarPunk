// Fill out your copyright notice in the Description page of Project Settings.


#include "RewardsAndUpgrades/Upgrade.h"
#include "FMODBlueprintStatics.h"
#include "Components/CapsuleComponent.h"
#include "Player/PlayableCharacter.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "Managers/PlatesManager.h"
#include "UpgradesData.h"
#include "Managers/UpgradesManager.h"

AUpgrade::AUpgrade()
{

}

UClass* AUpgrade::GetTypeOfReward_Implementation() {

	return AUpgrade::StaticClass();
}



void AUpgrade::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //UE_LOG(LogTemp, Warning, TEXT("Cojo Reward."));
    if ((OtherActor != NULL) && OtherComp->GetClass() == UCapsuleComponent::StaticClass() && (OtherActor == Player))
    {

        GoToHUD(0);
        ReturnToPool();
        ReceiveByPlayerAudioInstance = UFMODBlueprintStatics::PlayEventAttached(ReceiveByPlayerAudioEvent, OtherActor->GetRootComponent(), NAME_None, OtherActor->GetActorLocation(), EAttachLocation::KeepWorldPosition, true, true, true);

    }
}


void AUpgrade::AddToHUD(int32 QuantityToAdd)
{
    Super::AddToHUD(QuantityToAdd);

    if (GameMode && GameMode->UpgradesManager->GetNumberOfUpgrade(Type) < GameMode->UpgradesManager->UpgradesData->Upgrades.Find(Type)->MaxUpgrades)
    {
        //Adds the reward to the GetRewards Component of the player.
        GameMode->UpgradesManager->AddUpgrade(Type, 1);
    }

}

