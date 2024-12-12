// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/CAnimNotify_Dead.h"
#include "GameFramework/Actor.h"
#include "Characters/CPlayer.h"

void UCAnimNotify_Dead::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (MeshComp && MeshComp->GetOwner())
    {
        // 캐릭터 오너를 가져와서 ACPlayer로 캐스팅
        ACPlayer* Player = Cast<ACPlayer>(MeshComp->GetOwner());

        if (Player)
        {
            // End_Dead 함수 호출
            Player->End_Dead();
        }
    }
}
