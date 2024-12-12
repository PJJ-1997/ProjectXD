// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CWeapon_Sniper.h"
#include "Global.h"
#include "CWeaponComponent.h"
#include "Animation/AnimMontage.h"
#include "Camera/CameraShakeBase.h"
#include "Widgets/CUserWidget_CrossHair.h"
#include "Weapons/CMagazine.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Characters/CPlayer.h"

ACWeapon_Sniper::ACWeapon_Sniper()
{
    // ���̷�Ż �޽��� ���۷����� ��������
    CHelpers::GetAsset<USkeletalMesh>(&CachedMesh, "SkeletalMesh'/Game/Sniper/SK_Lyra_Sniper.SK_Lyra_Sniper'");

    // ���� ���� ������
    HolsterSocketName = "Rifle_AR4_Holster";
    CHelpers::GetAsset<UAnimMontage>(&EquipMontage, "AnimMontage'/Game/Character/Montages/Rifle_Equip_AR4_Montage.Rifle_Equip_AR4_Montage'");
    EquipMontage_PlayRate = 1.5f;
    RightHandSocketName = "Rifle_AR4_RightHand";

    // Aim ������
    BaseData.TargetArmLength = 200;
    BaseData.SocketOffset = FVector(0, 50, 15);
    BaseData.FieldOfView = 90;

    AimData.TargetArmLength = 200;
    AimData.SocketOffset = FVector(0, 50, 15);
    AimData.FieldOfView = 90;

    LeftHandLocation = FVector(-32.5f, 15.5f, 4.0f);

    // ��Ÿ ������ �ʱ�ȭ
    WeaponName = "Sniper";
    Mesh->SetVisibility(false);
}

void ACWeapon_Sniper::BeginPlay()
{
    Super::BeginPlay();

    if (CachedMesh)
    {
        Mesh->SetSkeletalMesh(CachedMesh);
    }

    Mesh->SetVisibility(false);

    FString ContextString;
    FWeaponData* WeaponDataRow = WeaponDataTable->FindRow<FWeaponData>(WeaponName, ContextString);
    if (WeaponDataRow)
    {
        WeaponData = *WeaponDataRow;
    }
}

void ACWeapon_Sniper::Begin_Equip()
{
    Super::Begin_Equip();
    Mesh->SetVisibility(true);

    if (RightHandSocketName.IsValid())
        CHelpers::AttachTo(this, Owner->GetMesh(), RightHandSocketName);

    Owner->GetArms()->SetRelativeTransform(ArmsMeshTransform);
}

void ACWeapon_Sniper::End_Equip()
{
    Super::End_Equip();
}

void ACWeapon_Sniper::Begin_Aim()
{
    Super::Begin_Aim();

    APlayerController* OwnerPlayerController = Cast<APlayerController>(Owner->GetController());
    if (OwnerPlayerController && OwnerPlayerController->IsLocalController())
    {
        if (!!CrossHair)
            CrossHair->SetVisibility(ESlateVisibility::Hidden);
        {
            if (Owner)
            {
                ACharacter* CharacterOwner = Cast<ACharacter>(Owner);
                if (CharacterOwner)
                {
                    UCameraComponent* CameraComponent = CharacterOwner->FindComponentByClass<UCameraComponent>();
                    if (CameraComponent)
                    {
                        CameraComponent->SetFieldOfView(20.0f);
                        CameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
                    }
                }

                // ������ ȭ�鿡 ǥ��
                if (AimWidgetClass && !AimWidgetInstance)
                {
                    APlayerController* PlayerController = Cast<APlayerController>(CharacterOwner->GetController());
                    if (PlayerController)
                    {
                        AimWidgetInstance = CreateWidget<UUserWidget>(PlayerController, AimWidgetClass);
                        if (AimWidgetInstance)
                        {
                            AimWidgetInstance->AddToViewport();
                        }
                    }
                }
            }
        }
    }
}

void ACWeapon_Sniper::End_Aim()
{
    Super::End_Aim();

    APlayerController* OwnerPlayerController = Cast<APlayerController>(Owner->GetController());
    if (OwnerPlayerController && OwnerPlayerController->IsLocalController())
    {
        if (!!CrossHair)
            CrossHair->SetVisibility(ESlateVisibility::Visible);
        {
            if (Owner)
            {
                ACharacter* CharacterOwner = Cast<ACharacter>(Owner);
                if (CharacterOwner)
                {
                    UCameraComponent* CameraComponent = CharacterOwner->FindComponentByClass<UCameraComponent>();
                    if (CameraComponent)
                    {
                        CameraComponent->SetFieldOfView(90.0f);
                        CameraComponent->SetRelativeLocation(FVector(-30.0f, 0.0f, 0.0f));
                    }
                }

                // ������ ȭ�鿡�� ����
                if (AimWidgetInstance)
                {
                    AimWidgetInstance->RemoveFromParent();
                    AimWidgetInstance = nullptr; // �ν��Ͻ��� nullptr�� �ʱ�ȭ�Ͽ� ���� Begin_Aim���� �ٽ� ������ �� �ֵ��� ��
                }
            }
        }
    }
}
