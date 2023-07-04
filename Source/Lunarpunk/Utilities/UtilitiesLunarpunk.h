// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DrawDebugHelpers.h"
#include "UtilitiesLunarpunk.generated.h"

/**
 * 
 */
UCLASS()
class LUNARPUNK_API UUtilitiesLunarpunk : public UObject
{
	GENERATED_BODY()
	

public:


	static bool CheckIsAnythingBetweenActorAndTarget(const UWorld* WorldContext, const FVector& Origin, const AActor* Target, ECollisionChannel CollisionChannel = ECC_WorldStatic, bool Draw = false, float Time = 5.0f);


	static int32 GetPlayerPlates(const UWorld* WorldContext);


	//Obtain the actors in a range looking for them in the specified object types.
	template<typename T>
	static bool ObtainActorsInRange(const UWorld* WorldContext, const FVector& Center, float Radius, TArray<T*>& OutActors, const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, bool Draw = false, float Time = 5.0f)
	{
		OutActors.Empty();

		if (!WorldContext)
		{
			return false;
		}

		EDrawDebugTrace::Type DrawType = Draw ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

		//MultiSphere params:
		TArray<FHitResult> OutHits;
		TArray<AActor*>ActorsToIgnore;
		bool bResult;

		//Throw multispehere 
		bResult = UKismetSystemLibrary::SphereTraceMultiForObjects(
			WorldContext, Center, Center + FVector(0, 0, 15.0f), 100.0 * Radius, ObjectTypes, false,
			ActorsToIgnore, DrawType, OutHits, true, FLinearColor::Red, FLinearColor::Green, Time
		);

		if (bResult)
		{
			T* CurrentHit;

			//Add all the actors in range to the array.
			for (FHitResult Hit : OutHits)
			{
				CurrentHit = Cast<T>(Hit.GetActor());
				if (CurrentHit && !OutActors.Contains(CurrentHit))
				{
					OutActors.Add(CurrentHit);
				}
			}
		}

		return OutActors.Num() > 0 ? true : false;

	}


	//Given an array with actors, checks which of that actors are in the given angle through a Forward an return an array containing them.
	//That is, checks if the actors are in the cone with the specified angle and origin (center).
	template<typename T>
	static bool CheckActorsInAngle(const UWorld* WorldContext, const FVector& Center, const FVector& Forward, const TArray<T*>& InActors, TArray<T*>& OutActors, float AngleToCheck, bool DrawCone = false, float LenghtCone = 5.0f, float Time = 3.0f)
	{

		OutActors.Empty();

		if (!WorldContext)
		{
			return false;
		}

		float AngleTolerance = FMath::Cos(FMath::DegreesToRadians(0.5 * AngleToCheck));
		FVector CenterToActorDirection;
		float DotProductValue;
		for (T* Actor : InActors)
		{
			CenterToActorDirection = (Actor->GetActorLocation() - Center).GetSafeNormal();
			DotProductValue = FVector::DotProduct(Forward, CenterToActorDirection);
			if (DotProductValue >= AngleTolerance)
			{
				OutActors.Add(Actor);
				if (DrawCone)
				{
					DrawDebugSphere(WorldContext, Actor->GetActorLocation(), 50.0f, 6, FColor::Green, false, Time);
				}
			}
		}

		if (DrawCone)
		{
			DrawDebugCone(WorldContext, Center, Forward, 100 * LenghtCone, FMath::DegreesToRadians(0.5 * AngleToCheck), 0.0, 10, FColor::Red, false, Time);
		}

		return OutActors.Num() > 0 ? true : false;

	}


	//Obtain the actors of the specified object types in a a cone of the specified lenght and angle.
	template<typename T>
	static bool ObtainActorsInCone(const UWorld* WorldContext, const FVector& Center, const FVector& Forward, float AngleToCheck, float Lenght, TArray<T*>& OutActors, const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, bool Draw = false, float Time = 5.0f)
	{

		TArray<T*> ActorsInRange;

		bool bActorsInRange = ObtainActorsInRange<T>(
			WorldContext,
			Center,
			Lenght,
			ActorsInRange,
			ObjectTypes,
			false,
			false
			);

		if (!bActorsInRange)
		{
			return false;
		}

		CheckActorsInAngle<T>(
			WorldContext,
			Center,
			Forward,
			ActorsInRange,
			OutActors,
			AngleToCheck,
			Draw,
			Lenght,
			Time
			);

		return OutActors.Num() > 0 ? true : false;
	}


};
