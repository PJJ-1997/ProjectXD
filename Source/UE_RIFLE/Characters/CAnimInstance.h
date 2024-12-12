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
        FVector LeftHandLocation;   // 무기에 따라 위치가 바뀜




public:
    UFUNCTION()
    void OnWeponTypeChanged(EWeaponType InPrevType, EWeaponType InNewType);

public:
	// UE에서는 override를 해주어야 한다, BP에서의 BeginPlay() 전의 호출
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
private:
	// Owner,Parent 같은 경우는 언리얼엔진에서 정의되있는 경우가있어서 사용X(자주쓰는단어라)
	class ACharacter* Character; 
    class UCWeaponComponent* Weapon;
private:
    FRotator PrevRotation;
};
