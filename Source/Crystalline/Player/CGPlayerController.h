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

	/** Sets up UI related components.*/
	virtual void SetupInputComponent() override;

	/**Shows the in game scoreboard.*/
	void ACGPlayerController::OnShowScoreboard();

	/**Hides the in game scoreboard.*/
	void ACGPlayerController::OnHideScoreboard();

	/**Notifies the player that the round is over.*/
	virtual void ClientGameEnded_Implementation(AActor* EndGameFocus, bool bIsWinner) override;

	/** notify player about started match */
	UFUNCTION(reliable, client)
	void ClientGameStarted();
	//virtual void ClientGameStarted_Implementation();
};
