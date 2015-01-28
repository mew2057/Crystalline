// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CrystallineWeapon.generated.h"

namespace EWeaponState
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


/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class CRYSTALLINE_API ACrystallineWeapon : public AActor
{
	GENERATED_BODY()
	
	
	
	
};
