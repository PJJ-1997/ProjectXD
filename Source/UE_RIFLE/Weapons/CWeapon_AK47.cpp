#include "Weapons/CWeapon_AK47.h"
#include "Global.h"
#include "CWeaponComponent.h"
#include "Animation/AnimMontage.h"
#include "Camera/CameraShakeBase.h"
#include "Widgets/CUserWidget_CrossHair.h"
#include "Weapons/CMagazine.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Characters/CPlayer.h"

ACWeapon_AK47::ACWeapon_AK47()
{

    USkeletalMesh* mesh;
    CHelpers::GetAsset<USkeletalMesh>(&mesh, "SkeletalMesh'/Game/FPS_Weapon_Bundle/Weapons/Meshes/Ka47/SK_KA47.SK_KA47'");
    Mesh->SetSkeletalMesh(mesh);

    CHelpers::CreateComponent<UStaticMeshComponent>(this, &Sight, "Sight", Mesh, "DotSight");
    UStaticMesh* staticMesh;
    CHelpers::GetAsset<UStaticMesh>(&staticMesh, "StaticMesh'/Game/FPS_Weapon_Bundle/Weapons/Meshes/Accessories/SM_T4_Sight.SM_T4_Sight'");
    Sight->SetStaticMesh(staticMesh);
    Sight->SetRelativeScale3D(FVector(1, 0.95f, 1));
    Sight->SetCollisionProfileName("NoCollision");

    {
        HolsterSocketName = "Rifle_AK47_Holster";
        CHelpers::GetAsset<UAnimMontage>(&EquipMontage, "AnimMontage'/Game/Character/Montages/Rifle_Equip_AK47_Montage.Rifle_Equip_AK47_Montage'");
        EquipMontage_PlayRate = 1.5f;
        RightHandSocketName = "Rifle_AK47_RightHand";
        LeftHandSocketName = "Rifle_AK47_LeftHand";
    }

    // Aim
    {
        BaseData.TargetArmLength = 200;
        BaseData.SocketOffset = FVector(0, 50, 15);
        BaseData.FieldOfView = 90;

        AimData.TargetArmLength = 30;
        AimData.SocketOffset = FVector(55, 0, 10);
        AimData.FieldOfView = 55;

        LeftHandLocation = FVector(-32.5f, 15.5f, 4.0f);
    }


    // Fire
    {
        CHelpers::GetClass<UCameraShakeBase>(&CameraShakeClass, "Blueprint'/Game/Weapons/BP_CameraShake_AK47.BP_CameraShake_AK47_C'");
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
        CHelpers::GetClass<ACMagaZine>(&MagazineClass, "Blueprint'/Game/Weapons/BP_CMagaZine_AK47.BP_CMagaZine_AK47_C'");
    }

    // Arms
    {
        ArmsMeshTransform.SetLocation(FVector(-14.25f, -5.85f, -156.935f));
        ArmsMeshTransform.SetRotation(FQuat(FRotator(-0.5f, -11.85f, -1.2f)));

        ArmsLeftHandTransform.SetLocation(FVector(-33, 11, -1.5f));
        ArmsLeftHandTransform.SetRotation(FQuat(FRotator(-4.0f, -138.0f, 77.0f)));
    }
    WeaponName = "AK47";
}

void ACWeapon_AK47::BeginPlay()
{
    Super::BeginPlay();

    Mesh->SetVisibility(false);
    Sight->SetVisibility(false);

    FString ContextString;
    FWeaponData* WeaponDataRow = WeaponDataTable->FindRow<FWeaponData>(WeaponName, ContextString);
    if (WeaponDataRow)
    {
        WeaponData = *WeaponDataRow;
    }
}

void ACWeapon_AK47::Begin_Equip()
{
    Super::Begin_Equip();
    Mesh->SetVisibility(true);
    Sight->SetVisibility(true);

    if (LeftHandSocketName.IsValid())
        CHelpers::AttachTo(this, Owner->GetMesh(), LeftHandSocketName);

    Owner->GetArms()->SetRelativeTransform(ArmsMeshTransform);

    FTimerHandle SwitchHandTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(SwitchHandTimerHandle, this, &ACWeapon_AK47::SwitchToRightHand, 1.0f, false);
}

void ACWeapon_AK47::SwitchToRightHand()
{
    if (RightHandSocketName.IsValid())
        CHelpers::AttachTo(this, Owner->GetMesh(), RightHandSocketName);
}

void ACWeapon_AK47::UnEquip()
{
    Sight->SetVisibility(false);
    Mesh->SetVisibility(false);

    if (!!CrossHair)
        CrossHair->SetVisibility(ESlateVisibility::Hidden);
}

void ACWeapon_AK47::End_Equip()
{
    Super::End_Equip();

}

void ACWeapon_AK47::Begin_Aim()
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

void ACWeapon_AK47::End_Aim()
{
    Super::End_Aim();

    APlayerController* OwnerPlayerController = Cast<APlayerController>(Owner->GetController());
    if (OwnerPlayerController && OwnerPlayerController->IsLocalController())
    {
        if (!!CrossHair)
            CrossHair->SetVisibility(ESlateVisibility::Visible);

        Owner->GetArms()->SetVisibility(false);      // ABP_Arms
        Owner->GetMesh()->SetVisibility(true);       // ABP_Character
        Owner->GetBackpack()->SetVisibility(false);

        Mesh->AttachToComponent(Owner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandSocketName);
    }

    CHelpers::GetComponent<UCWeaponComponent>(Owner)->OnWeaponAim_Arms_End.Broadcast();
}