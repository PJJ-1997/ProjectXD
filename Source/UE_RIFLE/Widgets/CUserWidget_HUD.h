// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Header�� define�ϴ� ���� generated.h ���� ����

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Weapons/CWeaponComponent.h"   
#include "CUserWidget_HUD.generated.h"

// ABP , serWidget_HUD �� ���� ���鶧
// C++ �Ļ������� ������ ����, �Ϲ����� ������ ����
UCLASS()
class UE_RIFLE_API UCUserWidget_HUD : public UUserWidget
{
	GENERATED_BODY()
public: // version���� public�� ��þ��ϸ� private�� C����, 
        // BlueprintImplementableEvent : C���� �����Լ��� ����� �������� ����
    UFUNCTION(BlueprintImplementableEvent)
        void OnAutoFire();
    UFUNCTION(BlueprintImplementableEvent)
        void OffAutoFire();

public:
    UFUNCTION(BlueprintImplementableEvent)
        void UpdateMagazine(uint8 InCurr, uint8 InTotal);

    UFUNCTION(BlueprintImplementableEvent)
        void UpdateWeaponType(EWeaponType InType);

        UFUNCTION(BlueprintImplementableEvent)
        void UpdateHealth(float Health);

};
