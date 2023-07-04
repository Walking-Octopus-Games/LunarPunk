// Fill out your copyright notice in the Description page of Project Settings.


#include "DropRewardsComponent.h"
#include "Pool/Pool.h"
#include <Kismet/GameplayStatics.h>
#include "Plate.h"

// Sets default values for this component's properties
UDropRewardsComponent::UDropRewardsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDropRewardsComponent::BeginPlay()
{
	Super::BeginPlay();

	//Comprobamos que la probabilidad de las distintas mejoras suma 1 y si no es así, lo corregimos.
	if (EachUpgradeProbs.Num() > 0)
	{
		float TotalProbUpgrades = 0.0f;
		for (const auto& upgrade : EachUpgradeProbs)
		{
			TotalProbUpgrades += upgrade.Value;
		}
		if (TotalProbUpgrades != 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("RewardsComponent.cpp: La probabilidad total de las Upgrades no suma 1. Se ajusta la del primero"));
			EachUpgradeProbs.begin()->Value += 1 - TotalProbUpgrades;
			//Nota: Puede que aquí salga negativo si nos hemos pasado por arriba.
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RewardsComponent.cpp: Upgrades haven't been set."));
	}
	// ...


	if (GetOwner() == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("RewardsComponent.cpp: Owner not found."));
	}

	/*ActorDeath.AddDynamic(this,&UDropRewardsComponent::DropRewards);*/


	//Obtain the pool reference.
	MyPool = Cast<APool>(UGameplayStatics::GetActorOfClass(GetWorld(), APool::StaticClass()));
	if (!IsValid(MyPool))
	{
		UE_LOG(LogTemp, Warning, TEXT("RewardsComponent.cpp: Pool not found"));
	}

	BaseNumPlatesToDrop = NumPlatesToDrop;
	bBaseRandomPlates = bRandomPlates;
	BaseUpgradesProb = UpgradesProb;
}


// Called every frame
void UDropRewardsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UDropRewardsComponent::DropRewards()
{

	if (bRandomPlates)
	{
		PlatesToDrop = FMath::RandRange(MinPlatesDropped, MaxPlatesDropped);
	}
	else
	{
		PlatesToDrop = NumPlatesToDrop;
	}

	if (IsValid(GetOwner()) && IsValid(MyPool))
	{

		FVector ActorLocation = GetOwner()->GetActorLocation();
		FVector Offset;

		//Hacemos que salgan en un punto aleatorio dentro de un radio.
		Offset = FVector(FMath::FRandRange(-Radius, Radius), FMath::FRandRange(-Radius, Radius), ZOffset);
		//Takes the next available actor in the pool
	
		AActor* NextPlateActor = MyPool->GetNextActor(PlatesClass);

		/*int32 Num = MyPool->NumActiveActorsOfClass(PlatesClass);
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("PlatesActivas: %i"), Num));*/

		if (IsValid(NextPlateActor))
		{
			NextPlateActor->SetActorLocation(ActorLocation + Offset);
			NextPlateActor->SetActorRotation(FRotator(FMath::FRandRange(0, 360), FMath::FRandRange(0, 360), FMath::FRandRange(0, 360)));
			if (NextPlateActor->Implements<UIsPoolable>())
			{
				Cast<IIsPoolable>(NextPlateActor)->Reactivate();
			}

			
			//Set the quantity.
			APlate* NextPlateActorCasted = Cast<APlate>(NextPlateActor);
			if (IsValid(NextPlateActorCasted))
			{
				NextPlateActorCasted->SetQuantity(PlatesToDrop);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("RewardsComponent.cpp: No plates left in the pool"));
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("No quedan plates en la pool (activas = %i )"), Num));
		}

		//Número aleatorio para ver si se genera mejora.
		float lottery = FMath::FRand();

		//Si quisiésemos generar varias mejoras, aquí se podría poner un bucle.
		if (lottery <= UpgradesProb)
		{
			//Si solo hay un elemento dentro de los Upgrades, lo Spawneamos sí o sí.
			if (EachUpgradeProbs.Num() == 1)
			{
				Offset = FVector(FMath::FRandRange(-Radius, Radius), FMath::FRandRange(-Radius, Radius), ZOffset);
				//world->SpawnActor<AActor>(EachUpgradeProbs.begin()->Key, actorLocation + offset, FRotator::ZeroRotator);
		
				AActor* NextActor = MyPool->GetNextActor(EachUpgradeProbs.begin()->Key);

				if (IsValid(NextActor))
				{
					NextActor->SetActorLocation(ActorLocation + Offset);
					if (NextActor->Implements<UIsPoolable>())
					{
						Cast<IIsPoolable>(NextActor)->Reactivate();
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("RewardsComponent.cpp: No upgrades left in the pool"));
				}
			}
			else //Si hay varios elementos, lanzamos otro número aleatorio y vemos a ver cuál de ellos toca.
			{
				lottery = FMath::FRand();
				float LowerAcumulative = 0;
				float UpperAcumulative = 0;
				for (const auto& upgrade : EachUpgradeProbs)
				{
					UpperAcumulative += upgrade.Value;
					if (LowerAcumulative <= lottery && lottery <= UpperAcumulative)
					{
						Offset = FVector(FMath::FRandRange(0, Radius), FMath::FRandRange(0, Radius), ZOffset);
			
						AActor* NextActor = MyPool->GetNextActor(upgrade.Key);
						if (IsValid(NextActor))
						{
							NextActor->SetActorLocation(ActorLocation + Offset);
							if (NextActor->Implements<UIsPoolable>())
							{
								Cast<IIsPoolable>(NextActor)->Reactivate();
							}
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("RewardsComponent.cpp: No upgrades left in the pool"));
						}
					
						//Hacemos un break si ya ha generado una mejora.
						break;
					}
				}
			}

		}

	}

}

void UDropRewardsComponent::ResetValues()
{
	NumPlatesToDrop = BaseNumPlatesToDrop;
	bRandomPlates = bBaseRandomPlates;
	UpgradesProb = BaseUpgradesProb;
}