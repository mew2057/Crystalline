// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameModes/CGBaseGameMode.h"
#include "CGMenuGameMode.generated.h"

/**
 * Defined separately from ACGBaseGameMode, as the menu system does not require bots.
 */
UCLASS()
class CRYSTALLINE_API ACGMenuGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	/** Ignores Restarts, since the menu doesn't need player pawns. */
	virtual void RestartPlayer(class AController* NewPlayer) override;

	/** @return CGGameSession  */
	virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;	
};
