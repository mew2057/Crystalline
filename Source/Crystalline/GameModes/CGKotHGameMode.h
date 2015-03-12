// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameModes/CGBaseGameMode.h"
#include "CGKotHGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGKotHGameMode : public ACGBaseGameMode
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(transient)
	class ACGPlayerState* GameWinner;

	virtual void CheckScore(ACGPlayerState* Player) final;

	virtual bool IsWinner(ACGPlayerState* Player) override;

	virtual void EndGame(ACGPlayerState* Winner) override;
	
};
