// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Header에 define하는 경우는 generated.h 위에 선언

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Weapons/CWeaponComponent.h"   
#include "CUserWidget_HUD.generated.h"

// ABP , serWidget_HUD 는 블프 만들때
// C++ 파생블프로 만들지 말고, 일반적인 블프로 생성
UCLASS()
class UE_RIFLE_API UCUserWidget_HUD : public UUserWidget
{
	GENERATED_BODY()
public: // version별로 public을 명시안하면 private로 C에서, 
        // BlueprintImplementableEvent : C에서 가상함수를 만들고 블프에서 구현
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
