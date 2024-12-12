// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CUserWidget_CrossHair.h"
#include "Global.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"

void UCUserWidget_CrossHair::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    UCanvasPanel* panel = Cast<UCanvasPanel>(WidgetTree->RootWidget);
    CheckNull(panel);

    // 모든 하위 위젯을 재귀적으로 검색하여 Border와 Image를 찾습니다.
    FindAllWidgets(panel);

    // Top -> Bottom-> Right->Left
    TArray<UWidget*>  widgets = panel->GetAllChildren();
    for (UWidget* widget : widgets)
    {
        UBorder* border = Cast<UBorder>(widget); // T,B,R,L
        if (!!border)
        {
            // border
            //   ---> CanvasSlot
            Borders.Add(border);
            UCanvasPanelSlot* slot = UWidgetLayoutLibrary::SlotAsCanvasSlot(border);
            Alignments.Add(slot->GetAlignment());
        }

        // 이미지 위젯 추출
        if (UImage* img = Cast<UImage>(widget))
        {
            CrossHairImages.Add(img);
            // 초기 원본 색상 저장
            OriginalImageColors.Add(img->ColorAndOpacity);
        }

    }

    if (0)
    {
        // BP --> Length(X)
        for (int32 i = 0; i < Borders.Num(); i++)
        {
            CLog::Log(Borders[i]->GetName());
            CLog::Log(Alignments[i].ToString());
        }

    }





}
void UCUserWidget_CrossHair::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    float  minimum = 0.0f;
    float  maximum = 0.0f;

    // alignment조정
    for (int32 i = 0; i < (int32)EDirection::Max; i++)
    {
        switch ((EDirection)i)
        {
        case EDirection::Top:
            minimum = Alignments[i].Y;
            maximum = Alignments[i].Y + MaxRadius;
            break;

        case EDirection::Bottom:
            minimum = Alignments[i].Y;
            maximum = Alignments[i].Y - MaxRadius;
            break;

        case EDirection::Left:
            minimum = Alignments[i].X;
            maximum = Alignments[i].X + MaxRadius;
            break;

        case EDirection::Right:
            minimum = Alignments[i].X;
            maximum = Alignments[i].X - MaxRadius;
            break;

        }


        // Lerp함수
        // float Lerp(float start, float end, float amount)
        //{
        //    return start + (end - start)*amount;
        //}

        float value = FMath::Lerp<float>(minimum, maximum, Radius);

        UCanvasPanelSlot* slot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Borders[i]);
        switch ((EDirection)i)
        {
        case EDirection::Top:
        case EDirection::Bottom:
            slot->SetAlignment(FVector2D(Alignments[i].X, value));
            break;
        case EDirection::Left:
        case EDirection::Right:
            slot->SetAlignment(FVector2D(value, Alignments[i].Y));
            break;
        }

    }
}

void UCUserWidget_CrossHair::UpdateSpreadRange(float InRadius, float InMaxRadius)
{
    Radius = InRadius;
    MaxRadius = InMaxRadius; //선형보간의 값
}


void UCUserWidget_CrossHair::SetCrossHairImagesColor(const FLinearColor& InColor)
{
    for (UImage* Img : CrossHairImages)
    {
        if (Img)
        {
            Img->SetBrushTintColor(FLinearColor(InColor));
        }
    }
}

void UCUserWidget_CrossHair::ResetCrossHairImagesColor()
{
    for (int32 i = 0; i < CrossHairImages.Num(); i++)
    {
        if (CrossHairImages[i])
        {
            CrossHairImages[i]->SetBrushTintColor(OriginalImageColors[i]);
        }
    }
}


// 재귀적으로 모든 하위 위젯을 검색하는 함수
void UCUserWidget_CrossHair::FindAllWidgets(UWidget* ParentWidget)
{
    if (!ParentWidget)
        return;

    // Border 확인
    if (UBorder* border = Cast<UBorder>(ParentWidget))
    {
        Borders.Add(border);
        if (UCanvasPanelSlot* slot = UWidgetLayoutLibrary::SlotAsCanvasSlot(border))
        {
            Alignments.Add(slot->GetAlignment());
        }
    }

    // Image 확인
    if (UImage* img = Cast<UImage>(ParentWidget))
    {
        CrossHairImages.Add(img);
        OriginalImageColors.Add(img->ColorAndOpacity);
    }

    // PanelWidget인 경우 자식들을 재귀 탐색
    if (UPanelWidget* Panel = Cast<UPanelWidget>(ParentWidget))
    {
        const int32 Count = Panel->GetChildrenCount();
        for (int32 i = 0; i < Count; i++)
        {
            UWidget* Child = Panel->GetChildAt(i);
            FindAllWidgets(Child);
        }
    }
}
