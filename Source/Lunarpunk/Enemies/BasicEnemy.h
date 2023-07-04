// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IEnemy.h"
#include "AI/AvoidEnemies.h"
#include "Pool/IsPoolable.h"
#include "BasicEnemy.generated.h"



class UHealthComponent;
class UDamageComponent;
class UPoisonComponent;
class USphereComponent;
class APool;
class UDropRewardsComponent;
class UWalkToDestinationComponent;
class UCharacterMovementcomponent;
class UAudioEnemyComponent;
class ALunarPunkGameMode;
class AZigZagPawn;
class APlayableCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyDamaged, int32, Damage);

UCLASS()
class LUNARPUNK_API ABasicEnemy : public ACharacter, public IEnemy, public IIsPoolable
{
  GENERATED_BODY()

public:
  // Sets default values for this character's properties
  ABasicEnemy();
protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

public:
  UFUNCTION()
    virtual void Initialize(APool* Pool);

  // Called every frame
  virtual void Tick(float DeltaTime) override;

  // Called to bind functionality to input
  virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

  UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
      AActor* OtherActor,
      UPrimitiveComponent* OtherComp,
      int32 OtherBodyIndex,
      bool bFromSweep,
      const FHitResult& SweepResult);
  //UFUNCTION()
  //  void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
  //    AActor* OtherActor,
  //    UPrimitiveComponent* OtherComp,
  //    int32 OtherBodyIndex
  //  );

  void ChangeMaterial(int index);

  UFUNCTION(BlueprintCallable)
    virtual  UHealthComponent* GetHealthComponent()  override;

  UFUNCTION(BlueprintCallable)
    virtual  UDamageComponent* GetDamageComponent()  override;
  UFUNCTION(BlueprintCallable)

    virtual   APool* GetMyPool()  override;
  UFUNCTION(BlueprintCallable)
    virtual  UDropRewardsComponent* GetRewardsComponent()  override;

  UFUNCTION(BlueprintCallable)
    virtual UWalkToDestinationComponent* GetWalkToDestinationComponent()  override;


  UPROPERTY(EditAnywhere)
    TArray<UMaterial*> Materials;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UCapsuleComponent* DamageRangeCollision;

  UPROPERTY(EditAnywhere)
    USphereComponent* SphereToFollow;


  UPROPERTY(EditAnywhere)
    AZigZagPawn* ZigZagPawnToFollow;

  UPROPERTY(VisibleAnywhere)
    ALunarPunkGameMode* LunarPunkGameMode;

  UPROPERTY(VisibleAnywhere)
    APlayableCharacter* PlayerPawn;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAvoidEnemies* AvoidEnemiesComponent;
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USphereComponent* EnemyAvoidSphereCollision;

  UFUNCTION(BlueprintCallable)
    void AttachSphere(USphereComponent* sphere);

  UFUNCTION(BlueprintCallable)
    void AttachZigZagPawn(AZigZagPawn* zigzagpawn);

  UFUNCTION(BlueprintCallable)
    USphereComponent* GetSphereToFollow();


  UPROPERTY()
    FVector DestinationPawn;

  UPROPERTY(EditAnywhere)
    UCapsuleComponent* CapsuleComponentCollision;

  UPROPERTY(VisibleAnywhere)
    UCharacterMovementComponent* MovementComponent;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UAudioEnemyComponent* AudioEnemyComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UHealthComponent* HealthComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UDamageComponent* DamageComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UPoisonComponent* PoisonComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UDropRewardsComponent* RewardsComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UWalkToDestinationComponent* WalkToDestinationComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USkeletalMeshComponent* MeshComponent;

  UPROPERTY(EditAnywhere)
    APool* MyPool;

  UPROPERTY()
    class AAIController* AICharacterController;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Sound")
    TArray<USoundBase*> HitTurretsSounds;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Sound")
    TArray<USoundBase*> HitPlayerSounds;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Sound")
    float VolumeSound = 0.2f;

  /* UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Sound")
       UAudioComponent* AudioComponent;*/

       //If true, the enemy got active at the begin (Only for debug purposes).
  UPROPERTY(EditAnywhere, Category = "Debug And Tutorial")
    bool bSetActiveAtBegin = false;

  //If true, the enemy will do nothing.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug And Tutorial")
    bool bDeathBrainMode = false;

  //If true, the enemy will be destroyed instead of returned to the pool.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug And Tutorial")
    bool bNonReturnable = false;

  ////Delegate to add to the HUD the damage applied.
  UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
    FEnemyDamaged EnemyDamaged;

  //Functions to manage the enemy in the pool.
  UFUNCTION(BlueprintCallable)
    virtual void ReturnToPool() override;

  UFUNCTION(BlueprintCallable)
    virtual void Deactivate() override;

  UFUNCTION(BlueprintCallable)
    virtual void Reactivate() override;

  UFUNCTION(BlueprintCallable)
    virtual void DeactivateForDeathAnim();

  virtual void SetMaxHealth(float PercentageVariationMaxHealth) override;

  virtual void SetMaxDamage(float PercentageVariationMaxDamage) override;

  virtual void SetMovementSpeed(float PercentageVariationSpeed) override;

  virtual void SetPlatesVariation(int32 PlatesVariation) override;

  virtual void SetUpgradesVariation(float UpgradesVariation) override;

  UFUNCTION(BlueprintCallable)
    virtual void SetDeathBrainMode();

  UFUNCTION(BlueprintCallable)
    void TryToHit();

  UFUNCTION(BlueprintCallable)
    void EndTryToHit();

  UFUNCTION(BlueprintImplementableEvent)
    void DrawDamageWidget(float DamageReceived) override;

  UFUNCTION()
    void SetShowEnemyDamagePoints(bool bShowDamagePoints);

  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void SpawnImpactEffect(FVector HitLocation, FVector OtherActorLocation);

private:

  void PlayHitTurretSound();

  void PlayHitPlayerSound();

  UPROPERTY(VisibleAnywhere)
    bool bShowEnemyDamagePoints;

  float InitialSpeed = 0.0f;

};
