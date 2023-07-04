// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PositionHelperIcon.generated.h"

class APlayableCharacter;
class UHealthComponent;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUNARPUNK_API UPositionHelperIcon : public UActorComponent
{
	GENERATED_BODY()

public:	


	UPROPERTY(EditAnywhere, Category = "Config.")
	float OffsetFromScreenToShowIcon = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Config.")
	FVector2D IconOffsetFromBorder = FVector2D(84.0f, 84.0f);

	UPROPERTY(EditAnywhere, Category = "Config.")
	float AngleOffset = 30.0f;

	FTimerHandle TimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bShowingIconInHUD = false;
	
	// Sets default values for this component's properties
	UPositionHelperIcon();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void StartPositionIcon();

	UFUNCTION(BlueprintCallable)
	void StopPositionIcon();

	void UpdateIconDisplay();

	void SetStayHidden(bool Hidden) { bStayHidden = Hidden; }

	UFUNCTION(BlueprintImplementableEvent)
	void ShowIconInHUD();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateIconPositionInHUD(const FVector2D& HUDPosition, float AngleRotation);

	UFUNCTION(BlueprintImplementableEvent)
	void RemoveIconInHUD();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowDamageEffect(float Damage);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowDeathEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowReactivatedEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void InitializeWidget();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;


private:

	AActor* Owner;

	APlayableCharacter* Player;

	bool bCheckIconPosition = false;

	UHealthComponent* HealthComponentOwner;

	bool bIsInitialized = false;

	bool bStayHidden = false;
		
};
