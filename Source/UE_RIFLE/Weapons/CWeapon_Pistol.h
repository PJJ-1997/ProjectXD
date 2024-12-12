// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/CWeapon.h"
#include "CWeapon_Pistol.generated.h"

UCLASS()
class UE_RIFLE_API ACWeapon_Pistol : public ACWeapon
{
	GENERATED_BODY()
	
public:
    ACWeapon_Pistol();

protected:
    virtual void BeginPlay() override;

public:
    void  Begin_Equip() override;
    void  End_Equip() override;


public: // 1��Ī Aim
    void  Begin_Aim() override;
    void  End_Aim() override;

};