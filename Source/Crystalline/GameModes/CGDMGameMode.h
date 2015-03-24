// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameModes/CGBaseGameMode.h"
#include "CGDMGameMode.generated.h"

/**
 * The Death Match Game Mode defines behaviors for score checking and game states.
 */
UCLASS()
class CRYSTALLINE_API ACGDMGameMode : public ACGBaseGameMode
{
	GENERATED_BODY()
	
protected:	
	/** The winner of the game mode. */
	UPROPERTY(transient)
	class ACGPlayerState* GameWinner;

	/** Checks the score and sends a message to the players when a player is found to be close to winning. */
	virtual void CheckScore(ACGPlayerState* Player) override;

	/** If the player matches the game winner, the player is considered a winner.*/
	virtual bool IsWinner(ACGPlayerState* Player) override;
  
	/** Determine who's the winner and supplies it to the super class if Winner was not specified.  */
	virtual void EndGame(ACGPlayerState* Winner) override;

	///** select best spawn point for player */
	//virtual AActor* ChoosePlayerStart(AController* Player) override;
	//
	///** always pick new random spawn */
	//virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
 	//
	///** check team constraints */
	//virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const;
	//
	///** check if player should use spawnpoint */
	//virtual bool IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const;
};
