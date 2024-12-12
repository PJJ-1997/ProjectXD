// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/DamageInterface.h"
#include "Component/EffectComponent.h"
#include "CAI_Minion_Base.generated.h"

UCLASS()
class UE_RIFLE_API ACAI_Minion_Base : public ACharacter//, public IDamageInterface
{
	GENERATED_BODY()

public:
	ACAI_Minion_Base();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
