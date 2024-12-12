// Fill out your copyright notice in the Description page of Project Settings.

#include "Utilites/CLog.h"
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY_STATIC(GP,Display,All)

void CLog::Log(int32 InValue)
{
   // GLog->Log("GP", ELogVerbosity::Display, FString::FromInt(InValue));
    UE_LOG(GP, Display, L"%d", InValue);
}

void CLog::Log(float InValue)
{
    UE_LOG(GP, Display, L"%f", InValue);
}

void CLog::Log(const FString & InValue)
{
    UE_LOG(GP, Display, L"%s", *InValue);
}

void CLog::Log(const FVector & InValue)
{
    UE_LOG(GP, Display, L"%s", *InValue.ToString());   // 1.0,0.2,0.5
}

void CLog::Log(const FRotator & InValue)
{
    UE_LOG(GP, Display, L"%s", *InValue.ToString());   // 1.0,0.2,0.5
}

void CLog::Log(const UObject* InValue)
{
    FString str;

    if (InValue != nullptr)   // InValue != nullptr
        str.Append(InValue->GetName());

    str.Append(!!InValue ? "Not Null" : "Null");

    UE_LOG(GP, Display, L"%s", *str);

}

void CLog::Log(const FString & InFileName, const FString & InFuncName, int32 InLineNumber)
{
    // C://a/a.cpp
    int32 index  = 0;
    int32 length = 0;

    InFileName.FindLastChar(L'\\', index);
    length = InFileName.Len() - 1;

    FString fileName = InFileName.Right(length - index); // a.cpp
    UE_LOG(GP, Display, L"%s, %s, %d", *fileName, *InFuncName, InLineNumber);
}

void CLog::Print(int32 InValue, int32 InKey, float InDuration, FColor InColor)
{
    // InKey --> 스크린에 표시되는 라인넘버와 같은것  : -1이면 기존에 출력된것 이후

    GEngine->AddOnScreenDebugMessage(InKey, InDuration, InColor, FString::FromInt(InValue));
}

void CLog::Print(float InValue, int32 InKey, float InDuration, FColor InColor)
{
    GEngine->AddOnScreenDebugMessage(InKey, InDuration, InColor, FString::SanitizeFloat(InValue));
}

void CLog::Print(const FString & InValue, int32 InKey, float InDuration, FColor InColor)
{
	GEngine->AddOnScreenDebugMessage(InKey, InDuration, InColor, InValue);
}

void CLog::Print(const FVector & InValue, int32 InKey, float InDuration, FColor InColor)
{
	GEngine->AddOnScreenDebugMessage(InKey, InDuration, InColor, InValue.ToString());
}

void CLog::Print(const FRotator & InValue, int32 InKey, float InDuration, FColor InColor)
{
	GEngine->AddOnScreenDebugMessage(InKey, InDuration, InColor, InValue.ToString());
}

void CLog::Print(const UObject * InValue, int32 InKey, float InDuration, FColor InColor)
{
	FString str;
	if (!!InValue) //if(InValue != nullptr)
		str.Append(InValue->GetName());

	str.Append(!!InValue ? "Not Null" : "Null");

	GEngine->AddOnScreenDebugMessage(InKey, InDuration, InColor, str);
}

void CLog::Print(const FString & InFileName, const FString & InFuncName, int32 InLineNumber)
{
	//" C:\\a\\a.cpp"
	int32 index = 0, length = 0;
	InFileName.FindLastChar(L'\\', index);
	length = InFileName.Len() - 1;

	FString fileName = InFileName.Right(length - index); // subtract

	FString str = FString::Printf(L"%s, %s, %d ", *fileName, *InFuncName, InLineNumber);


	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, str);
}
