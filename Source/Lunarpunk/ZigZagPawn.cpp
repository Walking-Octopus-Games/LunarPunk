// Fill out your copyright notice in the Description page of Project Settings.


#include "ZigZagPawn.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AZigZagPawn::AZigZagPawn()
{
  // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

  CentralSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
  RootComponent = CentralSphere;

  //ZigZagMovementComponent = CreateDefaultSubobject<UMovementTurretComponent>(TEXT("Turret Movement Component"));

  static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

  LateralSphere = CreateDefaultSubobject<USphereComponent>(TEXT("LateralSphere"));
  LateralSphere->SetupAttachment(CentralSphere);
  LateralSphere->SetHiddenInGame(true);

  CentralSphere->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
  LateralSphere->SetRelativeLocation(FVector(700.f, Distance, 0.f));



  //FloatingMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating Movement Component"));
  //FloatingMovementComponent->MaxSpeed = Speed;

  //ZigZagOwner = Cast<APawn>(GetOwner());
  //UPawnMovementComponent* MovementComponent = ZigZagOwner->GetMovementComponent();


  //ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);









}

// Called when the game starts or when spawned
void AZigZagPawn::BeginPlay()
{
  Super::BeginPlay();

  //ZigZagOwner = Cast<APawn>(GetOwner());
  //AIController = Cast<AAIController>(ZigZagOwner->GetController());
  //AIController->MoveToLocation(DestinationPawn);



  APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

  if (this->GetDistanceTo(PlayerPawn) <= 20.0f)
  {
    this->Destroy();

  }









  //AIController->MoveToLocation(DestinationPosition, 10, false, true, true, true, 0, false);


}

// Called every frame
void AZigZagPawn::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  CurrentTime += DeltaTime;
  if (CurrentTime >= 1.5f)
  {
    Distance = -Distance;
    LateralSphere->SetRelativeLocation(FVector(700.f, Distance, 0.f));
    CurrentTime = 0.0f;

  }






  if (IsValid(GetWorld()))
  {
    if (IsValid(GetWorld()->GetFirstPlayerController()))
    {

      APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
      if (PlayerPawn != nullptr)
      {
        DestinationPawn = PlayerPawn->GetActorLocation();


        StartLocation = GetActorLocation();
        Direction = DestinationPawn - StartLocation;
        Direction = Direction.GetSafeNormal();
        CurrentDistance = 0.0f;

      }
    }
  }




  FVector Location = GetActorLocation();

  Location += Direction * AdvanceSpeed * DeltaTime;
  FRotator NewRotation = (DestinationPawn - StartLocation).Rotation();
  SetActorRotation(NewRotation);

  SetActorLocation(Location);
  CurrentDistance = (Location - StartLocation).Size();





  //if (this->GetDistanceTo(PlayerPawn) <= 200.0f) {
  //	this->Destroy();

  //}

}

void AZigZagPawn::TimeToChangePosition()
{

  if (this != nullptr)
  {
    LateralSphere->SetRelativeLocation(FVector(0.f, -Distance, 0.f));
  }




}



// Called to bind functionality to input
void AZigZagPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
  Super::SetupPlayerInputComponent(PlayerInputComponent);

}

