// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NiagaraSystem.h" // 나이아가라 시스템 헤더 추가
#include "EffectComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_RIFLE_API UEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEffectComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	void PlayEffect(const FVector& Location, const FRotator& Rotation);
	void PlaySound(const FVector& Location);
	void PlayMontage(UAnimMontage* Montage);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* HitEffect; // UParticleSystem -> UNiagaraSystem 변경

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* HitSound;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* HitMontage;
};
