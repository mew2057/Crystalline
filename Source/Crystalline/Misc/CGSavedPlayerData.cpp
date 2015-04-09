// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGSavedPlayerData.h"

UCGSavedPlayerData::UCGSavedPlayerData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SaveSlotName = TEXT("SavedPlayerData");
	LastUsedIPAddress = TEXT("127.0.0.1");
	UserIndex = 0;
	UsingIPAddress = 0;
}


