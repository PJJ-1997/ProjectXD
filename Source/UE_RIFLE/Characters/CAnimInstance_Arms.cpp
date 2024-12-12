// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CAnimInstance_Arms.h"

#include "Global.h"
#include "GameFramework/Character.h"
#include "CPlayer.h"
#include "Weapons/CWeaponComponent.h"

void UCAnimInstance_Arms::NativeBeginPlay()
{
    Super::NativeBeginPlay();
    OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner()); // 블프에서의 케스팅(변수세팅)
    CheckNull(OwnerCharacter);

    UActorComponent* comp = OwnerCharacter->GetComponentByClass(UCWeaponComponent::StaticClass());
    Weapon = Cast<UCWeaponComponent>(comp);

    // 함수바인딩

    if (!!Weapon)
        Weapon->OnWeaponTypeChanged.AddDynamic(this, &UCAnimInstance_Arms::OnWeponTypeChanged);
}

void UCAnimInstance_Arms::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    //	if (OwnerCharacter == nullptr)  
    //		return; --> CHelpers Define문
    CheckNull(OwnerCharacter);

    ArmsLeftHandTransform = Weapon->GetArmsLeftHandTransform();


}
void UCAnimInstance_Arms::OnWeponTypeChanged(EWeaponType InPrevType, EWeaponType InNewType)
{
    WeaponType = InNewType;

}