// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "OnlineSessionSettings.h"
#include "CGGameSession.h"

UFUNCTION(BlueprintCallable, Category = "Session | Settings")
void ACGGameSession::SetMapName(const FString & MapName)
{
	HostSettings->Set(SETTING_MAPNAME, MapName, EOnlineDataAdvertisementType::ViaOnlineService);

}

UFUNCTION(BlueprintCallable, Category = "Session | Settings")
void ACGGameSession::SetGameMode(const FString & GameType)
{
	HostSettings->Set(SETTING_GAMEMODE, GameType, EOnlineDataAdvertisementType::ViaOnlineService);
}
