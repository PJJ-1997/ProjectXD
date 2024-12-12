// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/ICharacter.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// Add default functionality here for any IICharacter functions that are not pure virtual.

void IICharacter::Create_DynamicMaterial(ACharacter* InCharacter)
{
    for (int32 i = 0; i < InCharacter->GetMesh()->GetMaterials().Num(); i++)
    {
        UMaterialInterface* material = InCharacter->GetMesh()->GetMaterials()[i];
        InCharacter->GetMesh()->SetMaterial(i, UMaterialInstanceDynamic::Create(material, InCharacter));
    }

}

void IICharacter::Change_Color(ACharacter* InCharacter, FLinearColor InColor)
{
    for (UMaterialInterface* material : InCharacter->GetMesh()->GetMaterials())
    {
        UMaterialInstanceDynamic* instance = Cast<UMaterialInstanceDynamic>(material);
        if (!!instance)
            instance->SetVectorParameterValue("BodyColor", InColor);
    }

}