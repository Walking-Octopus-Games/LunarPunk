// Fill out your copyright notice in the Description page of Project Settings.


#include "RewardsAndUpgrades/Plate.h"
#include "FMODBlueprintStatics.h"
#include "Components/CapsuleComponent.h"
#include "Player/PlayableCharacter.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "Managers/PlatesManager.h"


UClass* APlate::GetTypeOfReward_Implementation() {

	return APlate::StaticClass();
}

void APlate::SetQuantity(int32 NewQuantity)
{
	Quantity = NewQuantity;
	UpdateQuantityWidget();
}



void APlate::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //UE_LOG(LogTemp, Warning, TEXT("Cojo Reward."));
    if ((OtherActor != NULL) && OtherComp->GetClass() == UCapsuleComponent::StaticClass() && (OtherActor == Player))
    {

        GoToHUD(Quantity);
        ReturnToPool();
        ReceiveByPlayerAudioInstance = UFMODBlueprintStatics::PlayEventAttached(ReceiveByPlayerAudioEvent, OtherActor->GetRootComponent(), NAME_None, OtherActor->GetActorLocation(), EAttachLocation::KeepWorldPosition, true, true, true);

    }
}


void APlate::AddToHUD(int32 QuantityToAdd)
{
   
    Super::AddToHUD(QuantityToAdd);

    if(GameMode)
    {
        GameMode->GetPlatesManager()->AddPlates(QuantityToAdd);
    }
    //bIsActive = false;
    //Deactivate();
}