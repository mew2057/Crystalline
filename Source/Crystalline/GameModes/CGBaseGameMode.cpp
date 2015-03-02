// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGBaseGameMode.h"
#include "Player/CGCharacter.h"
#include "Player/CGPlayerController.h"
#include "Bots/CGBotController.h"
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
	PostGameTime = 10;
	ScorePerKill = 1;
	SuicidePenalty = 1; 
	bSpawnBots = true;
	BotsInRound = 2;

}

void ACGBaseGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	if (bSpawnBots)
	{
		// Create Bots
		CreateBots();
	}

}

void ACGBaseGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	// Start the game, if pregame time exists set that, otherwise start the match.
	ACGGameState* const CGGameState = Cast<ACGGameState>(GameState);
	if (CGGameState)
	{
		CGGameState->RemainingTime = RoundTime;
		CGGameState->GoalScore = ScoreToWin;
	}

	// Kicks off the bot players.
	StartBots();

	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		ACGPlayerController* PC = Cast<ACGPlayerController>(*It);
		if (PC)
		{
			PC->ClientGameStarted();
		}
	}
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

UClass* ACGBaseGameMode::GetDefaultPawnClassForController(AController* InController)
{ 

	// If we have a bot controller, tell the game mode we need a bot pawn.
	if (Cast<ACGBotController>(InController))
	{
		return BotPawn;
	}

	return Super::GetDefaultPawnClassForController(InController);
}



void ACGBaseGameMode::Killed(AController* Killer, AController* KilledPlayer, const UDamageType* DamageType)
{
	// Don't score kills if the match is over.
	if (GetMatchState() != MatchState::InProgress)
	{
		return;
	}

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

	for (FConstPawnIterator Iterator = GetWorld()->GetPawnIterator(); Iterator; ++Iterator)
	{
		ACGCharacter* Character = Cast<ACGCharacter>(*Iterator);
		ACGPlayerController* Controller = Character ? Cast<ACGPlayerController>(Character->Controller) : NULL;
		if (Controller)
		{
			// TODO handle ties.

			if (Winner == Controller->PlayerState)
			{
				// Tell them they won.
				Controller->GameHasEnded(Character, true);
			}
			else
			{
				// Tell them they lost.
				Controller->GameHasEnded(Winner, false);
			}			 
		}
	}

	// Set up the Timer for restart.
	ACGGameState* const CGGameState = Cast<ACGGameState>(GameState);
	if (CGGameState)
	{
		CGGameState->RemainingTime = PostGameTime;
	}

	EndMatch();
}

/** Determines whether or not the player is the winner of the round.*/
bool ACGBaseGameMode::IsWinner(ACGPlayerState* Player) 
{ 
	return false;
}

void ACGBaseGameMode::CreateBots()
{
	int32 BotsActive = 0;
	// Iterate over the controllers.
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		
		if (Cast<ACGBotController>(*It))
		{
			BotsActive++;
		}
	}

	for (; BotsActive < BotsInRound; BotsActive++)
	{
		SpawnBot(BotsActive);
	}

}

void ACGBaseGameMode::SpawnBot(int32 BotId)
{

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoCollisionFail = true;
	SpawnInfo.Instigator = nullptr;
	SpawnInfo.OverrideLevel = nullptr;

	ACGBotController* Bot = GetWorld()->SpawnActor<ACGBotController>(SpawnInfo);

	if (Bot && Bot->PlayerState)
	{
		Bot->PlayerState->PlayerName = FString::Printf(TEXT("Bottisimo #%d"), BotId);
	}
	
}


void ACGBaseGameMode::StartBots()
{
	ACGBotController* Bot;
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		Bot = Cast<ACGBotController>(*It);
		if (Bot)
		{
			RestartPlayer(Bot);
		}
	}
}
