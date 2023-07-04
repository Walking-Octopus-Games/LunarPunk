// Fill out your copyright notice in the Description page of Project Settings.


#include "Traps/ExplosivePipe.h"
#include "Enemies/BasicEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "HealthSystem/HealthComponent.h"
#include "Utilities/UtilitiesLunarpunk.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"



void AExplosivePipe::BeginPlay()
{
	Super::BeginPlay();

	Light = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName("Light"));

}

void AExplosivePipe::DoTrapEffect()
{

	UWorld* World = GetWorld();
	if (World)
	{

		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			World,
			ExplosionEffect,
			GetActorLocation(),
			GetActorRotation()
		);

		//Obtain the enemies in the cone.
		TArray<ABasicEnemy*> ActorsInCone;

		bool bHit = UUtilitiesLunarpunk::ObtainActorsInCone<ABasicEnemy>(
			World,
			GetActorLocation(),
			GetActorForwardVector(),
			Angle,
			Range,
			ActorsInCone,
			{ UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel3) },
			bDrawDebugCone,
			DrawTime
			);

		if (bHit)
		{
			UHealthComponent* HealthComponentEnemy = nullptr;
			for (ABasicEnemy* Enemy : ActorsInCone)
			{
				UNiagaraComponent* NiagaraComp2 = UNiagaraFunctionLibrary::SpawnSystemAttached(
					ImpactEffect,
					Enemy->MeshComponent,
					NAME_None,
					FVector(0.0f, 0.0f, 20.0f),
					FRotator::ZeroRotator,
					EAttachLocation::KeepRelativeOffset,
					true
				);

				for (auto it = DamagePercentageEachClass.begin(); it != DamagePercentageEachClass.end(); ++it)
				{
					if (Enemy->IsA(it->Key))
					{
						HealthComponentEnemy = Enemy->FindComponentByClass<UHealthComponent>();
						if (HealthComponentEnemy)
						{
							HealthComponentEnemy->ReceivePercentageDamage(it->Value);
						}
						HealthComponentEnemy = nullptr;
						break;
					}
				}
			}
		}


	}

}


void AExplosivePipe::DeactivateTrap()
{
	Super::DeactivateTrap();

	TurnOnDeactivatedLight();
}

void AExplosivePipe::ReactivateTrap()
{
	Super::ReactivateTrap();

	TurnOnActivatedLight();
}



void AExplosivePipe::TurnOnActivatedLight()
{
	if (Light)
	{
		Light->SetMaterial(0, ActivatedLightMaterial);
	}
}

void AExplosivePipe::TurnOnDeactivatedLight()
{
	if (Light)
	{
		Light->SetMaterial(0, DeactivatedLightMaterial);
	}
}