// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameModes/CGBaseGameMode.h"
#include "CGDMGameMode.generated.h"

/**
 * The Death Match Game Mode defines behaviors for score checking and game states.
 */
UCLASS(Abstract, Config = GameMode)
class CRYSTALLINE_API ACGDMGameMode : public ACGBaseGameMode
{
	GENERATED_BODY()
public:
	ACGDMGameMode(const FObjectInitializer& ObjectInitializer);

	/** If the player matches the game winner, the player is considered a winner.*/
	virtual bool IsWinner(ACGPlayerState* Player) override;
	
protected:	
	/** The winner of the game mode. */
	UPROPERTY(transient)
	class ACGPlayerState* GameWinner;


	/** Determine who's the winner and supplies it to the super class if Winner was not specified.  */
	virtual void EndGame(ACGPlayerState* Winner) override;
};
