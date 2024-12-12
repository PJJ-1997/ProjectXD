// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ICharacter.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UICharacter : public UInterface
{
    GENERATED_BODY()
};

/**
 *
 */
class UE_RIFLE_API IICharacter
{
    GENERATED_BODY()

public:  // 가상함수를 만들고 interface를 공유하는 모듈에서 override해서 사용(BP IDamage)
    virtual void End_Roll() {};  // 순수가상함수 X
    virtual void End_Hit() {};
    virtual void End_Dead() {};
public:
    void Create_DynamicMaterial(class ACharacter* InCharacter);
    void Change_Color(class ACharacter* InCharacter, FLinearColor InColor);


};
