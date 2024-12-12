// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CWeapon_Pistol.h"
#include  "Global.h"
#include  "CWeaponComponent.h"
#include  "Animation/AnimMontage.h"
#include "Camera/CameraShakeBase.h"
#include "Widgets/CUserWidget_CrossHair.h"
#include "Weapons/CMagazine.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Characters/CPlayer.h"

ACWeapon_Pistol::ACWeapon_Pistol()
{
    // 에셋을 가져오고
    USkeletalMesh* mesh;
    CHelpers::GetAsset<USkeletalMesh>(&mesh, "SkeletalMesh'/Game/MilitaryWeapSilver/Weapons/Pistols_A.Pistols_A'");
    Mesh->SetSkeletalMesh(mesh);


    // 장착관련 데이터
    {
        HolsterSocketName = NAME_None;
        CHelpers::GetAsset<UAnimMontage>(&EquipMontage, "AnimMontage'/Game/Character/Montages/Pistol_Equip_Montage.Pistol_Equip_Montage'");
        EquipMontage_PlayRate = 1.5f;
        RightHandSocketName = "Pistol_RightHand";
    }

    // Aim
    {
        BaseData.TargetArmLength = 200;
        BaseData.SocketOffset = FVector(0, 50, 15);
        BaseData.FieldOfView = 90;

        AimData.TargetArmLength = 30;
        AimData.SocketOffset = FVector(55, 0, 10);
        AimData.FieldOfView = 55;

        LeftHandLocation = FVector(-10.0f, 12.5f, 0.0f);
    }

    // Fire
    {
        CHelpers::GetClass<UCameraShakeBase>(&CameraShakeClass, "Blueprint'/Game/Weapons/BP_CameraShake_Pistol.BP_CameraShake_Pistol_C'");
    }

    // UI
    {
        CHelpers::GetClass<UCUserWidget_CrossHair>(&CrossHairClass, "WidgetBlueprint'/Game/Widgets/WB_CrossHair.WB_CrossHair_C'");
    }


    //Magazine
    {
        CHelpers::GetAsset<UAnimMontage>(&ReloadMontage, "AnimMontage'/Game/Character/Montages/Pistol_Reload_Montage.Pistol_Reload_Montage'");
        ReloadMontage_PlayRate = 1.5f;
        MagazineBoneName = "b_gun_mag";
        MagazineSocketName = "Pistol_Magazine";
        CHelpers::GetClass<ACMagaZine>(&MagazineClass, "Blueprint'/Game/Weapons/BP_CMagaZine_Pistol.BP_CMagaZine_Pistol_C'");
    }

    // Arms
    {
        ArmsMeshTransform.SetLocation(FVector(-35, 5.1f, -156.6));
        ArmsMeshTransform.SetRotation(FQuat(FRotator(0, -4.8f, 0)));

        ArmsLeftHandTransform.SetLocation(FVector(0, 11, 0));
        ArmsLeftHandTransform.SetRotation(FQuat(FRotator(0, 180, 180)));
    }
    WeaponName = "Pistol";
}

void ACWeapon_Pistol::BeginPlay()
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

void ACWeapon_Pistol::Begin_Equip()
{
    Super::Begin_Equip();
    Mesh->SetVisibility(true);
    Owner->GetArms()->SetRelativeTransform(ArmsMeshTransform);
}

void ACWeapon_Pistol::End_Equip()
{
    Super::End_Equip();
}

void ACWeapon_Pistol::Begin_Aim()
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

        CHelpers::AttachTo(this, Owner->GetArms(), RightHandSocketName);
    }

    CHelpers::GetComponent<UCWeaponComponent>(Owner)->OnWeaponAim_Arms_Begin.Broadcast(this);
}

void ACWeapon_Pistol::End_Aim()
{
    Super::End_Aim();

    APlayerController* OwnerPlayerController = Cast<APlayerController>(Owner->GetController());
    if (OwnerPlayerController && OwnerPlayerController->IsLocalController())
    {
        if (!!CrossHair)
            CrossHair->SetVisibility(ESlateVisibility::Visible);

        Owner->GetMesh()->SetVisibility(true);       // ABP_Character
        Owner->GetArms()->SetVisibility(false);      // ABP_Arms
        Owner->GetBackpack()->SetVisibility(true);

        CHelpers::AttachTo(this, Owner->GetMesh(), RightHandSocketName);
    }

    CHelpers::GetComponent<UCWeaponComponent>(Owner)->OnWeaponAim_Arms_End.Broadcast();
}
