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


	/**
	 * Starts the game as a host, includes the game type string and travel url.
	 * 
	 * @param LocalPlayer
	 * @param GameType Defines the String for the game type, meaning the level doesn't need to specifically have a game type.
	 * @param InTravelURL Defines the travel URL for the game.
	 * @return True on sucessful host.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Instance | Session")
	virtual bool HostGame(ULocalPlayer* LocalPlayer, const FString& GameType, const FString& InTravelURL) const;


	/** TODO Document
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Instance | Session")
	virtual void ServerTravel(const FString& URL) const;

	
};
