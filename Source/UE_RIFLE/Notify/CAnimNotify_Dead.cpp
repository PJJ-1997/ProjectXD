// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/CAnimNotify_Dead.h"
#include "GameFramework/Actor.h"
#include "Characters/CPlayer.h"

void UCAnimNotify_Dead::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (MeshComp && MeshComp->GetOwner())
    {
        // ĳ���� ���ʸ� �����ͼ� ACPlayer�� ĳ����
        ACPlayer* Player = Cast<ACPlayer>(MeshComp->GetOwner());

        if (Player)
        {
            // End_Dead �Լ� ȣ��
            Player->End_Dead();
        }
    }
}
