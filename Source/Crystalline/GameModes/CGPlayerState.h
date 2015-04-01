// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "CGPlayerState.generated.h"

#define MAX_PLAYER_NAME_SIZE 12
#define TRUNCATION "..."

/**
 * Awards score to the player based on the score event and tracks Crystalline Specific game stats. 
 */
UCLASS()
class CRYSTALLINE_API ACGPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ACGPlayerState(const FObjectInitializer& ObjectInitializer);

	/**
	 * Clears the player score, deaths and kills.
	 */
	virtual void Reset() override;

	/** 
	 * Prevents seamless transition player states from being removed from the session.
	 */
	virtual void UnregisterPlayerWithSession() override;
	
	/**
	 * Increments the number of kills, the score and forces a net update.
	 *
	 * @param Points The number of points that the kill is worth, added to the score.
	 */
	void ScoreKill( int32 Points);

	/**
	 * Changes number of deaths.
	 * Net Update is not forced and score is untouched.
	 */
	void ScoreDeath();

	/**
	 * Reduces the score of the player. Deaths are not recorded in this function call.
	 * 
	 * @param Points The number of points that a suicide will reduce the player's score by.
	 */
	void ScoreSuicide(int32 Points);

	/** 
 	 * Scores an objective for the player state.
	 * Currently doesn't impact anything but player score.
	 *
	 * @param Points The number of points to reward the player score.
	 */
	void ScoreObjective(int32 Points);

	/**
	* A Blueprint hook for modifying the score. Generally shouldn't be used in native code.
	*
	* @param AddToScore Number of points to add to the score.
	*/
	UFUNCTION(BlueprintCallable, Category = "State Score")
	void AddScore(int32 AddToScore);

	/** 
	 * Shortens the name to the defined name size and appends an elipse.
	 *
	 * @return The Player Name shortened to the appropriate length.
	 */
	FString GetShortenedName();

	/** XXX is there a way to expose the actual function for this?
	 * Invokes APlayerState's SetPlayerName.
	 *
	 * @param NewName The new name for the player.
	 */
	UFUNCTION(BlueprintCallable, Category = "State Name")
	virtual void SetName(const FString& NewName);

	UFUNCTION(BlueprintCallable, Category = "State Name")
		virtual void SetPlayerName(const FString& NewName) override {
		Super::SetPlayerName(NewName);
	};

	// Broadcasted across server and client reliably.
	/**
	 * Plays the Death Message on all local human player controllers.
	 *
	 * @param Killer The player who is responsible for the death.
	 * @param KilledPlayer The player whose character died.
	 * @param DamageType The type of damage that resulted in the death.
	 */
	UFUNCTION(Reliable, NetMulticast)
	void BroadcastDeathMessage(ACGPlayerState* Killer, ACGPlayerState* KilledPlayer, const UDamageType* DamageType);
	void BroadcastDeathMessage_Implementation(ACGPlayerState* Killer, ACGPlayerState* KilledPlayer, const UDamageType* DamageType);


	// Broadcasted across server and client reliably.
	/**
	* Plays A score message on all local human player controllers.
	*
	* @param MessageIndex The index of the message in the Game State MessageProperties.
	*/
	UFUNCTION(Reliable, NetMulticast)
	void BroadcastGameScoreMessage(int32 MessageIndex);
	void BroadcastGameScoreMessage_Implementation(int32 MessageIndex);


	/** 
	 * Getter for the number of kills.
	 *
	 * @return Number of Kills.
	 */
	FORCEINLINE int32 GetNumKills() { return NumKills;  }

	/**
	* Getter for the number of deaths.
	*
	* @return Number of deaths.
	*/
	FORCEINLINE int32 GetNumDeaths() { return NumDeaths; }

protected:
	/**The number of times the associated player has killed another player.*/
	UPROPERTY(Transient, Replicated)
	int32 NumKills;

	/**The Number of times the associated player has died*/
	UPROPERTY(Transient, Replicated)
	int32 NumDeaths;	
};
