// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"

#include "CGPlayerState.h"
#include "CGGameState.h"
#include "CGBaseGameMode.generated.h"



/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGBaseGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ACGBaseGameMode(const FObjectInitializer& ObjectInitializer);

	/**Informs the game state that a kill occured.*/
	virtual void Killed(AController* Killer, AController* KilledPlayer, const UDamageType* DamageType);


protected:


	/**Invoked when the game enters the In Progress State.*/
	virtual void HandleMatchHasStarted() override;


	/** Updates the gameplay timers.*/
	virtual void DefaultTimer() override;

	/**Checks the score after a scoring action for the player.*/
	virtual void CheckScore(ACGPlayerState* Player);


	/**Determines which player won the round.*/
	virtual void EndGame(ACGPlayerState* Winner = NULL);


	/** Determines whether or not the player is the winner of the round.*/
	virtual bool IsWinner(ACGPlayerState* Player);


	/**Time in seconds for a single round*/
	UPROPERTY(EditDefaultsOnly, Category=Config)
	int32 RoundTime;


	UPROPERTY(EditDefaultsOnly, Category = Config)
	int32 ScoreToWin;

	/** Score awarded per kill.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	int32 ScorePerKill;

	/** Score deducted per suicide.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	int32 SuicidePenalty;
};
