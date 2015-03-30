// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameModes/CGBaseGameMode.h"
#include "CGKotHGameMode.generated.h"

/**
 * Defines the King of the Hill game mode. Now with 90% less bobby.
 */
UCLASS()
class CRYSTALLINE_API ACGKotHGameMode : public ACGBaseGameMode
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(transient)
	class ACGPlayerState* GameWinner;

	virtual bool IsWinner(ACGPlayerState* Player) override;

	virtual void EndGame(ACGPlayerState* Winner) override;	
};
