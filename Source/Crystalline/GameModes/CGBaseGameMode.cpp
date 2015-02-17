// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGBaseGameMode.h"
#include "Player/CGCharacter.h"
#include "Player/CGPlayerController.h"

#include "GUI/CGPlayerHUD.h"



ACGBaseGameMode::ACGBaseGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/Player/CGPlayer"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	PlayerControllerClass = ACGPlayerController::StaticClass();	
	HUDClass              = ACGPlayerHUD::StaticClass();
	PlayerStateClass      = ACGPlayerState::StaticClass();
	GameStateClass        = ACGGameState::StaticClass();

	MinRespawnDelay = 2.0f;
	ScoreToWin = 25;
	RoundTime = 300;
	ScorePerKill = 1;
	SuicidePenalty = 1; 

}

void ACGBaseGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	// Start the game, if pregame time exists set that, otherwise start the match.
	ACGGameState* const CGGameState = Cast<ACGGameState>(GameState);
	if (CGGameState)
	{
		CGGameState->RemainingTime = RoundTime;
	}

	// XXX notify the players?
}


void ACGBaseGameMode::DefaultTimer()
{
	ACGGameState* const CGGameState = Cast<ACGGameState>(GameState);
	if (CGGameState && CGGameState->RemainingTime > 0 && --CGGameState->RemainingTime <= 0)
	{
		// If there's no longer time in the match we need to end the match.
		if (GetMatchState() == MatchState::InProgress)
		{
			EndGame();

		}
		else if (GetMatchState() == MatchState::WaitingToStart)
		{
			// Built into AGameMode.
			StartMatch();
		}
		else if (GetMatchState() == MatchState::WaitingPostMatch)
		{
			// Built into AGameMode.
			RestartGame();
		}
	}
}


void ACGBaseGameMode::Killed(AController* Killer, AController* KilledPlayer, const UDamageType* DamageType)
{
	// If the killer was the killed player, it was a suicide.
	if (Killer == KilledPlayer || (Killer == NULL))
	{
		if (KilledPlayer != NULL && Cast<ACGPlayerState>(KilledPlayer->PlayerState))
		{
			Cast<ACGPlayerState>(KilledPlayer->PlayerState)->ScoreSuicide(SuicidePenalty);
		}
	}
	else
	{
		ACGPlayerState* const KillerState = Cast<ACGPlayerState>(Killer->PlayerState);
		if (KillerState)
		{
			// Let the Killer Score
			KillerState->ScoreKill(ScorePerKill);
			// Check for a win.
			CheckScore(KillerState);
		}
	}

	// Increment the death counter.
	if (KilledPlayer != NULL && Cast<ACGPlayerState>(KilledPlayer->PlayerState))
	{
		Cast<ACGPlayerState>(KilledPlayer->PlayerState)->ScoreDeath();
	}
}

void ACGBaseGameMode::CheckScore(ACGPlayerState* Player)
{
	// If the player meets the goal score, they win.
}

/**Determines which player won the round.*/
void ACGBaseGameMode::EndGame(ACGPlayerState* Winner)
{
	EndMatch();
}

/** Determines whether or not the player is the winner of the round.*/
bool ACGBaseGameMode::IsWinner(ACGPlayerState* Player) 
{ 
	return false;
}
