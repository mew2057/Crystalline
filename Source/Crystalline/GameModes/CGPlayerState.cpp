// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "Player/CGPlayerController.h"

#include "CGPlayerState.h"

ACGPlayerState::ACGPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NumKills = 0;
	NumDeaths = 0;
	Score = 0;
}

void ACGPlayerState::Reset()
{
	Super::Reset();

	NumKills  = 0;
	NumDeaths = 0;
	Score     = 0;
}

void ACGPlayerState::ClientInitialize(class AController* InController)
{
	Super::ClientInitialize(InController);
}

void ACGPlayerState::UnregisterPlayerWithSession()
{
	if (!bFromPreviousLevel)
	{
		Super::UnregisterPlayerWithSession();
	}
}

void ACGPlayerState::ScoreKill( int32 Points)
{
	NumKills++;
	Score += Points;
	ForceNetUpdate();
}

void ACGPlayerState::ScoreDeath()
{
	NumDeaths++;
}

void ACGPlayerState::ScoreSuicide(int32 Points)
{
	Score -= Points;
	ForceNetUpdate();
}

void ACGPlayerState::ScoreObjective(int32 Points)
{
	Score += Points;
	ForceNetUpdate();
}


FString ACGPlayerState::GetShortenedName()
{
	if (PlayerName.Len() > MAX_PLAYER_NAME_SIZE)
	{
		return PlayerName.Left(MAX_PLAYER_NAME_SIZE) + TRUNCATION;
	}
	return PlayerName;
}


void ACGPlayerState::BroadcastDeathMessage_Implementation(ACGPlayerState* Killer, ACGPlayerState* KilledPlayer, const UDamageType* DamageType)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACGPlayerController * TempController = Cast<ACGPlayerController>(*It);
		if (TempController && TempController->IsLocalController())
		{
			// Invoke the dialog message.
			TempController->OnKillMessage(Killer, KilledPlayer, DamageType);
		}
	}
}

void ACGPlayerState::BroadcastGameModeMessage_Implementation(const FString & Message)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACGPlayerController * TempController = Cast<ACGPlayerController>(*It);
		if (TempController && TempController->IsLocalController())
		{
			// Invoke the dialog message for GameMode Messages.
			TempController->OnGameModeMessage(Message);
		}
	}
}


void ACGPlayerState::AddScore(int32 AddToScore)
{
	Score += AddToScore;
	ForceNetUpdate();
}

void ACGPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACGPlayerState, NumKills);
	DOREPLIFETIME(ACGPlayerState, NumDeaths);
}