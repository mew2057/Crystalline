// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameSession.h"
#include "CGGameSession.generated.h"


/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGGameSession : public AGameSession
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Session | Settings")
	virtual void SetMapName(const FString & MapName);
	
	UFUNCTION(BlueprintCallable, Category = "Session | Settings")
	virtual void SetGameMode(const FString & GameMode);
protected:

	/** */
	TSharedPtr<class FOnlineSessionSettings> HostSettings;
};
