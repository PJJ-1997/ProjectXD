// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy.h"
#include "Global.h"
#include "CAnimInstance.h"
#include "CAnimInstance_Arms.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Component/EffectComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Weapons/CWeaponComponent.h"

// Sets default values
AEnemy::AEnemy()
{
    PrimaryActorTick.bCanEverTick = true;

   // // ķ�� ������Ʈ�� ĳ���Ϳ��� ��ӹ޾��ִ� ������
   // CHelpers::CreateComponent<USpringArmComponent>(this, &SpringArm, "SpringArm", GetMesh());
   // CHelpers::CreateComponent<UCameraComponent>(this, &Camera, "Camera", SpringArm);
   // CHelpers::CreateComponent<UStaticMeshComponent>(this, &Backpack, "Backpack", GetMesh(), "Backpack");
   // CHelpers::CreateComponent<USkeletalMeshComponent>(this, &Arms, "Arms", Camera);
   //
   // // �츮�� ���� ������Ʈ�� Attach���� �ʴ´�.
   // CHelpers::CreateActorComponent<UCWeaponComponent>(this, &Weapon, "Weapon");
   //
   // // �޽�
   // USkeletalMesh* mesh; //Game/Character/Mesh/SK_Mannequin.SK_Mannequin
   // {
   //     CHelpers::GetAsset<USkeletalMesh>(&mesh,
   //         "SkeletalMesh'/Game/ParagonMinions/Characters/Minions/Dusk_Minions/Meshes/Minion_Lane_Melee_Core_Dusk.Minion_Lane_Melee_Core_Dusk'");// ���۷��� �����ؼ� �־��� �޽� �ּ� �־��ֱ�
   //     GetMesh()->SetSkeletalMesh(mesh);
   //     GetMesh()->SetRelativeLocation(FVector(0, 0, -88));
   //     // C������ ���� : PitchYawRoll, BP������ ���� : RollPitchYaw   
   //     GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
   //
   //     // ABP�� ���
   //     TSubclassOf<UCAnimInstance> animInstance; // TSubClassOf�� ������������ ���ǵǾ�����
   //     CHelpers::GetClass< UCAnimInstance>(&animInstance, "AnimBlueprint'/Game/AI/ABP_AI_Minion_Melee.ABP_AI_Minion_Melee_C'");
   //     GetMesh()->SetAnimClass(animInstance);
   // } 
   // CHelpers::CreateActorComponent<UEffectComponent>(this, &Effect, "Effect");
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

