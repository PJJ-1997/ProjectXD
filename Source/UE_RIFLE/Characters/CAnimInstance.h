// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Weapons/CWeaponComponent.h"
#include "CAnimInstance.generated.h"


UCLASS()
class UE_RIFLE_API UCAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
    UCAnimInstance();
protected:
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    float Speed;

    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    float Direction;

    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    float Pitch;


    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        bool bIsInAir;


protected:
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapons")
        EWeaponType WeaponType = EWeaponType::Max;
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapons")
        bool bInAim = false;
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapons")
        bool bFiring = false;
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapons")
        bool bUseHandIK = false;
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapons")
        FVector LeftHandLocation;   // ���⿡ ���� ��ġ�� �ٲ�




public:
    UFUNCTION()
    void OnWeponTypeChanged(EWeaponType InPrevType, EWeaponType InNewType);

public:
	// UE������ override�� ���־�� �Ѵ�, BP������ BeginPlay() ���� ȣ��
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
private:
	// Owner,Parent ���� ���� �𸮾������� ���ǵ��ִ� ��찡�־ ���X(���־��´ܾ��)
	class ACharacter* Character; 
    class UCWeaponComponent* Weapon;
private:
    FRotator PrevRotation;
};
