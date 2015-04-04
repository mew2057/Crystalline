// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGGameInstance.h"




ACGGameSession* UCGGameInstance::GetGameSession() const
{
	UWorld* const World = GetWorld();
	if (World)
	{
		AGameMode* const Game = World->GetAuthGameMode();
		if (Game)
		{
			return Cast<ACGGameSession>(Game->GameSession);
		}
	}

	return nullptr;
}
