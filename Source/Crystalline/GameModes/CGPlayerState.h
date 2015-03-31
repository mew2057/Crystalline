// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "CGPlayerState.generated.h"

#define MAX_PLAYER_NAME_SIZE 12
#define TRUNCATION "..."

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ACGPlayerState(const FObjectInitializer& ObjectInitializer);

	/**Clears out the Score for the player.*/
	virtual void Reset() override;

	virtual void ClientInitialize(class AController* InController) override;

	virtual void UnregisterPlayerWithSession() override;

	/**Increment the number of kills and the score.*/
	void ScoreKill( int32 Points);

	/**Doesn't affect player socre, just changes deaths.*/
	void ScoreDeath();

	/**Only Suicides negatively impact player score.*/
	void ScoreSuicide(int32 Points);

	/** 
 	 * Scores an objective for the player state.
	 * Currently doesn't impact anything but player score.
	 *
	 * @param Points The number of points to reward the player score.
	 */
	void ScoreObjective(int32 Points);


	FString GetShortenedName();

	// Broadcasted across server and client reliably.
	UFUNCTION(Reliable, NetMulticast)
	void BroadcastDeathMessage(ACGPlayerState* Killer, ACGPlayerState* KilledPlayer, const UDamageType* DamageType);
	void BroadcastDeathMessage_Implementation(ACGPlayerState* Killer, ACGPlayerState* KilledPlayer, const UDamageType* DamageType);


	// Broadcasted across server and client reliably.
	UFUNCTION(Reliable, NetMulticast)
	void BroadcastGameScoreMessage(int32 MessageIndex);
	void BroadcastGameScoreMessage_Implementation(int32 MessageIndex);


	UFUNCTION(BlueprintCallable, Category = "StateScore")
	void AddScore(int32 AddToScore);

	FORCEINLINE int32 GetNumKills() { return NumKills;  }

	FORCEINLINE int32 GetNumDeaths() { return NumDeaths; }

protected:
	/***/
	UPROPERTY(Transient, Replicated)
	int32 NumKills;

	/***/
	UPROPERTY(Transient, Replicated)
	int32 NumDeaths;	
};
