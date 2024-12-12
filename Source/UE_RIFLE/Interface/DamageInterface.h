// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageInterface.generated.h"

UINTERFACE(MinimalAPI)
class UDamageInterface : public UInterface
{
	GENERATED_BODY()
};

class UE_RIFLE_API IDamageInterface
{
	GENERATED_BODY()

public:
	virtual void  TakeDamage(FHitResult HitResult, float DamageAmount) = 0;
};
