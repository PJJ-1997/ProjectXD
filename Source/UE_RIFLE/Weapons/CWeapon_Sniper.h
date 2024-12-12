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

public: // 1��Ī Aim
    void  Begin_Aim() override;
    void  End_Aim() override;

protected:
    // �������Ʈ���� ������ �� �ִ� ���� Ŭ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> AimWidgetClass;

private:
    // Begin_Aim���� �����ϰ� End_Aim���� ������ ���� �ν��Ͻ�
    UPROPERTY()
    UUserWidget* AimWidgetInstance;
};
