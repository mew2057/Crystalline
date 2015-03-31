// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "CGPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	/**Restarts the player*/
	virtual void UnFreeze() override;

	/** If the pawn can't spawn, try the spawn again.*/
	virtual void FailedToSpawnPawn() override;

	/** Kick out to spectator mode when the player is killed.*/
	virtual void PawnPendingDestroy(APawn * inPawn ) override;

	/** Sets up UI related components.*/
	virtual void SetupInputComponent() override;

	/**Shows the in game scoreboard.*/
	void OnShowScoreboard();

	/**Hides the in game scoreboard.*/
	void OnHideScoreboard();

	/**Displays the kill message on the player HUD.*/
	void OnKillMessage(ACGPlayerState* Killer, ACGPlayerState* KilledPlayer, const UDamageType* DamageType);


	/**
	 * Displays GameMode Messages to the player.
	 * @param Message The message the player is to see, e.g. kills to win.
	 */
	void OnGameScoreMessage(int32 MessageIndex);

	/**Notifies the player that the round is over.*/
	virtual void ClientGameEnded_Implementation(AActor* EndGameFocus, bool bIsWinner) override;

	/** notify player about started match */
	UFUNCTION(reliable, client)
	void ClientGameStarted();
	virtual void ClientGameStarted_Implementation();

	/**
	 * Invoked by the server to set the spectator camera location and rotation. This is most important in the initial spawn.
	 *
	 * @param Location The location that the spectator is located.
	 * @param Rotation The rotation of the spectator.
	 */
	UFUNCTION(reliable, client)
	void ClientSetSpectatorCamera(const FVector & Location, const FRotator & Rotation);
	virtual void ClientSetSpectatorCamera_Implementation(const FVector & Location, const FRotator & Rotation);
};
