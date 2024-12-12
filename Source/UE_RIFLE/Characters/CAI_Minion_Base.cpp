// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CAI_Minion_Base.h"
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
ACAI_Minion_Base::ACAI_Minion_Base()
{
	PrimaryActorTick.bCanEverTick = true;

 // // 메쉬
 //  USkeletalMesh* mesh; //Game/Character/Mesh/SK_Mannequin.SK_Mannequin
 //  {
 //      CHelpers::GetAsset<USkeletalMesh>(&mesh,
 //          "SkeletalMesh'/Game/ParagonMinions/Characters/Minions/Dusk_Minions/Meshes/Minion_Lane_Melee_Core_Dusk.Minion_Lane_Melee_Core_Dusk'");// 레퍼런스 복사해서 넣어줄 메시 주소 넣어주기
 //      GetMesh()->SetSkeletalMesh(mesh);
 //      GetMesh()->SetRelativeLocation(FVector(0, 0, -88));
 //      // C에서의 방향 : PitchYawRoll, BP에서의 방향 : RollPitchYaw   
 //      GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
 // 
 //      // ABP를 등록
 //      TSubclassOf<UCAnimInstance> animInstance; // TSubClassOf는 포인터형으로 정의되어있음
 //      CHelpers::GetClass< UCAnimInstance>(&animInstance, "AnimBlueprint'/Game/AI/ABP_AI_Minion_Melee.ABP_AI_Minion_Melee_C'");
 //      GetMesh()->SetAnimClass(animInstance);
 //  } 
 //  CHelpers::CreateActorComponent<UEffectComponent>(this, &Effect, "Effect");
}

void ACAI_Minion_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACAI_Minion_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
