// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Abilities/SparkAbilityComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayableCharacter.h"
#include "Managers/AbilitiesManger.h"
#include "Managers/EntityManager.h"
#include "Managers/PlatesManager.h"
#include "Managers/GameFrameworks/LunarPunkGameMode.h"
#include "Enemies/IEnemy.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Pool/IsPoolable.h"
#include "HealthSystem/HealthComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/DecalComponent.h"

USparkAbilityComponent::USparkAbilityComponent()
{
    AbilityType = EAbilityType::Spark;
	//Create audio component.
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(FName("SparkAudioComponent"));
}


void USparkAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(AudioComponent) && IsValid(SparksSound))
	{
		AudioComponent->SetSound(SparksSound);
		AudioComponent->SetVolumeMultiplier(0.2);
	}

	SparksEffectComponent = Cast<UNiagaraComponent>(GetOwner()->GetDefaultSubobjectByName(FName("SparksEffect")));

	if (SparksEffectComponent)
	{
		SparksEffectComponent->DeactivateImmediate();
	}
}

void USparkAbilityComponent::Initialize(ALunarPunkPlayerController* _PlayableController, APlayableCharacter* _PlayableCharacter)
{
    Super::Initialize(_PlayableController, _PlayableCharacter);
	PlayerReference = _PlayableCharacter;
}

void USparkAbilityComponent::ThrowAbility()
{
	if (AssertThrowAbility())
	{
		ThrowSparkleEvent.Broadcast();
		SpawnLaunchSparkNiagaraEffect();
		ThrowSparks();
		Super::ThrowAbility();
	}
}

void USparkAbilityComponent::StopAbility()
{
}

void USparkAbilityComponent::Button2Pressed(bool bIsPressed)
{
	Super::Button2Pressed(bIsPressed);
}

void USparkAbilityComponent::ThrowSparks()
{
	if (GetOwner())
	{
		ActorsToBeSparksOrigins.Add(GetOwner());
		CurrentNumOtherActorsAffected = NumEnemiesAffectedFirst;
		DamageSparks = InitDamageSparks;
		PercentageDamageSparks = InitPercentageDamageSparks;
		++CurrentBouncess;
		DoBounce();
	}
}


void USparkAbilityComponent::DoBounce()
{
	if (ActorsToBeSparksOrigins.Num() > 0)
	{
		//Array to store the next actors origins of the bouncess.
		TArray<AActor*> NextActorsToBeSparksOrigin;
		AActor* CurrentActorSparksOrigin;


		//MultiSphere params:
		FVector SparksOrigin;
		FVector SphereEnd;
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel3));// ECC_Pawn));ECC_GameTraceChannel1
		TArray<FHitResult> OutHits;
		bool bResult;
		IIsPoolable* CastedEnemyToPoolable;

		if (!ActorsToIgnore.Contains(PlayerReference))
		{
			ActorsToIgnore.Add(PlayerReference);
		}

		for (int i = 0; i < ActorsToBeSparksOrigins.Num(); ++i)
		{

			CurrentActorSparksOrigin = ActorsToBeSparksOrigins[i];

			//Check if the actor is poolable and it's active.
			CastedEnemyToPoolable = Cast<IIsPoolable>(CurrentActorSparksOrigin);


			if ((CastedEnemyToPoolable != nullptr && CastedEnemyToPoolable->bIsActive) || CurrentActorSparksOrigin == PlayerReference)
			{

				//Update sparks origin
				SparksOrigin = CurrentActorSparksOrigin->GetActorLocation();
				SphereEnd = SparksOrigin + FVector(0, 0, 15.0f);

				//Add the current actor to ignore them in the next bouncess
				if (!ActorsToIgnore.Contains(CurrentActorSparksOrigin))
				{
					ActorsToIgnore.Add(CurrentActorSparksOrigin);
				}

				//Throw multispehere 
				bResult = UKismetSystemLibrary::SphereTraceMultiForObjects(
					GetWorld(), SparksOrigin, SphereEnd, TraceRange, ObjectTypes, false,
					ActorsToIgnore, EDrawDebugTrace::None, OutHits, true, FLinearColor::Red, FLinearColor::Green, 1.0f
				);

				//If there are Pawns in the range:
				if (bResult)
				{
					AActor* HitActor;
					IEnemy* HitActorCasted;
					IIsPoolable* HittedActorCastedToPoolable;
					UHealthComponent* HittedHealthComponent;
					int32 Counter = 0;

					//Array to store the already hitted actors to ignore them in the current bounce.
					TArray<AActor*> HittedActors;

					//Sort the hits array by distance to the origin.
					OutHits.Sort([SparksOrigin](const FHitResult& A, const FHitResult& B)
						{
							float D1 = FVector::DistSquared(A.GetActor()->GetActorLocation(), SparksOrigin);
							float D2 = FVector::DistSquared(B.GetActor()->GetActorLocation(), SparksOrigin);
							return D1 < D2;
						}
					);

					//Check the hits and throw the sparks
					for (int j = 0; j < OutHits.Num(); ++j)
					{

						HitActor = OutHits[j].GetActor();

						if (!HittedActors.Contains(HitActor))
						{
							//Add the current actor to the array of hitted actors.
							HittedActors.Add(HitActor);
							HitActorCasted = Cast<IEnemy>(HitActor);
							HittedActorCastedToPoolable = Cast<IIsPoolable>(HitActor);

							if (HitActorCasted != nullptr && HittedActorCastedToPoolable != nullptr && HittedActorCastedToPoolable->bIsActive)
							{

								//Make Damage:
								HittedHealthComponent = HitActorCasted->GetHealthComponent();
								if (IsValid(HittedHealthComponent))
								{
									if (DamageMode == ESparksDamageMode::Value)
									{
										HittedHealthComponent->ReceiveDamage(DamageSparks);
									}

									if (DamageMode == ESparksDamageMode::Percentage)
									{
										HittedHealthComponent->ReceiveDamage(PercentageDamageSparks * HittedHealthComponent->GetMaxHealth());
									}
									
									//SparksDamageComponent->MakeDamage(HittedHealthComponent);
								}

								//Spawns the niagara effect.
								SpawnNiagaraEffect(SparksOrigin, HitActor->GetActorLocation());

								//Add the hit actors to ignore him in the next bouncess.
								ActorsToIgnore.Add(HitActor);

								//If there are next bouncess, then add the current actor to the array of origins for this bouncess.
								if (CurrentBouncess < NumBouncess)
								{
									NextActorsToBeSparksOrigin.Add(HitActor);
								}
								//Refresh the num of actors hitted.
								++Counter;

								//Handle the next bouncess or finish the effect.
								if (Counter >= CurrentNumOtherActorsAffected)
								{
									break;
								}
							}
						}
					}
				}
			}
		}

		//Do the next bounce
		if (CurrentBouncess < NumBouncess)
		{
			ActorsToBeSparksOrigins.Empty();
			ActorsToBeSparksOrigins = NextActorsToBeSparksOrigin;
			CurrentNumOtherActorsAffected = NumEnemiesAffectedInEachBounce;
			++CurrentBouncess;

			//If true, decrease the damage strength of the next bounce.
			if (bContinuousDamageDecrease)
			{
				DamageSparks *= DamagePercentFromInit;
				PercentageDamageSparks *= DamagePercentFromInit;
			}

			if (NextActorsToBeSparksOrigin.Num() > 0)
			{
				//Do the bounce with or without delay.
				if (DelaySparks > 0)
				{
					GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USparkAbilityComponent::DoBounce, DelaySparks, false);
				}
				else
				{
					DoBounce();
				}

				return;
			}
		}
	}

	//Reset the actors to ignore Array.
	ActorsToIgnore.Empty();
	ActorsToBeSparksOrigins.Empty();
	CurrentBouncess = 0;

}



void USparkAbilityComponent::SpawnNiagaraEffect(FVector SparksOriginLocation, FVector SparksEndLocation)
{

	if (SparkEffect)
	{
		UNiagaraComponent* SparkEffectComponent;

		SparkEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			SparkEffect,
			SparksOriginLocation,
			UKismetMathLibrary::FindLookAtRotation(SparksOriginLocation, SparksEndLocation)
			);

		SparkEffectComponent->SetNiagaraVariableVec3(FString("HitVector"), SparksEndLocation);
		
	}

	AudioComponent->Play();

}


void USparkAbilityComponent::SpawnLaunchSparkNiagaraEffect()
{

	/*if (LaunchSparkEffect)
	{
		UNiagaraComponent* SparkLaunchEffectComponent;

		SparkLaunchEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			LaunchSparkEffect,
			GetOwner()->GetActorLocation()
		);

		SparkLaunchEffectComponent->SetNiagaraVariableFloat(FString("Radius"), RadiusLaunchEffect);
		SparkLaunchEffectComponent->SetNiagaraVariableFloat(FString("RadiusSparks"), RadiusLaunchEffect*100);

	}*/
	if (SparksEffectComponent)
	{
		SparksEffectComponent->ActivateSystem();
	}


}


