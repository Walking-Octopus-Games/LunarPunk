// Fill out your copyright notice in the Description page of Project Settings.


#include "GetRewardsComponent.h"
#include "CoreMinimal.h"
#include "Plate.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "Managers/EntityManager.h"
#include "Kismet/GameplayStatics.h"
#include "Utilities/UtilitiesLunarpunk.h"


// Sets default values for this component's properties
UGetRewardsComponent::UGetRewardsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGetRewardsComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(GetOwner()))
	{
		DrawInCone = Cast<UStaticMeshComponent>(GetOwner()->GetDefaultSubobjectByName(TEXT("DrawInCone")));
	}

	GameMode = Cast<ALunarPunkGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (bAutomaticDrawIn)
	{
		bIsDrawingIn = true;
	}

}


// Called every frame
void UGetRewardsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Do the draw in.
	if (bIsDrawingIn)
	{
		const UWorld* World = GetWorld();
		const AActor* Owner = GetOwner();
		if (World && Owner)
		{
			//Obtain actors in the cone
			TArray<AReward*> RewardsInCone;
			UUtilitiesLunarpunk::ObtainActorsInCone<AReward>(
				World,
				Owner->GetActorLocation(),
				GetOwner()->GetActorForwardVector(),
				DrawInAngle,
				DrawInDistance,
				RewardsInCone,
				{ UEngineTypes::ConvertToObjectType(ECC_WorldDynamic) }
			);

			//Try to draw in all the rewards in the cone.
			for (AReward* Reward : RewardsInCone)
			{
				//Only gets the reward if there is anywhing between the player and the reward
				//if (!UUtilitiesLunarpunk::CheckIsAnythingBetweenActorAndTarget(GetWorld(), Reward->GetActorLocation(), Owner))//, ECollisionChannel::ECC_WorldStatic, true, 5.0f) )
				//{
					Reward->BeingDrawIn(Owner->GetTransform(), DeltaTime * DrawInForce);
				//}
			}
		}

		if (bDrawCone)
		{
			if (!DrawInCone->IsVisible())
			{
				DrawCone();
			}
		}
	}
	else
	{
		if (bDrawCone)
		{
			if (DrawInCone->IsVisible())
			{
				DrawInCone->SetVisibility(false);
			}
		}

	}
}


//Function to start draw in
void UGetRewardsComponent::StartDrawingIn()
{
	bIsDrawingIn = true;
	//StartDrawIn.Broadcast();
}

//Function to Stop Draw in
void UGetRewardsComponent::StopDrawingIn()
{
	bIsDrawingIn = false;
	//StopDrawIn.Broadcast();
}

void UGetRewardsComponent::DrawCone() const
{
	if (GetOwner() != nullptr)
	{
		DrawInCone->SetVisibility(true);
	}
}

