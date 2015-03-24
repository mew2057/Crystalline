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
	virtual void ScoreKill( int32 Points);

	/**Doesn't affect player socre, just changes deaths.*/
	virtual void ScoreDeath();

	/**Only Suicides negatively impact player score.*/
	virtual void ScoreSuicide(int32 Points);

	FString GetShortenedName();

	// Broadcasted across server and client reliably.
	UFUNCTION(Reliable, NetMulticast)
	void BroadcastDeathMessage(ACGPlayerState* Killer, ACGPlayerState* KilledPlayer, const UDamageType* DamageType);
	void BroadcastDeathMessage_Implementation(ACGPlayerState* Killer, ACGPlayerState* KilledPlayer, const UDamageType* DamageType);

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
