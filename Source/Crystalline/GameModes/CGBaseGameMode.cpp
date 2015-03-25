// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGBaseGameMode.h"
#include "Player/CGCharacter.h"
#include "Player/CGPlayerController.h"
#include "Bots/CGBotController.h"
#include "CGPlayerStart.h"
#include "GUI/CGPlayerHUD.h"

#pragma region Public

ACGBaseGameMode::ACGBaseGameMode(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	ScoreToWin(25),
	RoundTime(300),
	PostGameTime(10),
	ScorePerKill(1),
	SuicidePenalty(1),
	bSpawnBots(false),
	BotsInRound(2),
	PlayerStartCooldownTime(10.f)
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/Player/CGPlayer"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	PlayerControllerClass = ACGPlayerController::StaticClass();	
	HUDClass              = ACGPlayerHUD::StaticClass();
	PlayerStateClass      = ACGPlayerState::StaticClass();
	GameStateClass        = ACGGameState::StaticClass();
	MinRespawnDelay		  = 2.f;
}

void ACGBaseGameMode::Killed(AController* Killer, AController* KilledPlayer, const UDamageType* DamageType)
{
	// Don't score kills if the match is over.
	if (GetMatchState() != MatchState::InProgress)
	{
		return;
	}

	ACGPlayerState* const KillerPlayerState = Cast<ACGPlayerState>(Killer->PlayerState);
	ACGPlayerState* const VictimPlayerState = KilledPlayer ? Cast<ACGPlayerState>(KilledPlayer->PlayerState) : NULL;

	// If the killer was the killed player, it was a suicide.
	if (Killer == KilledPlayer || (Killer == NULL))
	{
		if (KilledPlayer != NULL && Cast<ACGPlayerState>(KilledPlayer->PlayerState))
		{
			VictimPlayerState->ScoreSuicide(SuicidePenalty);
		}
	}
	else if (KillerPlayerState) // Otherwise score a kill for the killer.
	{
		// Let the Killer Score
		KillerPlayerState->ScoreKill(ScorePerKill);
		// Check for a win.
		CheckScore(KillerPlayerState);
	}

	// Increment the death counter.
	if (KilledPlayer != NULL && Cast<ACGPlayerState>(KilledPlayer->PlayerState))
	{
		VictimPlayerState->ScoreDeath();
		VictimPlayerState->BroadcastDeathMessage(KillerPlayerState, VictimPlayerState, DamageType);
	}

}

int32 ACGBaseGameMode::GetRoundTime() const
{ 
	return RoundTime; 
}


int32 ACGBaseGameMode::GetScoreToWin() const
{
	return ScoreToWin; 
}

#pragma endregion Public

#pragma region Protected

#pragma region Override

void ACGBaseGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	if (bSpawnBots)
	{
		CreateBots();
	}
}

void ACGBaseGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	// Set time
	ACGGameState* const CGGameState = Cast<ACGGameState>(GameState);
	if (CGGameState)
	{
		CGGameState->RemainingTime = RoundTime;
		CGGameState->GoalScore = ScoreToWin;
	}

	// Initializes the bot players.
	StartBots();

	// Inform all of the players that the game has started.
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
		// If the game was waiting to start start the match
		else if (GetMatchState() == MatchState::WaitingToStart) 
		{
			// Built into AGameMode.
			StartMatch();
		}
		// If the game was in the post match, restart it.
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

	// If not a bot spawn the default bot.
	return Super::GetDefaultPawnClassForController(InController);
}

AActor* ACGBaseGameMode::FindPlayerStart(AController* Player, const FString& IncomingName)
{
	AActor* PlayerStart = Super::FindPlayerStart(Player, IncomingName);

	// This is approximately correct.
	ACGPlayerStart* CGPlayerStart = Cast<ACGPlayerStart>(PlayerStart);
	if (CGPlayerStart)
	{
		CGPlayerStart->SetLastSpawnTime(GetWorld()->GetTimeSeconds());
	}	

	return PlayerStart;
}


AActor* ACGBaseGameMode::ChoosePlayerStart(AController* Player)
{
	// Determine if the player is a bot and get the crystalline controller.
	bool bIsBot = Cast<ACGBotController>(Player) != NULL;
	ACGPlayerController* PlayerController = Cast<ACGPlayerController>(Player);

	const int32 RandomStart = FMath::RandHelper(PlayerStarts.Num());
	const int32 StartCount = PlayerStarts.Num();

	float BestScore = 0.f;
	float CurrentScore = 0.f;

	ACGPlayerStart* BestStart    = NULL;
	ACGPlayerStart* CurrentStart = NULL;

	for (int32 i = 0; i < StartCount; ++i)
	{
		CurrentStart = Cast<ACGPlayerStart>(PlayerStarts[(i + RandomStart) % StartCount]);
		CurrentScore = RatePlayerStart(CurrentStart, PlayerController, bIsBot);
		
		// If the rating is perfect return the current state.
		if (CurrentScore == 100.f)
		{
			return CurrentStart;
		}
		else if (CurrentScore > BestScore)
		{
			BestScore = CurrentScore;
			BestStart = CurrentStart;
		}
	}

	return BestStart != NULL ? BestStart : Super::ChoosePlayerStart(Player);
}

#pragma endregion Override

float ACGBaseGameMode::RatePlayerStart( ACGPlayerStart* Start, ACGPlayerController* PlayerController, bool bBot) const
{
	float StartRating = 100.f;

	// If the PlayerStart is not yet allowed to spawn penalize the spawn.
	if (Start->GetLastSpawnTime() > 0 && 
		PlayerStartCooldownTime > (GetWorld()->GetTimeSeconds() - Start->GetLastSpawnTime()))
	{
		StartRating -= 50.f;
	}

	// EARLY RETURN
	// If the player is a bot and the spawn doesn't support bots, or 
	// the spawn is for bots and the player is not then this spawn is not suitable.
	if (XOR(Start->IsBotSpawn(), bBot))
	{
		return 0.f;
	}


	// Make Sure the player controller exists.
	if (PlayerController != NULL)
	{
		// XXX probably shouldn't do this cast every time?
		// Initial spawns should only be used once, if the player has died don't spawn them there again.
		ACGPlayerState* PlayerState = Cast<ACGPlayerState>(PlayerController->PlayerState);

		if (PlayerState->GetNumDeaths() > 0 && Start->IsInitialSpawn())
		{
			return -100.f;
		}

		// TODO Spatial locality.
	}	

	return StartRating;
}


void ACGBaseGameMode::CheckScore(ACGPlayerState* Player)
{
	// If the player meets the goal score, they win.
}

/**Determines which player won the round.*/
void ACGBaseGameMode::EndGame(ACGPlayerState* Winner)
{
	// Iterate over the pawns in the game world and let them know the game has ended.
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

	// Set up the timer for restart.
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

void ACGBaseGameMode::SpawnBot(int32 BotID)
{
	// Set the spawn parameters for the bot
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoCollisionFail = true;
	SpawnInfo.Instigator = nullptr;
	SpawnInfo.OverrideLevel = nullptr;

	// Spawn the bot controller in the world.
	ACGBotController* Bot = GetWorld()->SpawnActor<ACGBotController>(SpawnInfo);

	if (Bot && Bot->PlayerState)
	{
		Bot->PlayerState->PlayerName = FString::Printf(TEXT("Bottisimo #%d"), BotID);
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

#pragma endregion Protected
