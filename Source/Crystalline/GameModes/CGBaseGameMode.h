// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"

#include "CGPlayerState.h"
#include "CGGameState.h"
#include "CGBaseGameMode.generated.h"


/**
 * The Base Game Mode for Crystalline. Defines Bot Spawn logic, timer, and match states.
 */
UCLASS()
class CRYSTALLINE_API ACGBaseGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	/** Constructs the Game Mode. Initializes game attributes (mainly for the Editor's benefit) and defines the default classes that manage what the player sees. */
	ACGBaseGameMode(const FObjectInitializer& ObjectInitializer);

	/**
	 *	Informs the game state that a kill occured. The score is updated (if appropriate) and checked to see if the game is over.
	 *	Broadcasts the death information across the clients so appropriate messages may be passed to the player's HUD.
	 *
	 *  @param Killer The player who scored the kill, or commited suicide.
	 *  @param KilledPlayer The player who was the victim of the Kill.
	 *	@param DamageType The type of damage that killed the player, e.g. Shotgun, Pistol, etc.
	 */
	virtual void Killed(AController* Killer, AController* KilledPlayer, const UDamageType* DamageType);
	
	/**
	 * Gets the maximum round time. Note: This is NOT the current round time.
	 */
	UFUNCTION(BlueprintCallable, Category = "GameMode|State")
	int32 GetRoundTime() const;

	/**
	 * Gets the score necessary to "win" the round.
	 */
	UFUNCTION(BlueprintCallable, Category = "GameMode|State")
	int32 GetScoreToWin() const;


protected:	

	/**Called on the Transition to WaitingToStart, spawns bots if they are enabled for this game mode.*/
	virtual void HandleMatchIsWaitingToStart() override;

	/** 
	 * Invoked when the game enters the In Progress State. 
	 * Initializes the goal score and roundtime in the CGGameState, 
	 * Starts the bot players and Notifies the clients that the game has started.
	 */
	virtual void HandleMatchHasStarted() override;

	/** Updates the gameplay timer, if the timer zeroes out react according to the state that the gmae was in.*/
	virtual void DefaultTimer() override;

	/** Gets the Pawn for the controller, used to spawn bots.*/
	virtual UClass* GetDefaultPawnClassForController(AController* InController) override;

	/** Prevents players from spawning on top of each other. */
	virtual AActor* ChoosePlayerStart(AController* Player) override;

	/**
	 * Checks the score after a scoring action for the player, implemented in the subclasses
	 * @param Player The player who's state is being checked to see if it has won the game.
	 */
	UFUNCTION(BlueprintCallable, Category = "GameMode|State")
	virtual void CheckScore(ACGPlayerState* Player);

	/**
	 * Determines which player won the round.
	 * @param Winner The Winning state for easy checking in the function.
	 */
	virtual void EndGame(ACGPlayerState* Winner = NULL);

	/** 
	 *	XXX This function is not currently used by Crystalline.
	 * Determines whether or not the player is the winner of the round.
	 * @param Player The player being checked for winner status.
	 * @return True if the player is the winner.
	 */
	virtual bool IsWinner(ACGPlayerState* Player);

	/**
	 * Iterates over the controllers that exists in the game world and determines how many more 
	 * bot controllers need to be spawned and spawns them.
	 */
	void CreateBots();

	/**
	 * Spawns a bot controller with the specified ID and intitializes them.
	 * @param BotID The unique ID for the bot.
	 */
	void SpawnBot(int32 BotID);

	/** 
	 * Restarts all of the bots to start them off.
	 */
	void StartBots();

	/**Time, in seconds, for a single round.*/
	UPROPERTY(EditDefaultsOnly, Category=Config)
	int32 RoundTime;

	/** The score to win the round. */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	int32 ScoreToWin;

	/** Score awarded per kill.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	int32 ScorePerKill;

	/** Score deducted per suicide.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	int32 SuicidePenalty;

	/** Time for the post game state. */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	int32 PostGameTime;

	/** Default pawn for the bot. */
	UPROPERTY(EditDefaultsOnly, Category = BotConfig)
	TSubclassOf<class APawn> BotPawn;

	/** Flag for whether or not the Game should spawn bots. */
	UPROPERTY(EditDefaultsOnly, Category = BotConfig)
	uint32 bSpawnBots : 1;

	/** The number of bots the game should spawn. */
	UPROPERTY(EditDefaultsOnly, Category = BotConfig)
	int32 BotsInRound;
};
