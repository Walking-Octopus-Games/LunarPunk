// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "TargetForEnemiesHandler.h"
#include "RewardsAndUpgrades/UpgradesData.h"

#include "Turret.generated.h"

#define STENCIL_TURRET 254 //Visible through walls
#define STENCIL_TURRET_WITH_OUTLINE 255 //Adds outline

class ATurretWaypoint;
class UHealthComponent;
class USphereComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class ALunarPunkGameMode;
class AUpgrade;
//class UFMODAudioComponent;
class UAudioTurretComponent;

class UEnemyTargetPoints;
//class UTargetForEnemiesHandler;

UENUM()
enum class ETurretState : uint8
{
  Set //Turret has health and is attacking or healing other turrets
  , Repairing //Turret is being repaired
  , Deactivated //Turret has been deactivated
  , PickedUp //Turret is not set
  , Moving // Turret is moving forward
  , Conquest // Turret place in conquest zone
};

USTRUCT(BlueprintType)
struct LUNARPUNK_API FUpgradesInfo
{

  GENERATED_USTRUCT_BODY()

public:
  FUpgradesInfo()
  {
  };

  UPROPERTY(BlueprintReadOnly, EditAnyWhere)
    int Range;
  UPROPERTY(BlueprintReadOnly, EditAnyWhere)
    int UpgradedRange;
  UPROPERTY(BlueprintReadOnly, EditAnyWhere)
    int Cadence;
  UPROPERTY(BlueprintReadOnly, EditAnyWhere)
    int UpgradedCadence;
  UPROPERTY(BlueprintReadOnly, EditAnyWhere)
    int Health;
  UPROPERTY(BlueprintReadOnly, EditAnyWhere)
    int UpgradedHealth;
  UPROPERTY(BlueprintReadOnly, EditAnyWhere)
    int Power;
  UPROPERTY(BlueprintReadOnly, EditAnyWhere)
    int UpgradedPower;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShoot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurretPicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurretLeave);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRepairing);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopRepairing);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurretDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurretReactivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpgradeAdded, EUpgradeType, UpgradeType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateUpgradesInfo, FUpgradesInfo, UpgradesInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTurretMakeDamage, ETurretType, TurretType, float, damage);


UCLASS(Abstract)
class LUNARPUNK_API ATurret : public APawn
{
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  ATurret();
  void InitTurretState();

  UFUNCTION()
    void Initialize();

  UPROPERTY()
    ETurretType TypeOfTurret;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
    ETurretState State = ETurretState::Set;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
    ETurretState PreviousState = ETurretState::Set;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Turret|Health")
    UHealthComponent* HealthComponent = nullptr;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Turret|Health")
    float MaxTimeLeftToRepair = 3.0f;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Turret|Health")
    float MaxTimeLeftToRevive = 6.0f;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Turret|Health")
    float RecoverRate = 1.5f;

  //If true, the cost of repair a turret will be fixed. If false, it will depend on the max health points of the turrets.
  UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Turret|Cost")
    bool bFixedRepairCost = true;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Turret|Cost")
    float FixedRepairCost = 25;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Turret|Cost")
    float RepairCostPerHealthPoint = 0.2;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Turret|Shield")
    float ShieldRadius = 150.0f;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Upgrades")
    FUpgradesInfo UpgradesInfo;

  //UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Turret|Sound")
  //  USoundBase* RepairingSound;

  //UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Turret|Sound")
  //  USoundBase* RepairedSound;

  //UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Turret|Sound")
  //  USoundBase* ApplyUpgradeSound;

  //UPROPERTY(BlueprintReadWrite, EditAnywhere)
  //  UAudioComponent* AudioComponent;
  ////
  //UPROPERTY(BlueprintReadWrite, EditAnywhere)
  //  UFMODAudioComponent* FMODAudioComponent;



  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Turret|Upgrades")
    TMap<EUpgradeType, int> AppliedUpgrades;


  UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
    FShoot ShootEvent;

  UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
    FRepairing Repairing;

  UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
    FStopRepairing StopRepairing;

  UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
    FTurretDeath TurretDeath;

  UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
    FTurretReactivated TurretReactivated;

  UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
    FUpgradeAdded UpgradeAdded;


  UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable)
    FUpdateUpgradesInfo UpdateUpgradesInfoEvent;

  UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
    FTurretPicked TurretPickedUp;

  UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
    FTurretLeave TurretLeft;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UMovementTurretComponent* TurretMovementComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UFloatingPawnMovement* FloatingMovementComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UPositionHelperIcon* PositionHelperIconComponent;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UAudioTurretComponent* AudioTurretComponent;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneComponent* MyRootComponent;

  UPROPERTY(EditAnywhere, BlueprintAssignable, BlueprintCallable)
    FTurretMakeDamage TurretMakeDamage;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTargetForEnemiesHandler* TargetForEnemiesHandlerComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USphereComponent* AreaForEnemies;
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<UEnemyTargetPoints*> EnemyTargetPoints;
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UEnemyTargetPoints* TargetPoint4;
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UEnemyTargetPoints* TargetPoint3;
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UEnemyTargetPoints* TargetPoint2;
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UEnemyTargetPoints* TargetPoint1;
  // Called every frame
  virtual void Tick(float DeltaTime) override;

  UFUNCTION()
    virtual void StartTurret();

  UFUNCTION()
    void RestartTurret();

  UFUNCTION()
    virtual void StopTurret();

  UFUNCTION()
    virtual void TurnOnOutline();

  UFUNCTION()
    virtual void TurnOffOutline();


  /*
  Description: places the turret on the ground
  */
  UFUNCTION(BlueprintCallable, Category = "Turret")
    virtual void PlaceTurret();

  /*
  Description: picks up the turret from the ground
  Output: bool, indicates if the turret can be picked up
  */
  UFUNCTION(BlueprintCallable, Category = "Turret")
    virtual bool PickUpTurret();

  /*
  Description: called when someone tries to repair the turret
  */
  UFUNCTION(BlueprintCallable, Category = "Turret")
    bool StartRepairing();

  /*
  Description:
  */
  UFUNCTION(BlueprintCallable, Category = "Turret")
    void InterruptRepair();

  /*
  Description:
  */
  UFUNCTION(BlueprintCallable, Category = "Turret")
    int32 CostRepair();

  /*
  Description:
  */
  UFUNCTION(BlueprintCallable, Category = "Turret")
    void DeactivateTurret();


  /*
  Description:
  */
  virtual void UpgradeTurret(FUpgradesDataStruct* Upgrade, int Quantity);

  UFUNCTION()
    void ActivateShield();

  UFUNCTION()
    void DeactivateShield();

  UFUNCTION()
    void StartMovementEffect();

  UFUNCTION()
    void StopMovementEffect();

  /*
  Description:
  */
  UFUNCTION(BlueprintCallable, Category = "Turret")
    ETurretState GetState();


  virtual float GetRange() const
  {
    return 0.0f;
  }


  /*
  Description:
  */
  UFUNCTION()
    UNiagaraComponent* GetDeactivatedEffectComponent();

  /*
  Description:
  */
  UFUNCTION()
    void SetDeactivatedEffectComponent(UNiagaraComponent* NewDeactivatedEffectComponent);

  UFUNCTION(BlueprintImplementableEvent)
    void DrawDamageWidget(int Damage);

  UFUNCTION(BlueprintImplementableEvent)
    void DrawInfoWidget();

  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void RefreshInfoWidget(int32 PlatesPlayer);

  UFUNCTION(BlueprintImplementableEvent)
    void RemoveInfoWidget();

  UFUNCTION()
    void SetTargetWaypoint(ATurretWaypoint* NextWaypoint);


  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

  UFUNCTION()
    void DrawWidget(float DamageReceived);

  UFUNCTION()
    void SetShowDamagePoints(bool _bShowDamagePoints);

  UFUNCTION(BlueprintCallable, category = "Turret")
    virtual void InitializeUpgradeInfo();

  UFUNCTION()
    virtual void UpdateUpgradesInfo(FUpgradesDataStruct Upgrade, int Quantity);


  UPROPERTY(BlueprintReadWrite, EditAnywhere)
    class ATurretWaypoint* TargetWaypoint;

protected:

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere, category = "Turret|Health")
    float CurrentTimeLeftToHeal = 0.0f;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Turret|Effects")
    UNiagaraSystem* RepairEffectComponent;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Turret|Effects")
    UNiagaraSystem* DeactivatedEffect;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UNiagaraComponent* DeactivatedEffectComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UNiagaraComponent* FloatingEffectComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UNiagaraComponent* UpgradeEffectComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USphereComponent* SphereShootRange;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMeshComponent* MeshShield;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shield")
    float TimeShield = 5.0f;

  FTimerHandle ShieldTimerHandle;

  UFUNCTION(BlueprintCallable, category = "Turret")
    void SetTurretType(ETurretType Type);

  UPROPERTY()
    bool bShowDamagePoints;

  ALunarPunkGameMode* GameMode;


};
