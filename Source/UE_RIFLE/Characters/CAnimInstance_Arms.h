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
    // UE������ override�� ���־�� �Ѵ�, BP������ BeginPlay() ���� ȣ��
    virtual void NativeBeginPlay() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
private:
    // Owner,Parent ���� ���� �𸮾������� ���ǵ��ִ� ��찡�־ ���X(���־��´ܾ��)
    class ACharacter* OwnerCharacter;
    class UCWeaponComponent* Weapon;
};
