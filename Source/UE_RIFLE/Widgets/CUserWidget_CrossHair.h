// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CUserWidget_CrossHair.generated.h"

/**
 *
 */
UCLASS()
class UE_RIFLE_API UCUserWidget_CrossHair : public UUserWidget
{
    GENERATED_BODY()

protected:
    // UE������ override�� ���־�� �Ѵ�, BP������ BeginPlay() ���� ȣ��
    void NativeOnInitialized() override;
    void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
    // ��� ������ ��������� ã�� �Լ�
    void FindAllWidgets(UWidget* ParentWidget);

private: // Border�ϰ� Alignment�� ���ڿ� �迭
    TArray<class UBorder*> Borders;
    TArray<FVector2D>      Alignments;
    // �̹��� �迭
    TArray<class UImage*> CrossHairImages;
    TArray<FLinearColor> OriginalImageColors;
private:
    float  Radius;
    float  MaxRadius;


private:
    enum class EDirection
    {
        Top = 0, Bottom, Left, Right, Max,
    };
public:
    void  UpdateSpreadRange(float InRadius, float InMaxRadis);

    // ���ο� �̹��� ���� ���� ���� �Լ�
    void SetCrossHairImagesColor(const FLinearColor& InColor);
    void ResetCrossHairImagesColor();

};

