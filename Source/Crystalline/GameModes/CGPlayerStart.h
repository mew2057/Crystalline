// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerStart.h"
#include "CGPlayerStart.generated.h"

/**
 * Defines a more robust player start for Crystalline game modes.
 */
UCLASS()
class CRYSTALLINE_API ACGPlayerStart : public APlayerStart
{
	GENERATED_BODY()
public:

	/**
	 * Retrieves whether or not this spawn is an on start spawn only.
	 * @return True if the player start is for the initial spawn only.
	 */
	UFUNCTION(BlueprintCallable, Category = "Player Start")
	bool IsInitialSpawn();

	/**
	 * Indicates whether or not a bot may spawn at this player start.
	 *
	 * @return True if a bot may spawn at this location.
	 */
	UFUNCTION(BlueprintCallable, Category = "Player Start")
	bool IsBotSpawn();

	/**
	 * Retrieves the current team number for a spawn.
	 *
	 * @return The team numeric identifier.
	 */
	UFUNCTION(BlueprintCallable, Category = "Player Start")
	uint8 GetTeamNumber();

	/**
	* Retrieves the last time that the the spawn was used.
	*
	* @return The time that this start was last used.
	*/
	UFUNCTION(BlueprintCallable, Category = "Player Start")
	float GetLastSpawnTime();

	/**
	 * Sets the time that this Start was used at for the sake of determining start rating.
	 *
	 * @param Time The time this Start was last used.
	 */
	UFUNCTION(BlueprintCallable, Category = "Player Start")
	void SetLastSpawnTime(float Time);
	
protected:
	/** The team number of this spawn. Note this is only used in team game types.*/
	UPROPERTY(EditAnywhere)
	uint8 TeamNumber;
	
	/** The player can only spawn here once at the start of the game. */
	UPROPERTY(EditAnywhere)
	uint32 bInitialSpawnOnly : 1;

	/** Flag that dictates whether or not a bot may spawn at this location. */
	UPROPERTY(EditAnywhere)
	uint32 bBotsMaySpawn : 1;

	/** The last time that this start spawned a player. */
	UPROPERTY(Transient)
	float LastSpawnTime;
};
