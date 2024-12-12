// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CAnimNotify_Reload.generated.h"

UENUM()
enum class EReloadActionType : uint8
{
    Eject, Spawn, Load, End,
};
UCLASS()
class UE_RIFLE_API UCAnimNotify_Reload : public UAnimNotify
{
    GENERATED_BODY()

private:
    UPROPERTY(EditAnywhere, Category = "Action")
        EReloadActionType ActionType;
public:
    FString GetNotifyName_Implementation() const override;
    virtual void Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;
};
