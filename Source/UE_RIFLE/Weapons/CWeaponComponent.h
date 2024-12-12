#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CWeaponComponent.generated.h"

// BP에서 열거형을 C++에서 기술하고 그 타입을 BP에서도 보이게 하기 위해서
/*ForLoop에서 끝부분 체크*/
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    AR4, AK47, Gun, Sniper, Max
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponTypeChanged, EWeaponType, InPrevType, EWeaponType, InNewType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponAim_Arms_Begin, class ACWeapon*, InThisWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeaponAim_Arms_End);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE_RIFLE_API UCWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCWeaponComponent();
protected:
    virtual void BeginPlay() override;
public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    UPROPERTY(EditAnywhere, Category = "Settings")
    TArray<TSubclassOf<class ACWeapon>> WeaponClasses;
    UPROPERTY(EditAnywhere, Category = "Settings")
    TSubclassOf<class UCUserWidget_HUD> HUDClass;

public:
    void SetUnarmedMode();
    void CreateHUD();

public:
    UFUNCTION(BlueprintCallable)
    void SetAR4Mode();

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SetAR4Mode();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetAR4Mode();

    UFUNCTION(BlueprintCallable)
    void SetAK47Mode();

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SetAK47Mode();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetAK47Mode();

    // UFUNCTION(BlueprintCallable)
    // void SetPistolMode();
    //
    // UFUNCTION(Server, Reliable, WithValidation)
    // void Server_SetPistolMode();
    //
    // UFUNCTION(NetMulticast, Reliable)
    // void Multicast_SetPistolMode();

    UFUNCTION(BlueprintCallable)
    void SetGunMode();

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SetGunMode();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetGunMode();

    UFUNCTION(BlueprintCallable)
    void SetSniperMode();

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SetSniperMode();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetSniperMode();
public:
    UFUNCTION(BlueprintCallable)
    void Begin_Equip();

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_Begin_Equip();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_Begin_Equip();

    UFUNCTION(BlueprintCallable)
    void End_Equip();

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_End_Equip();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_End_Equip();

public:
    UFUNCTION(BlueprintCallable)
    void Begin_Fire();

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_Begin_Fire();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_Begin_Fire();

    UFUNCTION(BlueprintCallable)
    void End_Fire();

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_End_Fire();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_End_Fire();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastFireEffects(class ACWeapon* CurrentWeapon);

public:

    void Begin_Aim();
    void End_Aim();
    void ToggleAutoFire();

    // 리플리케이트를 위한 함수 선언
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_Begin_Aim();

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_End_Aim();

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_ToggleAutoFire();

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    UPROPERTY(ReplicatedUsing = OnRep_IsAiming)
    bool bIsAiming;
    UPROPERTY(ReplicatedUsing = OnRep_IsAutoFireEnabled)
    bool bIsAutoFireEnabled;
    UPROPERTY(ReplicatedUsing = OnRep_AmmoState)
    int32 CurrMagazineCount;

    UPROPERTY(ReplicatedUsing = OnRep_AmmoState)
    int32 CurrentAmmo;

    UFUNCTION()
    void OnRep_IsAiming();
    UFUNCTION()
    void OnRep_IsAutoFireEnabled();
    UFUNCTION()
    void OnRep_AmmoState();

public:
    bool IsInAim();
    bool IsFiring();
    FVector GetLeftHandLocation();
    FTransform GetArmsLeftHandTransform();

public:
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_Reload();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_Reload();

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_Eject_Magazine();

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_Spawn_Magazine();

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_Load_Magazine();

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_End_Reload();

    UFUNCTION()
    void OnAim_Arms_Begin(class ACWeapon* InThisWeapon);
    UFUNCTION()
    void OnAim_Arms_End();

public:
    void Reload();
    void Eject_Magazine();
    void Spawn_Magazine();
    void Load_Magazine();
    void End_Reload();
    void UpdateHUD();
    void SetCurrentWeaponAmmo(int32 MagazineCount, int32 AmmoCount);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetCurrentWeaponAmmo(int32 MagazineCount, int32 AmmoCount);

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SetCurrentWeaponAmmo(int32 MagazineCount, int32 AmmoCount);
    void Server_SetCurrentWeaponAmmo_Implementation(int32 MagazineCount, int32 AmmoCount);
    bool Server_SetCurrentWeaponAmmo_Validate(int32 MagazineCount, int32 AmmoCount);

public:
    void SetMode(EWeaponType InType);
    void ChangeType(EWeaponType InType);

public:
    class ACWeapon* GetCurrentWeapon();
    void UnEquipCurrentWeapon();
    void AddDroppedWeapon(EWeaponType WeaponType);
    bool IsWeaponDropped(EWeaponType WeaponType) const; // 드롭된 무기 여부 확인 함수
    void RemoveDroppedWeapon(EWeaponType WeaponType);
    bool CanPickupWeapon(EWeaponType WeaponType);
    void SetDefaultWeapon(EWeaponType DefaultType);

private:
    TArray<EWeaponType> DroppedWeapons;

private:
    EWeaponType Type = EWeaponType::Max;

private:
    class ACPlayer* Owner;
    TArray<class ACWeapon*> Weapons;

    bool bIsZoomedIn;  // 줌인 상태 확인 변수

public:
    FWeaponTypeChanged OnWeaponTypeChanged;
    FWeaponAim_Arms_Begin OnWeaponAim_Arms_Begin;
    FWeaponAim_Arms_End OnWeaponAim_Arms_End;

private:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> HUDWidgetClass;

public:
    class UCUserWidget_HUD* HUDWidget;
public:
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool IsUnarmedMode() const;
    FORCEINLINE bool IsAR4Mode() { return Type == EWeaponType::AR4; }
    FORCEINLINE bool IsAK47Mode() { return Type == EWeaponType::AK47; }
    //FORCEINLINE bool IsPistolMode() { return Type == EWeaponType::Pistol; }
    FORCEINLINE bool IsGunMode() { return Type == EWeaponType::Gun; }
    FORCEINLINE bool IsSniperMode() { return Type == EWeaponType::Sniper; }
};