// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "CGGameInstance.generated.h"

/**
 * A game instance class to expose features for the blueprints.
 */
UCLASS()
class CRYSTALLINE_API UCGGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public :
	/**
	 * Retrieves the currently "live" Session class for the current game mode.
	 *
	 * @return The Session currently active in the game mode, if present.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Instance | Session")
	virtual ACGGameSession* GetGameSession() const;
	
};
