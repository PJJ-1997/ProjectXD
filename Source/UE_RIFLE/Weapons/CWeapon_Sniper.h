// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/CWeapon.h"
#include "Blueprint/UserWidget.h"
#include "CWeapon_Sniper.generated.h"

/**
 * 
 */
UCLASS()
class UE_RIFLE_API ACWeapon_Sniper : public ACWeapon
{
	GENERATED_BODY()

public:
    ACWeapon_Sniper();

private:
    USkeletalMesh* CachedMesh;

protected:
    virtual void BeginPlay() override;
public:
    void  Begin_Equip() override;
    void  End_Equip() override;

public: // 1인칭 Aim
    void  Begin_Aim() override;
    void  End_Aim() override;

protected:
    // 블루프린트에서 설정할 수 있는 위젯 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> AimWidgetClass;

private:
    // Begin_Aim에서 생성하고 End_Aim에서 제거할 위젯 인스턴스
    UPROPERTY()
    UUserWidget* AimWidgetInstance;
};
