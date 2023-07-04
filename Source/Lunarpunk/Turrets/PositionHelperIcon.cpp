// Fill out your copyright notice in the Description page of Project Settings.


#include "Turrets/PositionHelperIcon.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayableCharacter.h"
#include "Player/LunarPunkPlayerController.h"
#include "DrawDebugHelpers.h"
#include "Player/CameraManager.h"
#include "Player/LunarPunkPlayerController.h"
#include "HealthSystem/HealthComponent.h"
#include "Turrets/Turret.h"

// Sets default values for this component's properties
UPositionHelperIcon::UPositionHelperIcon()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPositionHelperIcon::BeginPlay()
{
	Super::BeginPlay();

	// ...

	Owner = GetOwner();

	HealthComponentOwner = Owner->FindComponentByClass<UHealthComponent>();

	if (HealthComponentOwner)
	{
		HealthComponentOwner->DamageReceived.AddDynamic(this, &UPositionHelperIcon::ShowDamageEffect);
		HealthComponentOwner->ActorDeath.AddDynamic(this, &UPositionHelperIcon::ShowDeathEffect);
	}

	ATurret* OwnerCasted = Cast<ATurret>(Owner);

	if (OwnerCasted)
	{
		OwnerCasted->TurretReactivated.AddDynamic(this, &UPositionHelperIcon::ShowReactivatedEffect);
	}

	Player = Cast <APlayableCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	//StartPositionIcon();
	
}


// Called every frame
void UPositionHelperIcon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bCheckIconPosition)
	{
		UpdateIconDisplay();
	}
	// ...
}


void UPositionHelperIcon::StartPositionIcon()
{

	if (!bIsInitialized)
	{
		InitializeWidget();
		bIsInitialized = true;
	}

	bCheckIconPosition = true;

}


void UPositionHelperIcon::StopPositionIcon()
{
	bCheckIconPosition = false;
	bShowingIconInHUD = false;
	RemoveIconInHUD();
}


void UPositionHelperIcon::UpdateIconDisplay()
{

	
	if (!Owner || !Player)
	{
		return;
	}

	//Obtains the screen location of owner of the component
	FVector OwnerLocation = Owner->GetActorLocation();
	FVector2D OwnerScreenLocation;
	bool bProjection = UGameplayStatics::ProjectWorldToScreen(UGameplayStatics::GetPlayerController(GetWorld(), 0), OwnerLocation, OwnerScreenLocation);

	FVector2D Size;
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
	{
		Size = GEngine->GameViewport->Viewport->GetSizeXY();
	}
	else
	{
		return;
	}


	//Checks if the owner is out of the screen using the vieport size and the previous boolean:
	if(!bProjection || OwnerScreenLocation.X< -OffsetFromScreenToShowIcon || OwnerScreenLocation.Y< -OffsetFromScreenToShowIcon || OwnerScreenLocation.X > Size.X+ OffsetFromScreenToShowIcon || OwnerScreenLocation.Y > Size.Y + OffsetFromScreenToShowIcon)
	{

		//If the icon is not currently painted, then show it.
		if (!bShowingIconInHUD)
		{
			ShowIconInHUD();
			bShowingIconInHUD = true;
		}

		//Now we are going to project into the screen the player location and the player location with an offset in the Player->owner direction
		//in order to obtain the direction, in the screen, to the player to the owner.
		FVector PlayerLocation = Player->GetActorLocation();
		FVector DirOwnerPlayer = OwnerLocation - PlayerLocation;
		DirOwnerPlayer.Normalize();

		FVector2D PlayerScreenPosition;
		FVector2D PlayerScreenPositionOffset;

		UGameplayStatics::ProjectWorldToScreen(UGameplayStatics::GetPlayerController(GetWorld(),0), PlayerLocation, PlayerScreenPosition);
		UGameplayStatics::ProjectWorldToScreen(UGameplayStatics::GetPlayerController(GetWorld(), 0), PlayerLocation+300* DirOwnerPlayer, PlayerScreenPositionOffset);

		//This is the Player-owner direction in the screen.
		FVector2D DirOwnerInScreen = PlayerScreenPositionOffset - PlayerScreenPosition;

		//Adjust the size to show correctly the icons in the right and inferior border of the screen.
		Size -= IconOffsetFromBorder;


		//The Player-Owner direction in the screen is contained in the 2D line with the given equation:    (where lambda is a Real Number)
		//(X,Y) = PlayerScreenPosition + DirOwner * lambda.
		// 
		//To obtain the icon position in the border, we are going to calculate the apropiate lambda value of this line.
		//This value depends on the nearest border of the screen in wich the mentioned above 2D Line will intersect.
		//
		//Choosing the parametric equations, we can obtain lambda as follows depending on the border of the screen we are considering:
		//
		// 
		// lambda = (SizeX-PlayerPosX)/DirOwnerX	(right border)
		// lambda = (0 - PlayerPosX) / DirOwnerX    (left border)
		// lambda = (SizeY-PlayerPosY)/DirOwnerY	(inferior border)
		// lambda = (0-PlayerPosY)/DirOwnerY	(superior border)
		FVector2D FinalPosition;
		float LambdaX = 0.0f;
		float LambdaY = 0.0f;

		if (FMath::Abs(DirOwnerInScreen.X) > 0 && FMath::Abs(DirOwnerInScreen.Y) > 0)
		{
			//In each case, our line will intersect two borders. We difference cases depending on the x and y direction of our line.
			//We are going to calculate this intersections and then compare them to obtain the nearest (lower lambda value).
			if (DirOwnerInScreen.X > 0)
			{
				if (DirOwnerInScreen.Y > 0) 
				{
					//inferior and right borders intersections
					LambdaX = (Size.X - PlayerScreenPosition.X) / DirOwnerInScreen.X;
					LambdaY = (Size.Y - PlayerScreenPosition.Y) / DirOwnerInScreen.Y;
				}
				else
				{
					//Superior and right borders intersections.
					LambdaX = (Size.X - PlayerScreenPosition.X) / DirOwnerInScreen.X;
					LambdaY = (0 - PlayerScreenPosition.Y) / DirOwnerInScreen.Y;
				}
			}
			else
			{
				if (DirOwnerInScreen.Y > 0)
				{
					//Inferior and left borders intersections.
					LambdaX = (0 - PlayerScreenPosition.X) / DirOwnerInScreen.X;
					LambdaY = (Size.Y - PlayerScreenPosition.Y) / DirOwnerInScreen.Y;
				}
				else
				{
					// Superior and left borders intersections.
					LambdaX = (0 - PlayerScreenPosition.X) / DirOwnerInScreen.X;
					LambdaY = (0 - PlayerScreenPosition.Y) / DirOwnerInScreen.Y;
				}
			}
		}
		
		//Select the lower lambda.
		if (LambdaX < LambdaY)
		{
			FinalPosition = PlayerScreenPosition + (LambdaX) * DirOwnerInScreen;
		}
		else
		{
			FinalPosition = PlayerScreenPosition + (LambdaY) * DirOwnerInScreen;
		}


		//Angle:
		DirOwnerInScreen.Normalize();
		float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector2D::DotProduct(FVector2D(1.0f, 0.0f), DirOwnerInScreen)));

		if (DirOwnerInScreen.Y < 0)
		{
			Angle = 360 - Angle;
		}

		if ((DirOwnerInScreen.X > 0 && LambdaX < LambdaY) || (DirOwnerInScreen.Y < 0 && LambdaY < LambdaX))
		{
			Angle -= AngleOffset;
		}
		else
		{
			Angle += AngleOffset;
		}
		
		//Update the hud position and rotation of the icon.
		UpdateIconPositionInHUD(FinalPosition, Angle);

	}
	else
	{
		//If the owner is in the screen, the remove the icon.
		if (bShowingIconInHUD)
		{
			bShowingIconInHUD = false;
			RemoveIconInHUD();
		}
		
	}
}

