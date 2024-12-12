// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CStateComponent.generated.h"

UENUM()
enum class EStateType : uint8
{
	Idle = 0, Roll, Hit, Dead,
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStateTypeChanged, EStateType, InPrevType, EStateType, InNewType);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE_RIFLE_API UCStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCStateComponent();

public:
	void SetIdleMode();
	void SetRollMode();
	void SetHitMode();
	void SetDeadMode();

private:
	void ChangeType(EStateType InType);   // Binding --> ABP,Player,Enemey
public:
	FStateTypeChanged  OnStateTypeChanged;

private:
	EStateType Type = EStateType::Idle;

private:
	class ACharacter* OwnerCharacter;  // 각 콤포넌트마다 생성시 변수를 저장

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	FORCEINLINE bool IsIdleMode() { return Type == EStateType::Idle; }
	FORCEINLINE bool IsHitMode() { return Type == EStateType::Hit; }
	FORCEINLINE bool IsRollMode() { return Type == EStateType::Roll; }
	FORCEINLINE bool IsDeadMode() { return Type == EStateType::Dead; }
};
