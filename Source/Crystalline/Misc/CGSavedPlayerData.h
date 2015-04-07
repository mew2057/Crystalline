// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Misc/CGSaveGame.h"
#include "CGSavedPlayerData.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API UCGSavedPlayerData : public UCGSaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, Category = Basic)
		FString PlayerName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
		FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
		uint32 UserIndex;

	UPROPERTY(VisibleAnywhere, Category = Basic)
		uint32 UsingIPAddress:1;

	UCGSavedPlayerData(const FObjectInitializer& ObjectInitializer);
	
	
};
