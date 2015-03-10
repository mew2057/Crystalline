// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "CGSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API UCGSaveGame : public USaveGame
{
	GENERATED_BODY()

	
public:
	UCGSaveGame(const FObjectInitializer& ObjectInitializer);
	
};
