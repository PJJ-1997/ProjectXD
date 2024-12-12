// Copyright Epic Games, Inc. All Rights Reserved.


#include "CGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Global.h"

//ACGameMode::ACGameMode()
//{
//	CHelpers::GetClass<APawn> (&DefaultPawnClass, "/Script/Engine.Blueprint'/Game/Player/BP_CPlayer_Base.BP_CPlayer_Base_C'");
//	DefaultPawnClass = nullptr;
//}
//
//void ACGameMode::PostLogin(APlayerController* NewPlayer)
//{
//	Super::PostLogin(NewPlayer);
//
//	if (GameState)
//	{
//		// Print out the number of players
//		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
//		if (GEngine)
//		{
//			GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Yellow, FString::Printf(TEXT("Players in Game : %d"), NumberOfPlayers));
//		}
//
//		APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();
//		if (PlayerState)
//		{
//			FString PlayerName = PlayerState->GetPlayerName();
//
//			if (GEngine)
//			{
//				GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Cyan, FString::Printf(TEXT("%s has joined the game"), *PlayerName));
//			}
//		}
//	}
//
//	if (NewPlayer->IsLocalController()) // 로컬 플레이어(호스트) 확인
//	{
//		// 호스트를 위한 폰 스폰
//		if (HostPawnClass)
//		{
//			APawn* HostPawn = GetWorld()->SpawnActor<APawn>(HostPawnClass, FVector::ZeroVector, FRotator::ZeroRotator);
//			if (HostPawn)
//			{
//				NewPlayer->Possess(HostPawn);
//			}
//		}
//	}
//	else
//	{
//		// 클라이언트를 위한 폰 스폰
//		if (ClientPawnClass)
//		{
//			APawn* ClientPawn = GetWorld()->SpawnActor<APawn>(ClientPawnClass, FVector::ZeroVector, FRotator::ZeroRotator);
//			if (ClientPawn)
//			{
//				NewPlayer->Possess(ClientPawn);
//			}
//		}
//	}
//}
//
//void ACGameMode::Logout(AController* Exiting)
//{
//	Super::Logout(Exiting);
//
//	if (GameState)
//	{
//		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
//		if (GEngine)
//		{
//			GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Yellow, FString::Printf(TEXT("Players in Game : %d"), NumberOfPlayers - 1));
//		}
//
//		APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>();
//		if (PlayerState)
//		{
//			FString PlayerName = PlayerState->GetPlayerName();
//
//			if (GEngine)
//			{
//				GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Cyan, FString::Printf(TEXT("%s has exited the game"), *PlayerName));
//			}
//		}
//	}
//}