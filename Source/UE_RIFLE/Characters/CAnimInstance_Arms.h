// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Weapons/CWeaponComponent.h"
#include "CAnimInstance_Arms.generated.h"

/**
 * 
 */
UCLASS()
class UE_RIFLE_API UCAnimInstance_Arms : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapons")
        EWeaponType WeaponType = EWeaponType::Max;
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapons")
        FTransform ArmsLeftHandTransform;

public:
    UFUNCTION()
        void OnWeponTypeChanged(EWeaponType InPrevType, EWeaponType InNewType);
public:
    // UE에서는 override를 해주어야 한다, BP에서의 BeginPlay() 전의 호출
    virtual void NativeBeginPlay() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
private:
    // Owner,Parent 같은 경우는 언리얼엔진에서 정의되있는 경우가있어서 사용X(자주쓰는단어라)
    class ACharacter* OwnerCharacter;
    class UCWeaponComponent* Weapon;
};
