// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/CWeapon.h"
#include "CWeapon_AK47.generated.h"

UCLASS()
class UE_RIFLE_API ACWeapon_AK47 : public ACWeapon
{
	GENERATED_BODY()
	
public:
    ACWeapon_AK47();

protected:
    virtual void BeginPlay() override;
    virtual void UnEquip() override;

private:
    UPROPERTY(VisibleAnywhere)
        class UStaticMeshComponent* Sight;  // 조준경
private:
    UPROPERTY(EditDefaultsOnly, Category = "Equip")
        FName LeftHandSocketName;           // AK47은 장착시 왼손에 무기를 

public:
    void  Begin_Equip() override;
    void  End_Equip() override;
    void  SwitchToRightHand();

public: // 1인칭 Aim
    void  Begin_Aim() override;
    void  End_Aim() override;
};
