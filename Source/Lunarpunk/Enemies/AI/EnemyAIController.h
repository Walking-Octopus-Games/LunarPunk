// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DetourCrowdAIController.h"
#include "NavigationSystem.h"
#include "EnemyAIController.generated.h"


class UCrowdFollowingComponent;
class ALunarPunkGameMode;
class UEnemyTargetPoints;
class ATurret;
/**
 *
 */
UCLASS()
class LUNARPUNK_API AEnemyAIController : public AAIController
{
  GENERATED_BODY()

public:
  AEnemyAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  //AEnemyAIController(const FObjectInitializer& ObjectInitializer);

  UPROPERTY(EditAnywhere, Category = "Enemy | AI")
    bool bGoOnlyToPlayer = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | AI")
    UCrowdFollowingComponent* CrowdFollowingComponent;

  UPROPERTY()
    ALunarPunkGameMode* LPGameMode;

  UPROPERTY()
    UNavigationSystemV1* NavSystem;

  UPROPERTY()
    FVector TargetAreaPosition;
  UPROPERTY()
    UBlackboardComponent* PBlackboardComponent;

  UPROPERTY()
    UEnemyTargetPoints* ETP;
  UPROPERTY(VisibleAnywhere)
    FVector RelativePosition;


  //UPROPERTY(EditAnywhere, Category = "Enemy | AI")
  //  UCrowdFollowingComponent* CrowdFollowingComponent;

  UFUNCTION(BlueprintCallable, Category = "Enemy | AI")
    void UpdateNextTargetPoint();

  UFUNCTION()
    void RemovePlayerAsTarget();

  UFUNCTION()
    void SetTurretBlackboardTarget(AActor* Actor);
  UFUNCTION()
    void SetValidTargets();

  UFUNCTION(BlueprintCallable)
    void GetRandomPointInTurretArea(AActor* TurretTarget);

  UFUNCTION(BlueprintCallable)
    bool CheckIsTheActorValid(AActor* Actor);

  UFUNCTION(BlueprintCallable)
    bool IsETPAssigned();

  UFUNCTION(BlueprintCallable)
    void AssignETP(UEnemyTargetPoints* EtpToEnemy);

  UFUNCTION()
    void DeleteBBValues();

private:


  void UpdateActorTurrets(TArray<AActor*>& ActorTurrets) const;
  void UpdateRelativeAreaPosition(AActor* Turret);
  //void SetBlackboardTarget(AActor* Target, FVector& TLocation) const;
  bool IsAnyTurretAvailable(bool& bValue1);
  void SetPlayerAsTarget();
  void SetTurretAsTarget(AActor*& Target, bool& bTurretFound);
  ATurret* FindTurretWithFreeETP(TArray<ATurret*> Turrets);
  void ShuffleArray(TArray<ATurret*>& myArray);
protected:
  virtual void BeginPlay() override;

};
