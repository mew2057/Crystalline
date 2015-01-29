// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Weapons/CrystallineWeapon.h"
#include "CrystallinePlayerInventory.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACrystallinePlayerInventory : public AActor
{
	GENERATED_BODY()

#pragma region Weapons
protected:
	
	/** The Default list of weapons the player is carrying. */
	UPROPERTY(EditDefaultsOnly, Category=Weapons)
	TArray<TSubclassOf<class ACrystallineWeapon>> DefaultWeaponClasses;

	UPROPERTY(Transient)//, Replicated) // Transient- Empty on creation; Replicated- Replicated on server. 
	TArray<class ACrystallineWeapon*> Weapons; // This list shouldn't change after spawn.

	/** The currently equipped weapon for the player. */
	UPROPERTY(Transient) // Todo this needs an on replicate.
	ACrystallineWeapon* CurrentWeapon;

#pragma endregion
	
	
};
