// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/EffectComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h" // ���̾ư��� ���̺귯�� ����
#include "Animation/AnimInstance.h"

UEffectComponent::UEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEffectComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEffectComponent::PlayEffect(const FVector& Location, const FRotator& Rotation)
{
	if (HitEffect)
	{
		// ���̾ư��� �ý����� ����Ͽ� ����Ʈ ����
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, Location, Rotation);
	}
}

void UEffectComponent::PlaySound(const FVector& Location)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, Location);
	}
}

void UEffectComponent::PlayMontage(UAnimMontage* Montage)
{
	if (Montage)
	{
		AActor* Owner = GetOwner();
		if (Owner)
		{
			USkeletalMeshComponent* MeshComp = Owner->FindComponentByClass<USkeletalMeshComponent>();
			if (MeshComp && MeshComp->GetAnimInstance())
			{
				MeshComp->GetAnimInstance()->Montage_Play(Montage);
			}
		}
	}
}

