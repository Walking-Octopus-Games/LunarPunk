// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AimAssist.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"





// Sets default values for this component's properties
UAimAssist::UAimAssist()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAimAssist::BeginPlay()
{
	Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("Coseno: %f"), AngleTolerance);
	// ...
	
}


// Called every frame
void UAimAssist::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


bool UAimAssist::AimAssist(const FVector& ShotStart, const FVector& DesiredDirection, FRotator& CurrentRotation)
{
    FVector DesiredDirectionNormalized = DesiredDirection.GetSafeNormal();
    FVector LineStart = ShotStart;
    FVector LineEnd = LineStart + MaxDistance * DesiredDirectionNormalized;
    TArray<FHitResult> OutHit;
    TArray<AActor*> ActorsToIgnore;
    TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(TargetChannel));
    EDrawDebugTrace::Type Draw = bDrawDebugSpheres ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
    
    bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
        GetWorld(),
        LineStart,
        LineEnd,
        MaxRadius* AssistAmount,
        ObjectTypes,
        false,
        ActorsToIgnore,
        Draw,
        OutHit,
        true,
        FLinearColor::Red,
        FLinearColor::Green,
        DrawDuration
    );

    //Try to hit an enemy
    if (bHit)
    {
        const AActor* ActorHitted;
        FVector HitDirection;
        float DotProductValue = 0.0;

        for (const FHitResult& Hit : OutHit)
        {
            //Store all the hitted actors whose angle is in the aim assist cone. Do this only once.
            ActorHitted = Hit.GetActor();
            if (!ActorsHitted.Contains(ActorHitted))
            {
                ActorsHitted.Add(ActorHitted);
                HitDirection = (ActorHitted->GetActorLocation() - ShotStart).GetSafeNormal();
                DotProductValue = FVector::DotProduct(DesiredDirectionNormalized, HitDirection);
                if (DotProductValue >= AngleTolerance)
                {
                    ActorsHittedMap.Add(ActorHitted, DotProductValue);
                }
            }
        }

        //Sort the actors by his dot product. The greater the better.
        ActorsHittedMap.ValueSort(
            [this](float A, float B)
            {
                return A > B;
            }
        );

        //Draw actors debug information: sphere at location and dot product result.
        int Order = 0;
        for (auto it = ActorsHittedMap.begin(); it != ActorsHittedMap.end(); ++it)
        {
            if (bDrawDebugSpheres)
            {
#if !UE_BUILD_SHIPPING
                DrawDebugSphere(GetWorld(),it->Key->GetActorLocation(), 50, 3, FColor(180, 0, 0), false, DrawDuration, 0, 2);
                FColor drawColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f).ToFColor(true);
                bool drawShadow = true;
                DrawDebugString(GEngine->GetWorldFromContextObject(this), it->Key->GetActorLocation() + FVector(0.0, 0.0, 10.0), *FString::Printf(TEXT("%s%i%s%f"), TEXT("N:"), Order, TEXT(" Dot: "), it->Value), NULL, drawColor, DrawDuration, drawShadow);
#endif
            }
            ++Order;
        }

    }

    //Select the first actor in the sorted map and write the rotation.
    if (ActorsHittedMap.Num() > 0 && ActorsHittedMap.begin())
    {
        if (bDrawTargetSphere)
        {
#if !UE_BUILD_SHIPPING
            DrawDebugSphere(GetWorld(), ActorsHittedMap.begin()->Key->GetActorLocation(), 50, 3, FColor(0, 255, 0), false, DrawDuration, 0, 2);
#endif
        }

        FRotator NewRot = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), ActorsHittedMap.begin()->Key->GetActorLocation());
        NewRot.Roll = 0.0;
        NewRot.Pitch = 0.0;
        CurrentRotation = NewRot;
        ActorsHitted.Empty();
        ActorsHittedMap.Empty();
        return true;
    }
    ActorsHitted.Empty();
    ActorsHittedMap.Empty();
    return false;
}



#if WITH_EDITOR
void  UAimAssist::PostEditChangeProperty(FPropertyChangedEvent& _event)
{
    Super::PostEditChangeProperty(_event);

    FName ChangedName = _event.Property->GetFName();
 
    FName MaxAngleName = GET_MEMBER_NAME_CHECKED(UAimAssist, MaxAngle);
    if (ChangedName == MaxAngleName) 
    {
        AngleTolerance = FMath::Cos(FMath::DegreesToRadians(AssistAmount * MaxAngle));
    }

}
#endif
