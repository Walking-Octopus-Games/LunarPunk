// Fill out your copyright notice in the Description page of Project Settings.


#include "Traps/Cauldron.h"
#include "Kismet/GameplayStatics.h"
#include "Utilities/UtilitiesLunarpunk.h"
#include "NiagaraFunctionLibrary.h"
#include "Delegates/Delegate.h"
#include "Enemies/BasicEnemy.h"
#include "HealthSystem/HealthComponent.h"
#include "HealthSystem/DamageComponent.h"
#include "Enemies/BasicEnemy.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"


ACauldron::ACauldron()
{
	PrimaryActorTick.bCanEverTick = true;

	FloorEffect = CreateDefaultSubobject<UDecalComponent>(TEXT("FloorEffect"));
	FloorEffect->SetVisibility(false);
	FloorEffect->SetupAttachment(RootComponent);

	BubblePoisonEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BubblePoisonEffect"));
	BubblePoisonEffect->SetVisibility(false);
	BubblePoisonEffect->SetupAttachment(RootComponent);

	

}



void ACauldron::BeginPlay()
{
	Super::BeginPlay();

	SphereSetRange = Cast<USphereComponent>(GetDefaultSubobjectByName("CauldronEffectRange"));

	//SphereSetRange->SetSphereRadius(Diameter);
	SphereSetRange->OnComponentBeginOverlap.AddDynamic(this, &ACauldron::OnOverlapBegin);
	//SphereSetRange->OnComponentEndOverlap.AddDynamic(this, &ACauldron::OnOverlapEnd);
	SphereSetRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//SphereSetRange = Cast<USphereComponent>(GetDefaultSubobjectByName("CauldronEffectRange"));
	//SphereSetRange->SetSphereRadius(Diameter);

	

	Light = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName("Light"));




}
void ACauldron::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (State == ECauldron::Enabled && OtherActor->IsA<ABasicEnemy>()) {
		//Activo componente poison
		
		ABasicEnemy* PoisonedEnemy = Cast<ABasicEnemy>(OtherActor);
		PoisonedHealthComponent = PoisonedEnemy->GetHealthComponent();
		if (IsValid(PoisonedHealthComponent))
		{
			//PoisonedHealthComponent->ReceiveDamage(DamageGranade);
			//MakePoisonDamage(PoisonedHealthComponent);
			
			
			PoisonedEnemy->PoisonComponent->MakeDamgePoison(TimeBetweenHits, DamagePerSecond , NumberOfHits, PoisonedHealthComponent);

			
		}




		UE_LOG(LogTemp, Warning, TEXT("Beacon Overlapping: %s"), *OtherActor->GetClass()->GetName());
		UE_LOG(LogTemp, Warning, TEXT("Detected shoot"));

	}

	
	
}
void ACauldron::MakePoisonDamage()
{
	
	PoisonedHealthComponent->ReceiveDamage(DamagePerSecond);

}





void ACauldron::DoTrapEffect()
{
	State = ECauldron::Enabled;
	SphereSetRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	BubblePoisonEffect->SetVisibility(true);
	FloorEffect->SetVisibility(true);

	GetWorldTimerManager().SetTimer(TimerHandleCauldron, this, &ACauldron::DisableTrapEffect, 5.0f, false);

	

	

	
}

void ACauldron::DisableTrapEffect()
{
	// Desactiva la línea
	State = ECauldron::Disabled;
	SphereSetRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BubblePoisonEffect->SetVisibility(false);
	FloorEffect->SetVisibility(false);
}

//void ACauldron::OverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (OtherActor->IsA<ABasicEnemy>())
//	{
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Enemigo detectado"));
//	}

//}

/*
void ACauldron::DeactivateTrap()
{
	Super::DeactivateTrap();

	TurnOnDeactivatedLight();
}

void ACauldron::ReactivateTrap()
{
	Super::ReactivateTrap();

	TurnOnActivatedLight();
}
*/


void ACauldron::TurnOnActivatedLight()
{
	if (Light)
	{
		Light->SetMaterial(0, ActivatedLightMaterial);
	}
}

void ACauldron::TurnOnDeactivatedLight()
{
	if (Light)
	{
		Light->SetMaterial(0, DeactivatedLightMaterial);
	}
}