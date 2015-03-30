// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGDMGameMode.h"



void ACGDMGameMode::CheckScore(ACGPlayerState* Player)
{
	if (Player->Score >= ScoreToWin)
	{
		// Win the Game.
		EndGame(Player);
	}
	else if (Player->Score == ScoreToWin-1){
		//One kill to win
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Player->PlayerName + " is one kill away from winning.");
	}
	else if (Player->Score == ScoreToWin - 5){
		//5 kills to win
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Player->PlayerName + " is 5 kills away from winning.");
	}
}

bool ACGDMGameMode::IsWinner(ACGPlayerState* Player)
{
	return Player && Player == GameWinner;
}


void ACGDMGameMode::EndGame(ACGPlayerState* Winner)
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
