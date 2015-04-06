// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "OnlineSessionSettings.h"
#include "CGGameSession.h"

void ACGGameSession::InitializeHostSettings()
{
	HostSettings = MakeShareable(new FOnlineSessionSettings());
}

void ACGGameSession::SetMapName(const FString & MapName)
{

	//MakeShareable
	if (HostSettings.IsValid())
	{
		HostSettings->Set(SETTING_MAPNAME, MapName, EOnlineDataAdvertisementType::ViaOnlineService);
		UE_LOG(LogTemp, Warning, TEXT("MAP NAME SET"));

	}
}

void ACGGameSession::SetGameMode(const FString & GameType)
{
	if (HostSettings.IsValid())
	{
		HostSettings->Set(SETTING_GAMEMODE, GameType, EOnlineDataAdvertisementType::ViaOnlineService);
		UE_LOG(LogTemp, Warning, TEXT("GAME MODE SET"));

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Host Settings was INVALID!"));

	}
}
