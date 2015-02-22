// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/CGInventory.h"
#include "CGBotInventory.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGBotInventory : public ACGInventory
{
	GENERATED_BODY()

	virtual void DestroyInventory() override;

	virtual void InitializeInventory(const FCGDefaultWeaponConfig& Config) override;

	
private:

	UPROPERTY(Transient)
	int32 TierOneAmmo;

	UPROPERTY(Transient)
	int32 TierTwoAmmo;

	
};
