// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthSystem/PoisonComponent.h"
#include "DamageComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Utilities/UtilitiesLunarpunk.h"
#include "NiagaraFunctionLibrary.h"
#include "Delegates/Delegate.h"

#include "HealthComponent.h"



// Sets default values for this component's properties
UPoisonComponent::UPoisonComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPoisonComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPoisonComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UPoisonComponent::MakeDamgePoison(float TimeBetweenHits, float DamagePerSecond, float NumberOfHits, UHealthComponent* HealthComponent) {

	World = GetWorld();
	//GetOwner().damage

	for (int i = 0; i < NumberOfHits; i++) {
		//PoisonedHealthComponent->ReceiveDamage(DamagePerSecond);
		//GetWorld()->GetTimerManager().SetTimer(TimerHandleCauldron2, [&]() {}, TimeBetweenHits, false);


		//MakePoisonDamage(PoisonedHealthComponent);
		DamagePerSecondPoison = DamagePerSecond;
		HealthComponentPoison = HealthComponent;
		NumberOfHitsPoison = NumberOfHits;
		//GetOwner()->GetWorldTimerManager().SetTimer(TimerHandlePoison2, this, &UPoisonComponent::MakeDamage, TimeBetweenHits, false);
		World->GetTimerManager().SetTimer(TimerHandlePoison2, this, &UPoisonComponent::MakeDamage, TimeBetweenHits, true);
		//World->GetTimerManager().SetTimer(TimerHandlePoison2, this, &UPoisonComponent::MakeDamage, TimeBetweenHits, false);
		
	}
}

void UPoisonComponent::MakeDamage(){

	if (NumberOfHitsPoison > 0)
	{
		HealthComponentPoison->ReceiveDamage(DamagePerSecondPoison);
		--NumberOfHitsPoison;
	}
	else
	{
		World->GetTimerManager().ClearTimer(TimerHandlePoison2);
	}

}





