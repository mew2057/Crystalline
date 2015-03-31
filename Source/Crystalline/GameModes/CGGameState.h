// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "CGGameState.generated.h"


/**
 * Client side persistent representation of the game. Manages goal score, messages and more.
 */
UCLASS()
class CRYSTALLINE_API ACGGameState : public AGameState
{
	GENERATED_BODY()

public:
	ACGGameState(const FObjectInitializer& ObjectInitializer);

	/**Notify the players that the match has ended.*/
	virtual void HandleMatchHasEnded() override;

	/** Takes advantage of the existant game mode replication for specifying the game mode for the message system.*/
	virtual void ReceivedGameModeClass() override;

	/**
	 * Performs an implementation of Bubble Sort on the players for rankings. 
	 * Bubble sort is used over Merge Sort due to the adaptive nature of the Sorting algorithm. 
	 * Typically this should only take about 2N time as there will generally only be one swap.
	 */
	void SortPlayers();

	/**
	 * Called from the server.  Determines if a Score Message for the PointsToWin has been specified, if it has broadcast an RPC that plays the message on every player's screen.
	 * @param Player The player this message is about.
	 * @param PointsToWin Checks the CrystallineMessages for the appropriate message. Checks the MessageProperties for an entry with a matching value.
	 */
	void FindAndPlayScoreMessage(ACGPlayerState* Player, int32 PointsToWin) const;

	/**  
	 * Retrieves the Text for the index supplied.
	 * @param MessageIndex The Index of the message requested.
	 * @return The Message String.
	 */
	FString GetScoreMessageText(int32 MessageIndex) const;


	/** Time left in the round, post game, pre game, etc.*/
	UPROPERTY(Transient, Replicated)
	int32 RemainingTime;

	/** Set at the start of the match by the game mode, then never touched again.*/
	UPROPERTY(Transient, Replicated)
	float GoalScore;



protected:
	/** A Collection of constant messages used to provide the player with appropriate game specific messages.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FCGGameMessageProperties CrystallineMessages;

	/** A collection of the Game Mode Messages. */
	FCGGameModeMessageProperties GameModeMessages;
};
