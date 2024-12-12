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
    // UE에서는 override를 해주어야 한다, BP에서의 BeginPlay() 전의 호출
    void NativeOnInitialized() override;
    void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
    // 모든 위젯을 재귀적으로 찾는 함수
    void FindAllWidgets(UWidget* ParentWidget);

private: // Border하고 Alignment를 저자왈 배열
    TArray<class UBorder*> Borders;
    TArray<FVector2D>      Alignments;
    // 이미지 배열
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

    // 새로운 이미지 색상 변경 관련 함수
    void SetCrossHairImagesColor(const FLinearColor& InColor);
    void ResetCrossHairImagesColor();

};

