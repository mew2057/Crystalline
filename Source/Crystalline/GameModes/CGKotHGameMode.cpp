// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGKotHGameMode.h"


void ACGKotHGameMode::CheckScore(ACGPlayerState* Player)
{
	if (Player->Score >= ScoreToWin)
	{
		// Win the Game.
		EndGame(Player);
	}
	else if (Player->Score == ScoreToWin - 5){
		//One kill to win
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Player->PlayerName + " is 5 seconds away from winning.");
	}
	else if (Player->Score == ScoreToWin - 10){
		//One kill to win
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Player->PlayerName + " is 10 seconds away from winning.");
	}
	else if (Player->Score == ScoreToWin - 30){
		//5 kills to win
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Player->PlayerName + " is 30 seconds away from winning.");
	}
	else if (Player->Score == ScoreToWin - 60){
		//5 kills to win
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Player->PlayerName + " is 1 minute away from winning.");
	}
}


bool ACGKotHGameMode::IsWinner(ACGPlayerState* Player)
{
	return Player && Player == GameWinner;
}


void ACGKotHGameMode::EndGame(ACGPlayerState* Winner)
{
	// If the winner is not supplied, try and find them.
	if (Winner == NULL)
	{
		for (FConstControllerIterator Iterator = GetWorld()->GetControllerIterator();
			Iterator;
			++Iterator)
		{
			AController* Controller = *Iterator;
			ACGPlayerState* PlayerState = Cast<ACGPlayerState>(Controller->PlayerState);
			if (PlayerState)
			{
				if (Winner == NULL || PlayerState->Score > Winner->Score)
				{
					Winner = PlayerState;
				}
				else if (PlayerState->Score == Winner->Score)
				{
					// TODO handle tiebreakers!
				}
			}
		}
	}

	GameWinner = Winner;

	Super::EndGame(Winner);
}
