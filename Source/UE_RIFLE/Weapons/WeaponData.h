// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WeaponData.generated.h"

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float RecoilAngle;
    // 상하 반동 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float RecoilRate;

    // 좌우 반동 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float RecoilHorizontalRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float SpreadSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float MaxSpreadAlignment;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float AutoFireInterval;

    UPROPERTY(EditDefaultsOnly, Category = "Magazine")
    uint8 MaxMagazineCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    int32 MaxAmmo;

};

class UE_RIFLE_API WeaponData
{

};
