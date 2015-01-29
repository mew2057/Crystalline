// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CrystallineWeapon.generated.h"

UENUM()
namespace CrystallineWeaponState
{
	enum Type
	{
		Idle,
		Firing,
		Reloading,
		Equipping,
		Unusable, // This is a general case, if unusable the weapon may not be fired.
	};
}

// Defines the ammo type used in the weapons
UENUM() 
namespace CrystallineAmmo
{
	enum Type
	{
		Cooldown , // The gun doesn't use traditional ammo, but overheats over time.
		Crystal    // The Crystal gun's ammo.
	};
}

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class CRYSTALLINE_API ACrystallineWeapon : public AActor
{
	GENERATED_BODY()

	

#pragma region Overriden_Functions

	virtual void PostInitializeComponents() override;

#pragma endregion


#pragma region Fields

protected:
	//////////////////////////
	// Timing
	
	UPROPERTY(EditDefaultsOnly, Category = Timing)
	float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, Category = Timing)
	float ReloadTime;
	
	
	////////////////////////////
	////Accuracy - The weapon spread may be only necessary in sub classes - John
	//
	///** The maximum spread of the bullets in the horizontal dimension. TODO define what this translates to. */
	//UPROPERTY(EditDefaultsOnly, Category = Accuracy)
	//float HorizontalSpread;
	//
	///** The maximum spread of the bullets in the vertical dimension. TODO define what this translates to. */
	//UPROPERTY(EditDefaultsOnly, Category = Accuracy)
	//float VerticalSpread;
	//
	////TODO
	//// Determine if a range increment is necessary.
	//// It's necessary for hitscan guns - John
	//
	////////////////////////////
	//// Ammo Details

	/** The type of ammo used in the gun. */
	UPROPERTY(EditDefaultsOnly, Category = Ammunition)
	TEnumAsByte <CrystallineAmmo::Type> AmmoType; // NOTE: TEnumAsByte is needed for blueprint exposed enumerated types.
	
	/** The number of rounds available to the gun. */
	int32 Rounds;

	/** The number of rounds in a single ammo clip */
	int32 RoundsPerClip;

	


#pragma endregion



#pragma region Functions

#pragma endregion



};
