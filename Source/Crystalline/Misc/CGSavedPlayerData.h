// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Misc/CGSaveGame.h"
#include "CGSavedPlayerData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class CRYSTALLINE_API UCGSavedPlayerData : public UCGSaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString PlayerName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString SaveSlotName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 UserIndex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		uint32 UsingIPAddress:1;

	UCGSavedPlayerData(const FObjectInitializer& ObjectInitializer);
	
	
};
