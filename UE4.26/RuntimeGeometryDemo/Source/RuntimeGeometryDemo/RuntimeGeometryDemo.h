// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/Console.h"

// Disable warnings for ProtoBuf
//#pragma warning (disable : 4800) // forcing value to bool true or false
//#pragma warning (disable : 4125) // decimal digit terminates octal escape sequence
//#pragma warning (disable : 4647) // behavior change __is_pod has different value in previous version
//#pragma warning (disable : 4668) // 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'

inline FString ENetModeToFString(ENetMode NetMode)
{
	if (NetMode == NM_Standalone) return "NM_Standalone";
	if (NetMode == NM_DedicatedServer) return "NM_DedicatedServer";
	if (NetMode == NM_ListenServer) return "NM_ListenServer";
	if (NetMode == NM_Client) return "NM_Client";
	if (NetMode == NM_MAX) return "NM_MAX";
	return "UNKNOWN";
}

inline void SYSLOG(const FString Text)
{
	UE_LOG(LogTemp, Log, TEXT("%s"), *Text);
}

inline void CONLOG(const FString Text, AActor* Actor, bool bLog)
{
	UConsole* ViewportConsole = (GEngine !=NULL && GEngine->GameViewport != NULL) ? GEngine->GameViewport->ViewportConsole : NULL;
	FString LogText = "";

	if (Actor != nullptr)
	{
		const FString NetMode = ENetModeToFString(Actor->GetNetMode());
		LogText = FString::Printf(TEXT("[%s] : %s"), *NetMode, *Text);
	} else
	{
		LogText = FString::Printf(TEXT("[X] %s"), *Text);
	}

	if (ViewportConsole != nullptr)
	{
		ViewportConsole->OutputText(LogText);
	}
	if (bLog)
	{
		UE_LOG(LogTemp, Log, TEXT("%s"), *LogText);
	}
}
