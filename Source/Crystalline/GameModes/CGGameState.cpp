// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGPlayerState.h"
#include "CGGameState.h"

ACGGameState::ACGGameState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	RemainingTime = 0;
	GoalScore = 1;
}


void ACGGameState::HandleMatchHasEnded()
{
	//UE_Log(LogTemp, Log,TEXT("Match has ended!"));
}



void ACGGameState::SortPlayers()
{
	int32 PlayerCount = PlayerArray.Num();

	// Kick out if the player count is less than or equal to 1.
	if (PlayerCount <= 1)
	{
		return;
	}

	int32 FarthestSwapped = 0;

	// Replaced Merge Sort with an implementation of Bubble Sort. 
	// Merge sort is stable and scales well, but this data is unlikely to change by more than one or two positions between sorts, meaning 
	// typically we'll take advantage of the Adaptive nature of Bubble Sort.
	do
	{
		FarthestSwapped = 0;
		for (int32 i = 1; i < PlayerCount; ++i)
		{
			if (PlayerArray[i - 1]->Score < PlayerArray[i]->Score)
			{
				PlayerArray.SwapMemory(i - 1, i);
				FarthestSwapped = i;
			}
		}
		// Only iterate until the farthest swap on the next pass as the array is guaranteed to be sorted past that point.
		PlayerCount = FarthestSwapped;
	} while (FarthestSwapped > 0);
}

void ACGGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACGGameState, RemainingTime);
	DOREPLIFETIME(ACGGameState, GoalScore);
	DOREPLIFETIME(ACGGameState, CurrentGameMode);
}
