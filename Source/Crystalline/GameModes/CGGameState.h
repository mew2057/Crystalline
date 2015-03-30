// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "CGGameState.generated.h"


/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGGameState : public AGameState
{
	GENERATED_BODY()

public:
	ACGGameState(const FObjectInitializer& ObjectInitializer);

	/**Notify the players that the match has ended.*/
	virtual void HandleMatchHasEnded() override;

	/**
	 * Performs an implementation of Bubble Sort on the players for rankings. 
	 * Bubble sort is used over Merge Sort due to the adaptive nature of the Sorting algorithm. 
	 * Typically this should only take about 2N time as there will generally only be one swap.
	 */
	void SortPlayers();

	/** Time left in the round, post game, pre game, etc.*/
	UPROPERTY(Transient, Replicated)
	int32 RemainingTime;

	/** Set at the start of the match by the game mode, then never touched again.*/
	UPROPERTY(Transient, Replicated)
	float GoalScore;



protected:
	/** The index for the game mode in the message structure for the GameState. */
	UPROPERTY(Transient, Replicated)
	int32 CurrentGameMode;

	/** A Collection of constant messages used to provide the player with appropriate game specific messages.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FCGCrystallineMessageAssortment CrystallineMessages;
};
