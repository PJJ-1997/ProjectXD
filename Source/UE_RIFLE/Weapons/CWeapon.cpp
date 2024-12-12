#include "Weapons/CWeapon.h"
#include "Global.h"
#include "Characters/CPlayer.h"        
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Particles/ParticleSystem.h"
#include "Gameframework/SpringArmComponent.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "Sound/SoundWave.h"
#include "Widgets/CUserWidget_CrossHair.h"
#include "Widgets/CUserWidget_HUD.h"
#include "Weapons/CBullet.h"
#include "Weapons/CMagaZine.h"
#include "Weapons/CWeaponComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Interface/DamageInterface.h"
#include "DrawDebugHelpers.h"

//////////////////////////////////////////////////////////////////////////////////////
void FWeaponAimData::SetDataByNoneCurve(ACharacter* InOwner)
{
    // AR4, AK47 Aim Setting
    USpringArmComponent* sprinArm = CHelpers::GetComponent<USpringArmComponent>(InOwner);

    sprinArm->TargetArmLength = TargetArmLength;
    sprinArm->SocketOffset = SocketOffset;

    UCameraComponent* camera = CHelpers::GetComponent<UCameraComponent>(InOwner);
    camera->FieldOfView = FieldOfView;
}

void FWeaponAimData::SetData(ACharacter* InOwner)
{
    USpringArmComponent* sprinArm = CHelpers::GetComponent<USpringArmComponent>(InOwner);
    sprinArm->TargetArmLength = TargetArmLength;
    sprinArm->SocketOffset = SocketOffset;
}

//////////////////////////////////////////////////////////////////////////////////////
ACWeapon::ACWeapon()
{
    PrimaryActorTick.bCanEverTick = true;
    // Root & Mesh
    CHelpers::CreateComponent<USceneComponent>(this, &Root, "Root");
    CHelpers::CreateComponent<USkeletalMeshComponent>(this, &Mesh, "Mesh", Root);
    CHelpers::CreateActorComponent<UTimelineComponent>(this, &Timeline, "Timeline");
    WeaponDataTable = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/WeaponData.WeaponData'"));

    // Decal
    CHelpers::GetAsset<UMaterialInstanceConstant>(&HitDecal, "MaterialInstanceConstant'/Game/Materials/M_Decal_Inst.M_Decal_Inst'");
    CHelpers::GetAsset<UParticleSystem>(&HitParticle, "ParticleSystem'/Game/Effects/P_Impact_Default.P_Impact_Default'");

    // Aim Asset
    CHelpers::GetAsset<UCurveFloat>(&AimCurve, "CurveFloat'/Game/Weapons/Curve_Aim.Curve_Aim'");

    // Fire Asset
    CHelpers::GetAsset<UParticleSystem>(&FlashParticle, "ParticleSystem'/Game/Effects/P_Muzzleflash.P_Muzzleflash'");
    CHelpers::GetAsset<UParticleSystem>(&EjectParticle, "ParticleSystem'/Game/Effects/P_Eject_bullet.P_Eject_bullet'");
    CHelpers::GetAsset<USoundWave>(&FireSound, "SoundWave'/Game/Sounds/S_RifleShoot.S_RifleShoot'");

    // Bullet
    CHelpers::GetClass<ACBullet>(&BulletClass, "Blueprint'/Game/Weapons/BP_CBullet.BP_CBullet_C'");

    bReplicates = true;
    SetReplicateMovement(true);

    PickupCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("PickupCollision"));
    PickupCollision->SetupAttachment(Mesh);  // 총의 메시에 붙이기
    PickupCollision->SetBoxExtent(FVector(40.0f, 40.0f, 40.0f));  // 콜리전 크기 설정
    PickupCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // 기본적으로 비활성화
    PickupCollision->SetCollisionResponseToAllChannels(ECR_Ignore);  // 모든 채널 무시
    PickupCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);  // 플레이어와만 오버랩
    PickupCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));

    // 콜리전을 시각적으로 표시
    PickupCollision->SetHiddenInGame(false);  // 게임 내에서 항상 보이도록 설정
    PickupCollision->SetVisibility(true);
}

void ACWeapon::BeginPlay()
{
    Super::BeginPlay();

    PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &ACWeapon::OnPickupOverlapBegin);
    DrawDebugBox(GetWorld(), PickupCollision->GetComponentLocation(), PickupCollision->GetScaledBoxExtent(), FColor::Green, false, -1, 0, 2);

    PickupCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 초기화 시 콜리전 비활성화
    PickupCollision->SetVisibility(false); // 시각적으로 보이지 않도록 설정

    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACWeapon::CreateCrossHair, 0.1f, false);

    Owner = Cast<ACPlayer>(GetOwner());
    if (!Owner)
    {
        return;
    }

    BaseData.SetDataByNoneCurve(Owner);

    Camera = CHelpers::GetComponent<UCameraComponent>(Owner);

    if (HolsterSocketName.IsValid())
    {
        CHelpers::AttachTo(this, Owner->GetMesh(), HolsterSocketName);
    }


    FString ContextString;
    FWeaponData* WeaponDataRow = WeaponDataTable->FindRow<FWeaponData>(WeaponName, ContextString);
    if (WeaponDataRow)
    {
        WeaponData = *WeaponDataRow;
        CurrMagazineCount = WeaponData.MaxMagazineCount;
        CurrentAmmo = WeaponData.MaxAmmo - CurrMagazineCount;
    }

    if (Owner)
    {
        UCWeaponComponent* WeaponComp = Owner->FindComponentByClass<UCWeaponComponent>();
        if (WeaponComp && WeaponComp->HUDWidget)
        {
            WeaponComp->HUDWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }


}

void ACWeapon::CreateCrossHair()
{
    if (!Owner || !CrossHairClass) // null 체크
    {
        UE_LOG(LogTemp, Warning, TEXT("Owner or CrossHairClass is null in CreateCrossHair"));
        return;
    }

    CrossHair = CreateWidget<UCUserWidget_CrossHair>(Owner->GetController<APlayerController>(), CrossHairClass);
    if (CrossHair)
    {
        CrossHair->AddToViewport();
        CrossHair->SetVisibility(ESlateVisibility::Hidden);
        CrossHair->UpdateSpreadRange(CurrSpreadRadius, WeaponData.MaxSpreadAlignment);
    }
}

void ACWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (LastAddSpreadTime >= 0.0f)
    {
        float TimeSinceLastShot = GetWorld()->GetTimeSeconds() - LastAddSpreadTime;
        if (TimeSinceLastShot >= (WeaponData.AutoFireInterval + 0.25f))
        {
            CurrSpreadRadius = 0.0f;
            LastAddSpreadTime = 0.0f;
        }
        else
        {
            CurrSpreadRadius = FMath::FInterpTo(CurrSpreadRadius, 0.0f, DeltaTime, 2.0f);
        }

        if (CrossHair)
        {
            CrossHair->UpdateSpreadRange(CurrSpreadRadius, WeaponData.MaxSpreadAlignment);
        }
    }
}

void ACWeapon::ToggleAutoFire()
{
    bAutoFire = !bAutoFire;
    if (bAutoFire)
    {
        UE_LOG(LogTemp, Log, TEXT("Auto Fire Enabled"));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Auto Fire Disabled"));
    }
}

bool ACWeapon::CanEquip()
{
    bool b = false;
    b = b | bEquipping;
    b = b | bReload;
    b = b | bFiring;
    return !b;
}

void ACWeapon::Equip()
{
    bEquipping = true;
    if (!!EquipMontage)
        Owner->PlayAnimMontage(EquipMontage, EquipMontage_PlayRate);

    DeactivateCollision();

  //  // Show HUD and update ammo info
  //  if (Owner)
  //  {
  //      UCWeaponComponent* WeaponComp = Owner->FindComponentByClass<UCWeaponComponent>();
  //      if (WeaponComp && WeaponComp->HUDWidget)
  //      {
  //          WeaponComp->HUDWidget->SetVisibility(ESlateVisibility::Visible);
  //          WeaponComp->CurrMagazineCount = CurrMagazineCount;
  //          WeaponComp->CurrentAmmo = CurrentAmmo;
  //          WeaponComp->OnRep_AmmoState(); // Ensure HUD is updated with current ammo
  //      }
  //  }
}

void ACWeapon::Begin_Equip()
{
    if (RightHandSocketName.IsValid())
        CHelpers::AttachTo(this, Owner->GetMesh(), RightHandSocketName);
}

void ACWeapon::End_Equip()
{
    bEquipping = false;

    // 크로스헤어 표시
    if (CrossHair)
    {
        CrossHair->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("End_Equip: CrossHair is null."));
    }

    // HUD와 총알 정보 동기화
    if (Owner)
    {
        UCWeaponComponent* WeaponComp = Owner->FindComponentByClass<UCWeaponComponent>();
        if (WeaponComp)
        {
            // 무기 정보와 컴포넌트 상태 동기화
            WeaponComp->CurrMagazineCount = CurrMagazineCount;
            WeaponComp->CurrentAmmo = CurrentAmmo;

            // HUD 업데이트
            WeaponComp->OnRep_AmmoState();

            // 디버깅 로그
            UE_LOG(LogTemp, Log, TEXT("End_Equip: HUD updated with ammo - MagazineCount = %d, AmmoCount = %d"),
                CurrMagazineCount, CurrentAmmo);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("End_Equip: WeaponComp not found on Owner."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("End_Equip: Owner is null."));
    }
}

bool ACWeapon::CanUnEquip()
{
    bool b = false;
    b = b | bEquipping;
    b = b | bReload;
    return !b;
}

void ACWeapon::UnEquip()
{

    Mesh->SetVisibility(false);
    if (!!CrossHair)
        CrossHair->SetVisibility(ESlateVisibility::Hidden);
}

bool ACWeapon::CanFire()
{
    // Check if weapon is equipping, reloading, already firing, or if the magazine is empty
    if (bEquipping || bReload || bFiring || CurrMagazineCount <= 0)
    {
        return false;
    }

    // Explicitly check if the reload montage is playing
    if (Owner && Owner->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadMontage))
    {
        return false;
    }

    return true;
}

void ACWeapon::Begin_Fire()
{
    if (!CanFire()) // Ensure firing can only start if allowed
    {
        return;
    }

    bFiring = true;

    if (bAutoFire)
    {
        GetWorld()->GetTimerManager().SetTimer(AutoFireHandle, this, &ACWeapon::OnFiring, WeaponData.AutoFireInterval, true);
    }
    OnFiring();
}

void ACWeapon::End_Fire()
{
    bFiring = false;
    if (GetWorld()->GetTimerManager().IsTimerActive(AutoFireHandle))
        GetWorld()->GetTimerManager().ClearTimer(AutoFireHandle);
}

void ACWeapon::OnFiring()
{
    if (CurrMagazineCount > 0)
    {
        if (!Owner)
        {
            return;
        }

        FVector CameraLocation;
        FRotator CameraRotation;
        if (!Owner->GetController())
        {
            return;
        }
        Owner->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

        FVector MuzzleLocation = Mesh->GetSocketLocation("Muzzle");
        if (MuzzleLocation == FVector::ZeroVector)
        {
            return;
        }

        FVector Direction = CameraRotation.Vector();
        FVector Start = CameraLocation;
        FVector End = Start + Direction * HitDistance;

        FHitResult HitResult;
        TArray<AActor*> IgnoredActors;
        IgnoredActors.Add(Owner);

        bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End,
            UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), false, IgnoredActors,
            EDrawDebugTrace::None, HitResult, true);

        if (bHit)
        {
            FRotator ImpactRotation = UKismetMathLibrary::FindLookAtRotation(HitResult.Location, HitResult.TraceStart);
            if (HitDecal)
            {
                UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), HitDecal, FVector(5), HitResult.Location, ImpactRotation, 4);
                if (Decal)
                {
                    Decal->SetFadeScreenSize(0);
                }
            }
            if (HitParticle)
            {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitResult.Location, ImpactRotation);
            }

            // 캐릭터를 맞추었을 경우에만 크로스헤어 색 변경
            ACharacter* HitCharacter = Cast<ACharacter>(HitResult.GetActor());
            if (HitCharacter)
            {
                // 데미지 인터페이스를 구현한 액터라면 데미지를 준다.
                IDamageInterface* DamageActor = Cast<IDamageInterface>(HitResult.GetActor());
                if (DamageActor)
                {
                    // 부위별 데미지 배율 계산
                    FString HitBoneName = HitResult.BoneName.ToString();
                    float DamageMultiplier = 1.0f;

                    if (HitBoneName == "Head")
                    {
                        DamageMultiplier = 2.0f;
                    }
                    else if (HitBoneName == "Torso")
                    {
                        DamageMultiplier = 1.0f;
                    }
                    else if (HitBoneName == "Legs")
                    {
                        DamageMultiplier = 0.5f;
                    }

                    float FinalDamage = WeaponData.Damage * DamageMultiplier;
                    DamageActor->TakeDamage(HitResult, FinalDamage);
                }

                // 캐릭터 맞춘 경우에만 크로스헤어 색상 변경
                if (CrossHair)
                {
                    CrossHair->SetCrossHairImagesColor(FLinearColor::Red);

                    // 일정 시간 후 원래 색 복귀
                    FTimerHandle TimerHandle;
                    GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateWeakLambda(this, [this]()
                        {
                            if (CrossHair)
                            {
                                CrossHair->ResetCrossHairImagesColor();
                            }
                        }), 0.2f, false);

                    // 크로스헤어 스프레드 업데이트
                    CurrSpreadRadius += WeaponData.SpreadSpeed;
                    LastAddSpreadTime = GetWorld()->GetTimeSeconds();
                    CrossHair->UpdateSpreadRange(CurrSpreadRadius, WeaponData.MaxSpreadAlignment);
                }
            }
        }

        if (EjectParticle)
        {
            UGameplayStatics::SpawnEmitterAttached(EjectParticle, Mesh, "Eject", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset);
        }
        if (FlashParticle)
        {
            UGameplayStatics::SpawnEmitterAttached(FlashParticle, Mesh, "Muzzle", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset);
        }

        if (FireSound)
        {
            UGameplayStatics::SpawnSoundAtLocation(GetWorld(), FireSound, MuzzleLocation);
        }

        if (CameraShakeClass)
        {
            APlayerController* Controller = Owner->GetController<APlayerController>();
            if (Controller)
            {
                APlayerCameraManager* CameraManager = Controller->PlayerCameraManager;
                if (CameraManager)
                {
                    CameraManager->StartCameraShake(CameraShakeClass);
                }
            }
        }

        float VerticalRecoil = -WeaponData.RecoilRate * UKismetMathLibrary::RandomFloatInRange(0.6f, 1.0f);
        float HorizontalRecoil = WeaponData.RecoilHorizontalRate * UKismetMathLibrary::RandomFloatInRange(-1.0f, 1.0f);
        Owner->AddControllerPitchInput(VerticalRecoil);
        Owner->AddControllerYawInput(HorizontalRecoil);

        MulticastFireEffects();

        CurrMagazineCount--;

        if (UCWeaponComponent* WeaponComp = Owner->FindComponentByClass<UCWeaponComponent>())
        {
            WeaponComp->CurrMagazineCount = CurrMagazineCount;
            WeaponComp->CurrentAmmo = CurrentAmmo;
            WeaponComp->OnRep_AmmoState();
        }

        if (CurrMagazineCount <= 0 && CanReload())
        {
            Reload();
        }
    }
}

void ACWeapon::MulticastFireEffects_Implementation()
{
    FVector MuzzleLocation = Mesh->GetSocketLocation("Muzzle");  // 소켓 위치 재확인
    FRotator MuzzleRotation = Mesh->GetSocketRotation("Muzzle"); // 소켓 회전 재확인

    if (EjectParticle)
    {
        UGameplayStatics::SpawnEmitterAttached(EjectParticle, Mesh, "Eject", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset);
    }
    if (FlashParticle)
    {
        UGameplayStatics::SpawnEmitterAttached(FlashParticle, Mesh, "Muzzle", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset);
    }

    if (FireSound)
    {
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), FireSound, MuzzleLocation);
    }
}

void ACWeapon::Reload()
{
    if (!CanReload())
    {
        return;
    }

    bReload = true;

    End_Aim();
    End_Fire();

    if (Owner && ReloadMontage)
    {
        Owner->PlayAnimMontage(ReloadMontage, ReloadMontage_PlayRate);
    }

    uint8 AmmoNeeded = WeaponData.MaxMagazineCount - CurrMagazineCount;
    uint8 AmmoToReload = FMath::Min(AmmoNeeded, static_cast<uint8>(CurrentAmmo));

    // 로그 추가
    UE_LOG(LogTemp, Log, TEXT("Reloading: AmmoNeeded = %d, AmmoToReload = %d"), AmmoNeeded, AmmoToReload);

    CurrMagazineCount += AmmoToReload;
    CurrentAmmo -= AmmoToReload;

    // 로그 추가
    UE_LOG(LogTemp, Log, TEXT("After Reload: CurrMagazineCount = %d, CurrentAmmo = %d"), CurrMagazineCount, CurrentAmmo);

    bReload = false;

    if (HasAuthority())
    {
        UCWeaponComponent* WeaponComp = Owner->FindComponentByClass<UCWeaponComponent>();
        if (WeaponComp)
        {
            WeaponComp->CurrMagazineCount = CurrMagazineCount;
            WeaponComp->CurrentAmmo = CurrentAmmo;
            WeaponComp->OnRep_AmmoState();
        }
    }
}

bool ACWeapon::CanAim()
{
    bool b = false;
    b = b | bEquipping;
    b = b | bReload;
    b = b | bInAim;
    return !b;
}

void ACWeapon::Begin_Aim()
{
    bInAim = true;
    if (!!AimCurve)
    {
        Timeline->PlayFromStart();
        AimData.SetData(Owner);
        return;
    }
    AimData.SetDataByNoneCurve(Owner);
}

void ACWeapon::End_Aim()
{
    CheckFalse(bInAim);
    bInAim = false;
    if (!!AimCurve)
    {
        Timeline->ReverseFromEnd();
        BaseData.SetData(Owner);
        return;
    }
    BaseData.SetDataByNoneCurve(Owner);
}

bool ACWeapon::CanReload()
{
    bool canReload = !bEquipping && !bReload && CurrMagazineCount < WeaponData.MaxMagazineCount && CurrentAmmo > 0;

    return canReload;
}


void ACWeapon::Eject_Magazine()
{
    // 서버에서만 실행
    if (!HasAuthority()) return;

    CheckNull(MagazineClass);

    if (MagazineBoneName.IsValid())
        Mesh->HideBoneByName(MagazineBoneName, EPhysBodyOp::PBO_None);

    FTransform Transform = Mesh->GetSocketTransform(MagazineBoneName);
    ACMagaZine* Magazine = GetWorld()->SpawnActor<ACMagaZine>(
        MagazineClass,
        Transform.GetLocation(),
        Transform.GetRotation().Rotator(),
        FActorSpawnParameters()
    );

    if (Magazine)
    {
        Magazine->SetEject();
        Magazine->SetLifeSpan(5.0f);
    }
    Multicast_Eject_Magazine();
}

void ACWeapon::Multicast_Eject_Magazine_Implementation()
{
    if (!HasAuthority())
    {

        if (MagazineBoneName.IsValid())
            Mesh->HideBoneByName(MagazineBoneName, EPhysBodyOp::PBO_None);

        FTransform Transform = Mesh->GetSocketTransform(MagazineBoneName);
        ACMagaZine* Magazine = GetWorld()->SpawnActor<ACMagaZine>(
            MagazineClass,
            Transform.GetLocation(),
            Transform.GetRotation().Rotator(),
            FActorSpawnParameters()
        );

        if (Magazine)
        {
            Magazine->SetEject();
            Magazine->SetLifeSpan(5.0f);
        }
    }
}

void ACWeapon::Spawn_Magazine()
{
    CheckNull(MagazineClass);

    FActorSpawnParameters params;
    params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    MagaZine = GetWorld()->SpawnActor<ACMagaZine>(MagazineClass, params);
    CHelpers::AttachTo(MagaZine, Owner->GetMesh(), MagazineSocketName);
}

void ACWeapon::Multicast_Spawn_Magazine_Implementation()
{
    if (HasAuthority()) return;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ACMagaZine* Magazine = GetWorld()->SpawnActor<ACMagaZine>(MagazineClass, Params);
    if (Magazine)
    {
        CHelpers::AttachTo(Magazine, Owner->GetMesh(), MagazineSocketName);
    }
}
void ACWeapon::Load_Magazine()
{
    if (!HasAuthority()) return;

    if (MagazineBoneName.IsValid())
        Mesh->UnHideBoneByName(MagazineBoneName);

    if (!!MagaZine)
        MagaZine->Destroy();

    Multicast_Load_Magazine();
}

void ACWeapon::Multicast_Load_Magazine_Implementation()
{
    if (HasAuthority()) return;

    if (MagazineBoneName.IsValid())
        Mesh->UnHideBoneByName(MagazineBoneName);

    if (!!MagaZine)
        MagaZine->Destroy();
}

void ACWeapon::End_Reload()
{
    if (!HasAuthority()) return;

    bReload = false;

    uint8 AmmoNeeded = WeaponData.MaxMagazineCount - CurrMagazineCount;
    uint8 AmmoToReload = FMath::Min(static_cast<int32>(AmmoNeeded), CurrentAmmo);

    CurrMagazineCount += AmmoToReload;
    CurrentAmmo -= AmmoToReload;

    Multicast_End_Reload();
}

void ACWeapon::Multicast_End_Reload_Implementation()
{
    if (HasAuthority()) return;

    bReload = false;
}

uint8 ACWeapon::GetCurrMagazineCount() const
{
    return CurrMagazineCount;
}

EWeaponType ACWeapon::GetWeaponType() const
{
    return WeaponType;
}

void ACWeapon::OnPickupOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (HasAuthority())
    {
        if (OtherActor && OtherActor != this)
        {
            ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
            UCapsuleComponent* PlayerCapsule = Cast<UCapsuleComponent>(OtherComp);

            if (PlayerCharacter && PlayerCapsule && PlayerCapsule == PlayerCharacter->GetCapsuleComponent())
            {
                UCWeaponComponent* WeaponComponent = PlayerCharacter->FindComponentByClass<UCWeaponComponent>();
                if (WeaponComponent)
                {
                    if (WeaponComponent->GetCurrentWeapon() && WeaponComponent->GetCurrentWeapon()->GetWeaponType() == GetWeaponType())
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Player already has a weapon of this type!"));
                        return; // 이미 같은 타입의 무기를 들고 있으면 무기를 주울 수 없음
                    }

                    int32 MagazineCount, AmmoCount;

                    // 총알 정보를 불러오기
                    LoadAmmo(MagazineCount, AmmoCount);
                    // 디버깅 로그
                    UE_LOG(LogTemp, Log, TEXT("OnPickupOverlapBegin: Weapon %s has ammo - MagazineCount = %d, CurrentAmmo = %d"),
                        *GetName(), MagazineCount, AmmoCount);

                    WeaponComponent->RemoveDroppedWeapon(GetWeaponType());
                    WeaponComponent->SetMode(GetWeaponType());
                    WeaponComponent->SetCurrentWeaponAmmo(MagazineCount, AmmoCount); // 이부분에서 총이 없어서 업데이트 안되는것 !?

                    // 불러온 후 로그 출력
                    UE_LOG(LogTemp, Log, TEXT("After loading ammo: Weapon %s has MagazineCount = %d, CurrentAmmo = %d"),
                        *GetName(), MagazineCount, AmmoCount);


                    SetOwner(PlayerCharacter);

                    Multicast_DestroyWeapon();
                }
            }
        }
    }
}

void ACWeapon::Multicast_DestroyWeapon_Implementation()
{
    Destroy();
    UE_LOG(LogTemp, Log, TEXT("Weapon destroyed on all clients"));
}

void ACWeapon::ActivateCollision()
{
    PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // 충돌 활성화
    PickupCollision->SetCollisionResponseToAllChannels(ECR_Ignore);  // 모든 채널 무시
    PickupCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);  // 플레이어와 충돌 설정
    PickupCollision->SetHiddenInGame(false);  // 시각적으로 보이도록 설정
    PickupCollision->SetVisibility(true);  // 항상 보이도록 설정
}

void ACWeapon::DeactivateCollision()
{
    PickupCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    PickupCollision->SetVisibility(false);  // 비활성화 시 보이지 않도록 설정
    PickupCollision->SetHiddenInGame(true);
}

void ACWeapon::SaveAmmo(int32 MagazineCount, int32 AmmoCount)
{
    if (HasAuthority()) // 서버에서만 변경 가능
    {
        CurrMagazineCount = MagazineCount;
        CurrentAmmo = AmmoCount;

        // 로그 추가
        UE_LOG(LogTemp, Log, TEXT("Weapon %s saved ammo: MagazineCount = %d, CurrentAmmo = %d"),
            *GetName(), CurrMagazineCount, CurrentAmmo);

        OnRepAmmoChanged(); // 변경 즉시 반영
    }
}

void ACWeapon::LoadAmmo(int32& OutMagazineCount, int32& OutAmmoCount) const
{
    OutMagazineCount = CurrMagazineCount;
    OutAmmoCount = CurrentAmmo;

    // 로그 추가
    UE_LOG(LogTemp, Log, TEXT("Weapon %s loaded ammo: MagazineCount = %d, CurrentAmmo = %d"),
        *GetName(), OutMagazineCount, OutAmmoCount);
}

void ACWeapon::OnRepAmmoChanged()
{
    if (ACharacter* OwningCharacter = Cast<ACharacter>(Owner))
    {
        UCWeaponComponent* WeaponComp = OwningCharacter->FindComponentByClass<UCWeaponComponent>();
        if (WeaponComp)
        {
            WeaponComp->UpdateHUD();
        }
    }
}


void ACWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACWeapon, CurrMagazineCount);
    DOREPLIFETIME(ACWeapon, CurrentAmmo);
}




