// Fill out your copyright notice in the Description page of Project Settings.


#include "SparkProjectile.h"
#include "Enemies/IEnemy.h"
#include "HealthSystem/HealthComponent.h"
#include "HealthSystem/DamageComponent.h"
#include "Player/PlayableCharacter.h"
#include <Kismet/GameplayStatics.h>
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "HealthSystem/DamageComponent.h"
#include "HealthSystem/HealthComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/AudioComponent.h"



// Sets default values
ASparkProjectile::ASparkProjectile()
{
	ShootEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ShootEffectComponent"));
	SparksDamageComponent = CreateDefaultSubobject<UDamageComponent>(TEXT("SparksDamageComponent"));
	//Create audio component.
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(FName("Audio Component"));
	
}



void ASparkProjectile::BeginPlay()
{
	Super::BeginPlay();

	PlayerReference = Cast<APlayableCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	//Set initial values.
	NumBouncess = InitNumBouncess;
	NumEnemiesAffectedFirst = InitNumEnemiesAffectedFirst;
	NumEnemiesAffectedInEachBounce = InitNumEnemiesAffectedInEachBounce;
	Range = InitRange;
	DamagePercentFromInit = InitDamagePercentFromInit;
	bContinuousDamageDecrease = bInitContinuousDamageDecrease;


	if (IsValid(AudioComponent) && IsValid(SparksSound))
	{
		AudioComponent->SetSound(SparksSound);
		AudioComponent->SetVolumeMultiplier(0.2);
	}


}



// Funtion that is called when the projectile overlap something
void ASparkProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (bIsActive)
	{

		Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

		IEnemy* Enemy = Cast<IEnemy>(OtherActor);
		if ((Enemy != nullptr) && (OtherActor != this) && (OtherComp != NULL) && IsValid(DamageComponent) && IsValid(SparksDamageComponent))
		{
			bIsActive = true;
			ActorsToBeSparksOrigins.Add(OtherActor);
			CurrentNumOtherActorsAffected = NumEnemiesAffectedFirst;
			++CurrentBouncess;
			SparksDamageComponent->DamageStrength = DamagePercentFromInit*DamageComponent->DamageStrength;
			DoBounce();

		}	

	}
    
	
}


void ASparkProjectile::DoBounce()
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
		

			if (CastedEnemyToPoolable!=nullptr && CastedEnemyToPoolable->bIsActive)
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
					GetWorld(), SparksOrigin, SphereEnd, Range * 100.0,	ObjectTypes, false,	
					ActorsToIgnore, EDrawDebugTrace::None, OutHits,	true, FLinearColor::Red, FLinearColor::Green, 1.0f														
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

							if (HitActorCasted!=nullptr && HittedActorCastedToPoolable!=nullptr && HittedActorCastedToPoolable->bIsActive)
							{

								//Make Damage:
								HittedHealthComponent = HitActorCasted->GetHealthComponent();
								if (IsValid(HittedHealthComponent))
								{
									SparksDamageComponent->MakeDamage(HittedHealthComponent);
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
				SparksDamageComponent->DamageStrength *= DamagePercentFromInit;
			}

			if (NextActorsToBeSparksOrigin.Num() > 0)
			{
				//Do the bounce with or without delay.
				if (DelaySparks > 0)
				{
					GetWorldTimerManager().SetTimer(TimerHandle, this, &ASparkProjectile::DoBounce, DelaySparks, false);
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
	bIsActive = false;
	CurrentBouncess = 0;

}



void ASparkProjectile::ResetStats()
{

	NumBouncess = InitNumBouncess;
	NumEnemiesAffectedFirst = InitNumEnemiesAffectedFirst;
	NumEnemiesAffectedInEachBounce = InitNumEnemiesAffectedInEachBounce;
	Range = InitRange;
	DamagePercentFromInit = InitDamagePercentFromInit;
	bContinuousDamageDecrease = bInitContinuousDamageDecrease;
}


void ASparkProjectile::Reactivate()
{
	Super::Reactivate();

	ResetStats();

}



void ASparkProjectile::SpawnNiagaraEffect(FVector SparksOriginLocation, FVector SparksEndLocation)
{
	if (IsValid(ShootEffectComponent) && ShootEffectComponent->GetAsset())
	{
		UNiagaraComponent* ShootEffectComponentCopy = ShootEffectComponent;

		ShootEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,
			ShootEffectComponent->GetAsset(),
			SparksOriginLocation,
			UKismetMathLibrary::FindLookAtRotation(SparksOriginLocation, SparksEndLocation)
		);

		ShootEffectComponent->SetNiagaraVariableVec3(FString("HitVector"), SparksEndLocation);
		ShootEffectComponent = ShootEffectComponentCopy;
	}

	AudioComponent->Play();

}


