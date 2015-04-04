// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGMenuGameMode.h"

void ACGMenuGameMode::RestartPlayer(class AController* NewPlayer)
{
	// Intentionally Blank
	//Super::RestartPlayer(NewPlayer);
}

TSubclassOf<AGameSession> ACGMenuGameMode::GetGameSessionClass() const
{
	return ACGGameSession::StaticClass();

}
