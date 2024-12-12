#include "Weapons/CWeapon_AR4.h"
#include "Global.h"
#include "CWeaponComponent.h"
#include "Animation/AnimMontage.h"
#include "Camera/CameraShakeBase.h"
#include "Widgets/CUserWidget_CrossHair.h"
#include "Weapons/CMagazine.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Characters/CPlayer.h"

ACWeapon_AR4::ACWeapon_AR4()
{
    // 에셋을 가져오고
    USkeletalMesh* mesh;
    CHelpers::GetAsset<USkeletalMesh>(&mesh, "SkeletalMesh'/Game/FPS_Weapon_Bundle/Weapons/Meshes/AR4/SK_AR4.SK_AR4'");

    Mesh->SetSkeletalMesh(mesh);

    // 장착관련 데이터
    {
        HolsterSocketName = "Rifle_AR4_Holster";
        CHelpers::GetAsset<UAnimMontage>(&EquipMontage, "AnimMontage'/Game/Character/Montages/Rifle_Equip_AR4_Montage.Rifle_Equip_AR4_Montage'");
        EquipMontage_PlayRate = 1.5f;
        RightHandSocketName = "Rifle_AR4_RightHand";
    }

    // Aim
    {
        BaseData.TargetArmLength = 200;
        BaseData.SocketOffset = FVector(0, 50, 15);
        BaseData.FieldOfView = 90;

        AimData.TargetArmLength = 30; // 원래 80
        AimData.SocketOffset = FVector(55, 0, 10); //(0, 55, 10)
        AimData.FieldOfView = 55; // 65

        LeftHandLocation = FVector(-32.5f, 15.5f, 4.0f);  //(-32.5f, 15.5f, 7.0f);  
    }

    // Fire
    {
        CHelpers::GetClass<UCameraShakeBase>(&CameraShakeClass, "Blueprint'/Game/Weapons/BP_CameraShake_AR4.BP_CameraShake_AR4_C'");
    }

    // UI
    {
        CHelpers::GetClass<UCUserWidget_CrossHair>(&CrossHairClass, "WidgetBlueprint'/Game/Widgets/WB_CrossHair.WB_CrossHair_C'");
    }

    //Magazine
    {
        CHelpers::GetAsset<UAnimMontage>(&ReloadMontage, "AnimMontage'/Game/Character/Montages/Rifle_Reload_Montage.Rifle_Reload_Montage'");
        ReloadMontage_PlayRate = 1.5f;
        MagazineBoneName = "b_gun_mag";
        MagazineSocketName = "Rifle_Magazine";
        CHelpers::GetClass<ACMagaZine>(&MagazineClass, "Blueprint'/Game/Weapons/BP_CMagaZine.BP_CMagaZine_C'");
    }
    // Arms
    {
        ArmsMeshTransform.SetLocation(FVector(-8.25f, -5.85f, -158.935f));
        ArmsMeshTransform.SetRotation(FQuat(FRotator(-0.5f, -11.85f, -1.23f)));

        ArmsLeftHandTransform.SetLocation(FVector(-33, 11, -1.5f));
        ArmsLeftHandTransform.SetRotation(FQuat(FRotator(-4.0f, -138.0f, 77.0f)));
    }
    WeaponName = "AR4";
    Mesh->SetVisibility(false);
}

void ACWeapon_AR4::BeginPlay()
{
    Super::BeginPlay();

    Mesh->SetVisibility(false);

    FString ContextString;
    FWeaponData* WeaponDataRow = WeaponDataTable->FindRow<FWeaponData>(WeaponName, ContextString);
    if (WeaponDataRow)
    {
        WeaponData = *WeaponDataRow;
    }
}

void ACWeapon_AR4::Begin_Equip()
{
    Super::Begin_Equip();
    Mesh->SetVisibility(true);

    if (RightHandSocketName.IsValid())
        CHelpers::AttachTo(this, Owner->GetMesh(), RightHandSocketName);

    Owner->GetArms()->SetRelativeTransform(ArmsMeshTransform);
}

void ACWeapon_AR4::End_Equip()
{
    Super::End_Equip();
}

void ACWeapon_AR4::Begin_Aim()
{
    Super::Begin_Aim();

    APlayerController* OwnerPlayerController = Cast<APlayerController>(Owner->GetController());
    if (OwnerPlayerController && OwnerPlayerController->IsLocalController())
    {
        if (!!CrossHair)
            CrossHair->SetVisibility(ESlateVisibility::Hidden);

        Owner->GetArms()->SetVisibility(true);        // ABP_Arms
        Owner->GetMesh()->SetVisibility(false);       // ABP_Character
        Owner->GetBackpack()->SetVisibility(false);

        Mesh->AttachToComponent(Owner->GetArms(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandSocketName);
    }

    CHelpers::GetComponent<UCWeaponComponent>(Owner)->OnWeaponAim_Arms_Begin.Broadcast(this);
}

void ACWeapon_AR4::End_Aim()
{
    Super::End_Aim();

    APlayerController* OwnerPlayerController = Cast<APlayerController>(Owner->GetController());
    if (OwnerPlayerController && OwnerPlayerController->IsLocalController())
    {
        if (!!CrossHair)
            CrossHair->SetVisibility(ESlateVisibility::Visible);

        Owner->GetMesh()->SetVisibility(true);       // ABP_Character
        Owner->GetArms()->SetVisibility(false);      // ABP_Arms
        Owner->GetBackpack()->SetVisibility(false);

        Mesh->AttachToComponent(Owner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandSocketName);
    }

    CHelpers::GetComponent<UCWeaponComponent>(Owner)->OnWeaponAim_Arms_End.Broadcast();
}
