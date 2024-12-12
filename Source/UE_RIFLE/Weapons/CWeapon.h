#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/WeaponData.h"
#include "Components/BoxComponent.h"
#include "CWeapon.generated.h"

USTRUCT()
struct FWeaponAimData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    float TargetArmLength;
    UPROPERTY(EditAnywhere)
    FVector SocketOffset;
    UPROPERTY(EditAnywhere)
    float FieldOfView;

public:
    void SetDataByNoneCurve(class ACharacter* InOwner);
    void SetData(class ACharacter* InOwner);

};

UCLASS(abstract)
class UE_RIFLE_API ACWeapon : public AActor
{
    GENERATED_BODY()

public:
    ACWeapon();
protected:
    virtual void BeginPlay() override;
public:
    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY(VisibleAnywhere)
    class USceneComponent* Root;
public:
    UPROPERTY(VisibleAnywhere)
    class USkeletalMeshComponent* Mesh;
protected:
    UPROPERTY(VisibleAnywhere)
    class UTimelineComponent* Timeline;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Equip")
    FName HolsterSocketName;
    UPROPERTY(EditDefaultsOnly, Category = "Equip")
    class UAnimMontage* EquipMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Equip")
    float EquipMontage_PlayRate = 1.0f;
    UPROPERTY(EditDefaultsOnly, Category = "Equip")
    FName RightHandSocketName;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Hit")
    float HitDistance = 3000.0f;
    UPROPERTY(EditDefaultsOnly, Category = "Hit")
    class UMaterialInstanceConstant* HitDecal;
    UPROPERTY(EditDefaultsOnly, Category = "Hit")
    class UParticleSystem* HitParticle;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Aim")
    FWeaponAimData AimData;
    UPROPERTY(EditDefaultsOnly, Category = "Aim")
    FWeaponAimData BaseData;
    UPROPERTY(EditDefaultsOnly, Category = "Aim")
    class UCurveFloat* AimCurve;
    UPROPERTY(EditDefaultsOnly, Category = "Aim")
    float AimingSpeed = 200;
    UPROPERTY(EditDefaultsOnly, Category = "Aim")
    FVector LeftHandLocation;

    UPROPERTY(EditDefaultsOnly, Category = "Fire")
    class UParticleSystem* FlashParticle;
    UPROPERTY(EditDefaultsOnly, Category = "Fire")
    class UParticleSystem* EjectParticle;
    UPROPERTY(EditDefaultsOnly, Category = "Fire")
    class USoundWave* FireSound;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Fire")
    TSubclassOf<class UCameraShakeBase> CameraShakeClass;
    UPROPERTY(EditDefaultsOnly, Category = "Fire")
    TSubclassOf<class ACBullet> BulletClass;
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UCUserWidget_CrossHair> CrossHairClass;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
    UDataTable* WeaponDataTable;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
    FName WeaponName;

public:
    void ToggleAutoFire();
    void CreateCrossHair();


    UPROPERTY(EditDefaultsOnly, Category = "MagaZine")
    class UAnimMontage* ReloadMontage;
    UPROPERTY(EditDefaultsOnly, Category = "MagaZine")
    float ReloadMontage_PlayRate;
    UPROPERTY(EditDefaultsOnly, Category = "MagaZine")
    FName MagazineBoneName;
    UPROPERTY(EditDefaultsOnly, Category = "MagaZine")
    FName MagazineSocketName;
    UPROPERTY(EditDefaultsOnly, Category = "MagaZine")
    TSubclassOf<class ACMagaZine> MagazineClass;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Arms")
    FTransform ArmsMeshTransform;
    UPROPERTY(EditDefaultsOnly, Category = "Arms")
    FTransform ArmsLeftHandTransform;

public:
    bool CanEquip();
    void Equip();
    virtual void Begin_Equip();
    virtual void End_Equip();
public:
    bool CanUnEquip();
    virtual void UnEquip();

public:
    bool CanFire();
    void Begin_Fire();
    void End_Fire();
public:
    UFUNCTION()
    void OnFiring();

    FTimerHandle AutoFireHandle;

public:
    bool CanAim();
    virtual void Begin_Aim();
    virtual void End_Aim();

public:
    bool CanReload();
    void Reload();
    void Eject_Magazine();
    void Spawn_Magazine();
    void Load_Magazine();
    void End_Reload();
    uint8 GetCurrMagazineCount() const;

protected:
    class ACPlayer* Owner;
    class UCUserWidget_CrossHair* CrossHair;
    class ACMagaZine* MagaZine;
    class UCameraComponent* Camera;
private:
    bool bEquipping = false;
    bool bInAim = false;
    bool bFiring = false;
    bool bReload = false;
    bool bAutoFire = false;
public:
    float CurrSpreadRadius = 0.0f;
    float LastAddSpreadTime = 0.0f;
public:
    UPROPERTY(ReplicatedUsing = OnRepAmmoChanged)
    int32 CurrMagazineCount;

    UPROPERTY(ReplicatedUsing = OnRepAmmoChanged)
    int32 CurrentAmmo;

private:
    UFUNCTION()
    void OnRepAmmoChanged();


    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    FWeaponData WeaponData;

public:
    EWeaponType GetWeaponType() const;

private:
    // 무기 유형을 저장할 변수
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    EWeaponType WeaponType;

    UPROPERTY(VisibleAnywhere, Category = "Collision")
    UBoxComponent* PickupCollision;  // 박스 콜리전 컴포넌트

    UFUNCTION()
    void OnPickupOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:
    void ActivateCollision();
    void DeactivateCollision();

public:
    FORCEINLINE bool IsInAim() { return bInAim; }
    FORCEINLINE bool IsFiring() { return bFiring; }
    FORCEINLINE bool IsAutoFire() { return bAutoFire; }
    FORCEINLINE FVector GetLeftHandLocation() { return LeftHandLocation; }
    FORCEINLINE uint8 GetCurrMagazineCount() { return CurrMagazineCount; }
    FORCEINLINE uint8 GetMaxMagazineCount() { return WeaponData.MaxMagazineCount; }
    FORCEINLINE FTransform GetArmsLeftHandTransform() { return ArmsLeftHandTransform; }
    const FWeaponData& GetWeaponData() const { return WeaponData; }

    // 추가된 메서드
    void SetCurrMagazineCount(uint8 NewCount) { CurrMagazineCount = NewCount; }
    void SetCurrentAmmo(int32 NewCount) { CurrentAmmo = NewCount; }
    int32 GetTotalRemainingAmmo() const { return CurrentAmmo; }
public:
    void ServerPickupWeapon_Implementation(ACharacter* PlayerCharacter);
    bool ServerPickupWeapon_Validate(ACharacter* PlayerCharacter);

    //Server
public:
    UFUNCTION(NetMulticast, Reliable)
    void MulticastFireEffects();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_Eject_Magazine();
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_Spawn_Magazine();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_Load_Magazine();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_End_Reload();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_DestroyWeapon();

public:
    // 총알 정보를 저장하는 함수
    void SaveAmmo(int32 MagazineCount, int32 AmmoCount);

    // 총알 정보를 불러오는 함수
    void LoadAmmo(int32& OutMagazineCount, int32& OutAmmoCount) const;
};