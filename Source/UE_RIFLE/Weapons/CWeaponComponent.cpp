#include "Weapons/CWeaponComponent.h"
#include "Global.h"
#include "CWeapon.h"
#include "Characters/CPlayer.h"
#include "Characters/CPlayerController.h"
#include "Widgets/CUserWidget_HUD.h"
#include "Net/UnrealNetwork.h"

UCWeaponComponent::UCWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    CHelpers::GetClass<UCUserWidget_HUD>(&HUDClass, "WidgetBlueprint'/Game/Widgets/WB_HUD.WB_HUD_C'");

    bIsAiming = false;
    bIsAutoFireEnabled = false;
    SetIsReplicatedByDefault(true);
}

void UCWeaponComponent::BeginPlay()
{
    Super::BeginPlay();

    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UCWeaponComponent::CreateHUD, 0.1f, false);

    Owner = Cast<ACPlayer>(GetOwner());
    CheckNull(Owner);

    FActorSpawnParameters params;
    params.Owner = Owner;
    params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    for (TSubclassOf<ACWeapon> weaponClass : WeaponClasses)
    {
        if (!!weaponClass)
        {
            ACWeapon* weapon = Owner->GetWorld()->SpawnActor<ACWeapon>(weaponClass, params);
            Weapons.Add(weapon);
        }
        else
        {
            Weapons.Add(nullptr);
        }
    }

    OnWeaponAim_Arms_Begin.AddDynamic(this, &UCWeaponComponent::OnAim_Arms_Begin);
    OnWeaponAim_Arms_End.AddDynamic(this, &UCWeaponComponent::OnAim_Arms_End);
}

bool UCWeaponComponent::IsUnarmedMode() const
{
    return Type == EWeaponType::Max;
}

void UCWeaponComponent::CreateHUD()
{
    if (Owner && HUDClass)
    {
        APlayerController* PlayerController = Cast<APlayerController>(Owner->GetController());

        // 서버가 Dedicated Server가 아닌 경우 (서버에서 플레이어로 위젯을 보려면 필요)
        if (PlayerController && (PlayerController->IsLocalController() || GetWorld()->GetNetMode() == NM_ListenServer))
        {
            HUDWidget = CreateWidget<UCUserWidget_HUD>(PlayerController, HUDClass);
            if (HUDWidget)
            {
                HUDWidget->AddToViewport();
                HUDWidget->SetVisibility(ESlateVisibility::Hidden);
            }
        }
    }
}

void UCWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UCWeaponComponent, bIsAiming);
    DOREPLIFETIME(UCWeaponComponent, bIsAutoFireEnabled);
    DOREPLIFETIME(UCWeaponComponent, CurrMagazineCount);
    DOREPLIFETIME(UCWeaponComponent, CurrentAmmo);
}

void UCWeaponComponent::SetAR4Mode()
{
    if (Owner && Owner->HasAuthority())
    {
        Multicast_SetAR4Mode();
    }
    else
    {
        Server_SetAR4Mode();
    }
}

void UCWeaponComponent::Server_SetAR4Mode_Implementation()
{
    Multicast_SetAR4Mode();
}

bool UCWeaponComponent::Server_SetAR4Mode_Validate()
{
    return true;
}

void UCWeaponComponent::Multicast_SetAR4Mode_Implementation()
{
    SetMode(EWeaponType::AR4);
}

void UCWeaponComponent::SetAK47Mode()
{
    if (Owner && Owner->HasAuthority())
    {
        Multicast_SetAK47Mode();
    }
    else
    {
        Server_SetAK47Mode();
    }
}

void UCWeaponComponent::Server_SetAK47Mode_Implementation()
{
    Multicast_SetAK47Mode();
}

bool UCWeaponComponent::Server_SetAK47Mode_Validate()
{
    return true;
}

void UCWeaponComponent::Multicast_SetAK47Mode_Implementation()
{
    SetMode(EWeaponType::AK47);
}

void UCWeaponComponent::SetGunMode()
{
    if (Owner && Owner->HasAuthority())
    {
        Multicast_SetGunMode();
    }
    else
    {
        Server_SetGunMode();
    }
}

void UCWeaponComponent::Server_SetGunMode_Implementation()
{
    Multicast_SetGunMode();
}

bool UCWeaponComponent::Server_SetGunMode_Validate()
{
    return true;
}

void UCWeaponComponent::Multicast_SetGunMode_Implementation()
{
    SetMode(EWeaponType::Gun);
}

void UCWeaponComponent::SetSniperMode()
{
    if (Owner && Owner->HasAuthority())
    {
        Multicast_SetSniperMode();
    }
    else
    {
        Server_SetSniperMode();
    }
}

void UCWeaponComponent::Server_SetSniperMode_Implementation()
{
    Multicast_SetSniperMode();
}

bool UCWeaponComponent::Server_SetSniperMode_Validate()
{
    return true;
}

void UCWeaponComponent::Multicast_SetSniperMode_Implementation()
{
    SetMode(EWeaponType::Sniper);
}

void UCWeaponComponent::Begin_Equip()
{
    if (Owner && Owner->HasAuthority())
    {
        Multicast_Begin_Equip();
    }
    else
    {
        Server_Begin_Equip();
    }
}

void UCWeaponComponent::Server_Begin_Equip_Implementation()
{
    Multicast_Begin_Equip();
}

bool UCWeaponComponent::Server_Begin_Equip_Validate()
{
    return true;
}

void UCWeaponComponent::Multicast_Begin_Equip_Implementation()
{
    CheckNull(GetCurrentWeapon());
    GetCurrentWeapon()->Begin_Equip();
}

void UCWeaponComponent::End_Equip()
{
    if (Owner && Owner->HasAuthority())
    {
        Multicast_End_Equip();
    }
    else
    {
        Server_End_Equip();
    }
}

void UCWeaponComponent::Server_End_Equip_Implementation()
{
    Multicast_End_Equip();
}

bool UCWeaponComponent::Server_End_Equip_Validate()
{
    return true;
}

void UCWeaponComponent::Multicast_End_Equip_Implementation()
{
    CheckNull(GetCurrentWeapon());
    GetCurrentWeapon()->End_Equip();
}

void UCWeaponComponent::Begin_Fire()
{
    if (Owner && Owner->HasAuthority())  // 서버에서만 발사 가능 여부를 판단
    {
        if (GetCurrentWeapon() && GetCurrentWeapon()->CanFire())  // 서버에서 발사 가능 여부 확인
        {
            GetCurrentWeapon()->Begin_Fire();  // 발사 시작
            //Multicast_Begin_Fire();
        }
    }
    else  // 클라이언트가 발사를 시도하면 서버로 발사 요청을 보냄
    {
        Server_Begin_Fire();
    }
}

void UCWeaponComponent::Server_Begin_Fire_Implementation()
{
    if (GetCurrentWeapon() && GetCurrentWeapon()->CanFire())
    {
        Multicast_Begin_Fire();  // 서버에서 발사 로직 동기화
    }
    //Multicast_Begin_Fire();
}

bool UCWeaponComponent::Server_Begin_Fire_Validate()
{
    return true;
}

void UCWeaponComponent::Multicast_Begin_Fire_Implementation()
{
    CheckNull(GetCurrentWeapon());
    if (!GetCurrentWeapon()->CanFire())
        return;
    GetCurrentWeapon()->Begin_Fire();
    // GetCurrentWeapon()->MulticastFireEffects();
}

void UCWeaponComponent::End_Fire()
{
    if (Owner && Owner->HasAuthority())
    {
        Multicast_End_Fire();
    }
    else
    {
        Server_End_Fire();
    }
}

void UCWeaponComponent::Server_End_Fire_Implementation()
{
    Multicast_End_Fire();
}

bool UCWeaponComponent::Server_End_Fire_Validate()
{
    return true;
}

void UCWeaponComponent::Multicast_End_Fire_Implementation()
{
    CheckNull(GetCurrentWeapon());
    GetCurrentWeapon()->End_Fire();
}

void UCWeaponComponent::MulticastFireEffects_Implementation(ACWeapon* CurrentWeapon)
{
    if (CurrentWeapon)
    {
        CurrentWeapon->MulticastFireEffects();
    }
}

void UCWeaponComponent::Begin_Aim()
{
    if (GetOwnerRole() < ROLE_Authority)
    {
        Server_Begin_Aim();
    }

    CheckNull(GetCurrentWeapon());
    if (!GetCurrentWeapon()->CanAim())
        return;

    GetCurrentWeapon()->Begin_Aim();
    bIsAiming = true;
    bIsZoomedIn = true; // 줌인 상태로 설정
}

void UCWeaponComponent::End_Aim()
{
    if (GetOwnerRole() < ROLE_Authority)
    {
        Server_End_Aim();
    }

    CheckNull(GetCurrentWeapon());
    GetCurrentWeapon()->End_Aim();
    bIsAiming = false;  // 상태 업데이트
    bIsZoomedIn = false; // 줌인 상태 해제
}

void UCWeaponComponent::ToggleAutoFire()
{
    if (GetOwnerRole() < ROLE_Authority)
    {
        Server_ToggleAutoFire();
    }
    else
    {
        bIsAutoFireEnabled = !bIsAutoFireEnabled;
        OnRep_IsAutoFireEnabled();
    }
}

void UCWeaponComponent::Server_Begin_Aim_Implementation()
{
    Begin_Aim();
}

bool UCWeaponComponent::Server_Begin_Aim_Validate()
{
    return true;
}

void UCWeaponComponent::Server_End_Aim_Implementation()
{
    End_Aim();
}

bool UCWeaponComponent::Server_End_Aim_Validate()
{
    return true;
}

void UCWeaponComponent::Server_ToggleAutoFire_Implementation()
{
    ToggleAutoFire();
}

bool UCWeaponComponent::Server_ToggleAutoFire_Validate()
{
    return true;
}

void UCWeaponComponent::OnRep_IsAiming()
{
    if (bIsAiming)
    {
        if (GetCurrentWeapon())
        {
            GetCurrentWeapon()->Begin_Aim();
        }
    }
    else
    {
        if (GetCurrentWeapon())
        {
            GetCurrentWeapon()->End_Aim();
        }
    }
}

void UCWeaponComponent::OnRep_IsAutoFireEnabled()
{
    if (HUDWidget)
    {
        if (bIsAutoFireEnabled)
        {
            HUDWidget->OnAutoFire();  // 연사 모드로 전환
        }
        else
        {
            HUDWidget->OffAutoFire(); // 단발 모드로 전환
        }
    }
    if (GetCurrentWeapon())
    {
        GetCurrentWeapon()->ToggleAutoFire();
    }
}

bool UCWeaponComponent::IsInAim()
{
    if (!GetCurrentWeapon())
        return false;
    return GetCurrentWeapon()->IsInAim();
}

bool UCWeaponComponent::IsFiring()
{
    ACWeapon* CurrentWeapon = GetCurrentWeapon();
    return CurrentWeapon ? CurrentWeapon->IsFiring() : false;
}

FVector UCWeaponComponent::GetLeftHandLocation()
{
    CheckNullResult(GetCurrentWeapon(), FVector::ZeroVector);
    return GetCurrentWeapon()->GetLeftHandLocation();
}

FTransform UCWeaponComponent::GetArmsLeftHandTransform()
{
    CheckNullResult(GetCurrentWeapon(), FTransform());
    return GetCurrentWeapon()->GetArmsLeftHandTransform();
}

void UCWeaponComponent::Reload()
{
    if (GetOwnerRole() < ROLE_Authority)
    {
        if (!GetCurrentWeapon() || !GetCurrentWeapon()->CanReload())
        {
            return;
        }

        // 클라이언트에서도 Reload 시 발사 중지
        GetCurrentWeapon()->End_Fire(); // 이 부분 추가

        Server_Reload();
        return;
    }

    if (!GetCurrentWeapon() || !GetCurrentWeapon()->CanReload())
    {
        return;
    }

    GetCurrentWeapon()->End_Fire(); // 서버에서도 Reload 시 발사 중지

    GetCurrentWeapon()->Reload();
    Multicast_Reload();
}

void UCWeaponComponent::Eject_Magazine()
{
    // 서버 역할이 아닐 경우 서버 함수 호출
    if (GetOwnerRole() < ROLE_Authority)
    {
        Server_Eject_Magazine();
        return;
    }
    CheckNull(GetCurrentWeapon());

    GetCurrentWeapon()->Eject_Magazine();
}

void UCWeaponComponent::Spawn_Magazine()
{
    if (GetOwnerRole() < ROLE_Authority)
    {
        Server_Spawn_Magazine();
        return;
    }

    CheckNull(GetCurrentWeapon());
    GetCurrentWeapon()->Spawn_Magazine();
}

void UCWeaponComponent::Load_Magazine()
{
    if (GetOwnerRole() < ROLE_Authority)
    {
        Server_Load_Magazine();
        return;
    }

    CheckNull(GetCurrentWeapon());
    GetCurrentWeapon()->Load_Magazine();
}


void UCWeaponComponent::End_Reload()
{
    if (GetOwnerRole() < ROLE_Authority)
    {
        Server_End_Reload();
        return;
    }

    CheckNull(GetCurrentWeapon());
    GetCurrentWeapon()->End_Reload();
}


void UCWeaponComponent::Server_Reload_Implementation()
{
    Reload();
}

bool UCWeaponComponent::Server_Reload_Validate()
{
    ACWeapon* CurrentWeapon = GetCurrentWeapon();

    if (!CurrentWeapon)
    {
        return true;
    }

    bool canReload = CurrentWeapon->CanReload();

    return true;
}

void UCWeaponComponent::Multicast_Reload_Implementation()
{
    CheckNull(GetCurrentWeapon());

    ACPlayer* OwnerCharacter = Cast<ACPlayer>(GetCurrentWeapon()->GetOwner());
    if (OwnerCharacter && GetCurrentWeapon()->ReloadMontage)
    {
        OwnerCharacter->PlayAnimMontage(GetCurrentWeapon()->ReloadMontage, GetCurrentWeapon()->ReloadMontage_PlayRate);
    }

    uint8 AmmoNeeded = GetCurrentWeapon()->GetWeaponData().MaxMagazineCount - GetCurrentWeapon()->GetCurrMagazineCount();
    uint8 AmmoToReload = FMath::Min(AmmoNeeded, static_cast<uint8>(CurrentAmmo));

    // 로그 추가
    UE_LOG(LogTemp, Log, TEXT("Multicast_Reload_Implementation: AmmoNeeded = %d, AmmoToReload = %d"), AmmoNeeded, AmmoToReload);

    CurrMagazineCount = GetCurrentWeapon()->GetCurrMagazineCount() + AmmoToReload;
    CurrentAmmo = CurrentAmmo - AmmoToReload;

    // 로그 추가
    UE_LOG(LogTemp, Log, TEXT("After Multicast_Reload_Implementation: CurrMagazineCount = %d, CurrentAmmo = %d"), CurrMagazineCount, CurrentAmmo);

    GetCurrentWeapon()->SetCurrMagazineCount(CurrMagazineCount);
    GetCurrentWeapon()->SetCurrentAmmo(CurrentAmmo);

    OnRep_AmmoState();
}

void UCWeaponComponent::Server_Eject_Magazine_Implementation()
{
    CheckNull(GetCurrentWeapon());
}

bool UCWeaponComponent::Server_Eject_Magazine_Validate()
{
    return GetCurrentWeapon() != nullptr;
}

void UCWeaponComponent::Server_Spawn_Magazine_Implementation()
{
    Spawn_Magazine();
}

bool UCWeaponComponent::Server_Spawn_Magazine_Validate()
{
    return GetCurrentWeapon() != nullptr;
}

void UCWeaponComponent::Server_Load_Magazine_Implementation()
{
    Load_Magazine();
}

bool UCWeaponComponent::Server_Load_Magazine_Validate()
{
    return GetCurrentWeapon() != nullptr;
}

void UCWeaponComponent::Server_End_Reload_Implementation()
{
    End_Reload();
}

bool UCWeaponComponent::Server_End_Reload_Validate()
{
    return GetCurrentWeapon() != nullptr;
}

void UCWeaponComponent::UpdateHUD()
{
    if (GetCurrentWeapon() && HUDWidget)
    {
        HUDWidget->UpdateMagazine(CurrMagazineCount, CurrentAmmo);
        HUDWidget->UpdateWeaponType(Type);

        if (GetCurrentWeapon()->IsAutoFire())
            HUDWidget->OnAutoFire();
        else
            HUDWidget->OffAutoFire();
    }

    // 로그 추가
    UE_LOG(LogTemp, Log, TEXT("UpdateHUD: CurrMagazineCount = %d, CurrentAmmo = %d"), CurrMagazineCount, CurrentAmmo);
}

void UCWeaponComponent::SetCurrentWeaponAmmo(int32 MagazineCount, int32 AmmoCount)
{
    if (!Weapons.IsValidIndex((int32)Type) || !GetCurrentWeapon()) // 유효성 검사 추가
    {
        UE_LOG(LogTemp, Warning, TEXT("SetCurrentWeaponAmmo: Invalid weapon or null CurrentWeapon. Aborting ammo update."));
        return;
    }

    // 클라이언트에서 서버로 요청
    if (GetOwnerRole() < ROLE_Authority) // 클라이언트일 경우
    {
        Server_SetCurrentWeaponAmmo(MagazineCount, AmmoCount);
        return;
    }

    // 서버에서 멀티캐스트 호출
    if (GetOwnerRole() == ROLE_Authority)
    {
        Multicast_SetCurrentWeaponAmmo(MagazineCount, AmmoCount);
    }

    // 디버깅 로그
    UE_LOG(LogTemp, Log, TEXT("SetCurrentWeaponAmmo: Ammo update requested - MagazineCount = %d, AmmoCount = %d"),
        MagazineCount, AmmoCount);
}

void UCWeaponComponent::Server_SetCurrentWeaponAmmo_Implementation(int32 MagazineCount, int32 AmmoCount)
{
    // 서버에서 멀티캐스트 호출
    Multicast_SetCurrentWeaponAmmo(MagazineCount, AmmoCount);

    // 디버깅 로그
    UE_LOG(LogTemp, Log, TEXT("Server_SetCurrentWeaponAmmo: Ammo update relayed to clients - MagazineCount = %d, AmmoCount = %d"),
        MagazineCount, AmmoCount);
}

bool UCWeaponComponent::Server_SetCurrentWeaponAmmo_Validate(int32 MagazineCount, int32 AmmoCount)
{
    // 유효성 검사 (필요시 추가)
    return true;
}

void UCWeaponComponent::Multicast_SetCurrentWeaponAmmo_Implementation(int32 MagazineCount, int32 AmmoCount)
{
    // 현재 무기가 유효한지 확인
    if (!GetCurrentWeapon())
    {
        UE_LOG(LogTemp, Warning, TEXT("Multicast_SetCurrentWeaponAmmo: CurrentWeapon is null. Aborting ammo update."));
        return;
    }

    // 무기의 탄약 상태 업데이트
    GetCurrentWeapon()->CurrMagazineCount = MagazineCount;
    GetCurrentWeapon()->CurrentAmmo = AmmoCount;

    // 컴포넌트의 탄약 상태도 업데이트
    CurrMagazineCount = MagazineCount;
    CurrentAmmo = AmmoCount;

    // HUD 업데이트
    OnRep_AmmoState();

    // 디버깅 로그
    UE_LOG(LogTemp, Log, TEXT("Multicast_SetCurrentWeaponAmmo: CurrMagazineCount = %d, CurrentAmmo = %d"),
        CurrMagazineCount, CurrentAmmo);
}

void UCWeaponComponent::OnRep_AmmoState()
{
    UpdateHUD();
}

void UCWeaponComponent::SetMode(EWeaponType InType)
{
    // 줌인 상태일 경우 무기 변경을 막음
    if (bIsZoomedIn)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot change weapon while zoomed in"));
        return;
    }

    // 현재 무기가 발사 중인 경우 무기 변경을 막음
    if (IsFiring())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot change weapon while firing"));
        return;
    }

    // 드롭된 무기인지 확인
    if (IsWeaponDropped(InType))
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot equip %s because it has been dropped."), *UEnum::GetValueAsString(InType));
        return; // 무기가 드롭된 상태이므로 장착 방지
    }

    if (Type == InType)
    {
        SetUnarmedMode();
        return;
    }

    if (!IsUnarmedMode())
    {
        CheckFalse(Weapons[(int32)InType]->CanEquip());
        GetCurrentWeapon()->UnEquip();
    }

    CheckNull(Weapons[(int32)InType]);
    CheckFalse(Weapons[(int32)InType]->CanEquip());

    Weapons[(int32)InType]->Equip();
    ChangeType(InType);

    if (HUDWidget)
    {
        HUDWidget->SetVisibility(ESlateVisibility::Visible);
    }
}

void UCWeaponComponent::SetUnarmedMode()
{
    if (!GetCurrentWeapon())
    {
        UE_LOG(LogTemp, Warning, TEXT("GetCurrentWeapon() is NULL in SetUnarmedMode"));
        return;
    }

    if (!GetCurrentWeapon()->CanUnEquip())
    {
        UE_LOG(LogTemp, Warning, TEXT("Current weapon cannot be unequipped"));
        return;
    }

    GetCurrentWeapon()->UnEquip();
    ChangeType(EWeaponType::Max);

    if (HUDWidget)
    {
        HUDWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UCWeaponComponent::ChangeType(EWeaponType InType)
{
    EWeaponType prevType = Type;
    Type = InType;

    if (OnWeaponTypeChanged.IsBound())
        OnWeaponTypeChanged.Broadcast(prevType, InType);
}

ACWeapon* UCWeaponComponent::GetCurrentWeapon()
{
    CheckTrueResult(IsUnarmedMode(), nullptr);
    return Weapons[(int)Type];
}

void UCWeaponComponent::UnEquipCurrentWeapon()
{
    if (GetCurrentWeapon())
    {
        GetCurrentWeapon()->UnEquip(); // 현재 무기 해제
    }
}

void UCWeaponComponent::AddDroppedWeapon(EWeaponType WeaponType)
{
    // DroppedWeapons 배열에 무기 유형 추가
    if (!IsWeaponDropped(WeaponType))
    {
        DroppedWeapons.Add(WeaponType);
    }
}

bool UCWeaponComponent::IsWeaponDropped(EWeaponType WeaponType) const
{
    return DroppedWeapons.Contains(WeaponType);
}

void UCWeaponComponent::RemoveDroppedWeapon(EWeaponType WeaponType)
{
    if (IsWeaponDropped(WeaponType))
    {
        DroppedWeapons.Remove(WeaponType);
    }
}


void UCWeaponComponent::OnAim_Arms_Begin(ACWeapon* InThisWeapon)
{
    for (ACWeapon* weapon : Weapons)
    {
        if (weapon == InThisWeapon)
            continue;
        if (weapon == nullptr)
            continue;
        weapon->SetHidden(true);
    }
}

void UCWeaponComponent::OnAim_Arms_End()
{
    for (ACWeapon* weapon : Weapons)
    {
        if (weapon == nullptr)
            continue;
        weapon->SetHidden(false);
    }
}
