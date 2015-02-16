// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "CGBaseGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGBaseGameMode : public AGameMode
{
	GENERATED_BODY()

	ACGBaseGameMode(const FObjectInitializer& ObjectInitializer);

//	virtual void SetPlayerDefaults(APawn* PlayerPawn) override;
};
