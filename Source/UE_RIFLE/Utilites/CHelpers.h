// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"



#define  CN(x) {if(x==nullptr) return;}
#define  CheckTrue(x)  { if(x==true) return;}
#define  CheckTrueResult(x,y)  { if(x==true) return y;}

#define  CheckFalse(x)  { if(x==false) return;}
#define  CheckFalseResult(x,y)  { if(x==false) return y;}

#define  CheckNull(x) { if(x==nullptr) return;}
#define  CheckNullResult(x,y)  { if(x==nullptr) return y;}

#define  CreateTextRender() \
{ \
	CHelpers::CreateComponent<UTextRenderComponent>(this, &Text, "Text", Root);\
	Text->SetRelativeLocation(FVector(0, 0, 100));\
	Text->SetRelativeRotation(FRotator(0, 180, 0));\
	Text->SetWorldScale3D(FVector(2));\
	Text->TextRenderColor = FColor::Red;\
	Text->HorizontalAlignment = EHorizTextAligment::EHTA_Center;\
	Text->Text = FText::FromString(GetName().Replace(L"Default__", L""));\
}\

class UE_RIFLE_API CHelpers
{
public:
    template<typename T>
    static void CreateComponent(AActor* InActor, T** OutComponent, FName InName, USceneComponent* InParent = nullptr, FName InSocketName = NAME_None)
    {
        *OutComponent = InActor->CreateDefaultSubobject<T>(InName);
        if (!!InParent)
        {
			// 생성자에서만 SetupAttachment()가 가능하다
            // 소켓명에 _가 있으면  _를 space문자로 만들기 때문에 주의를 요한다
            (*OutComponent)->SetupAttachment(InParent,InSocketName);
            return;
        }
        InActor->SetRootComponent(*OutComponent);
    }

	template<typename T>
	static void CreateActorComponent(AActor* InActor, T** OutComponent, FName InName)
	{
		*OutComponent = InActor->CreateDefaultSubobject<T>(InName);		
	}

    template<typename T>
    static void GetAsset(T** OutObject, FString InPath)
    {
        ConstructorHelpers::FObjectFinder<T>  asset(*InPath);
        *OutObject = asset.Object;
    }

    template<typename T>
    static void GetAssetDynamic(T** OutObject, FString InPath)
    {
        *OutObject = Cast<T>(StaticLoadObject(T::StaticClass(), nullptr, *InPath));
    }

	template<typename T>
	static void GetClass(TSubclassOf<T>* OutObject, FString InPath)
	{
		ConstructorHelpers::FClassFinder<T> asset(*InPath);
		*OutObject = asset.Class;
	}

	template<typename T>
	static T* FindActor(UWorld* InWorld)
	{
		for (AActor* actor : InWorld->GetCurrentLevel()->Actors)
		{
			// actor가 Null인걸 체크하는 이유는 pendingKill
			if (!!actor && actor->IsA<T>()) // !!actor(액터가 NULL이 아니고),AC04_Trigger와 상속관계가 맞다면
			{
				return Cast<T>(actor);
			}
		}
		return nullptr;
	}

	template<typename T>
	static void FindActors(UWorld* InWorld, TArray<T*>& OutActors)
	{
		OutActors.Empty();
		for (AActor* actor : InWorld->GetCurrentLevel()->Actors)
		{
			if (!!actor  && actor->IsA<T>())
				OutActors.Add(Cast<T>(actor));
		}
	}

	// 장착관련
	static void AttachTo(AActor* InActor, USceneComponent* InParent, const FName& InSocketName)
	{
		InActor->AttachToComponent(InParent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), InSocketName);
	}

    template<typename T>
    static T*  GetComponent(AActor* InActor)
    {

        return Cast<T>(InActor->GetComponentByClass(T::StaticClass()));


    }



};
