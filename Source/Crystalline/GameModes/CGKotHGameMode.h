// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameModes/CGBaseGameMode.h"
#include "CGKotHGameMode.generated.h"

/**
 * Defines the King of the Hill game mode. Now with 90% less bobby.
 */
UCLASS(Abstract, Config = GameMode)
class CRYSTALLINE_API ACGKotHGameMode : public ACGBaseGameMode
{
	GENERATED_BODY()
public:
	ACGKotHGameMode(const FObjectInitializer& ObjectInitializer);

	//XXX Remove?
	/** Check to see that the player matches the game winner.*/
	virtual bool IsWinner(ACGPlayerState* Player) override;

protected:
	/** The winner of the game.*/
	UPROPERTY(transient)
	class ACGPlayerState* GameWinner;

	/**
	 * Tries to find the winner if one is not supplied.
	 */
	virtual void EndGame(ACGPlayerState* Winner) override;	
};
