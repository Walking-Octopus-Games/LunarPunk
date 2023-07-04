// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/UtilitiesLunarpunk.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "Managers/PlatesManager.h"

bool UUtilitiesLunarpunk::CheckIsAnythingBetweenActorAndTarget(const UWorld* WorldContext, const FVector& Origin, const AActor* Target, ECollisionChannel CollisionChannel, bool Draw, float Time)
{

	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	FCollisionResponseParams ResponseParam;

	bool bHit = false;
	

	bHit = WorldContext->LineTraceSingleByChannel(
		Hit,
		Origin,
		Target->GetActorLocation(),
		CollisionChannel,
		CollisionParams,
		ResponseParam
		);


	if (Draw)
	{
		DrawDebugLine(
			WorldContext,
			Origin,
			Target->GetActorLocation(),
			FColor::Red,
			false,
			Time
		);
	}

	if (bHit)
	{
		if (Draw)
		{
			DrawDebugSphere(
				WorldContext,
				Hit.Location,
				50.0f,
				8,
				FColor::Red,
				false,
				Time
			);
		}
		

		if (Hit.GetActor() == Target)
		{
			return false;
		}
	}

	

	return true;
}


int32 UUtilitiesLunarpunk::GetPlayerPlates(const UWorld* WorldContext)
{

	ALunarPunkGameMode* GameMode = Cast<ALunarPunkGameMode>(UGameplayStatics::GetGameMode(WorldContext));

	if (GameMode && GameMode->PlatesManager)
	{
		return GameMode->PlatesManager->GetCurrentPlates();
	}

	return 0;
}