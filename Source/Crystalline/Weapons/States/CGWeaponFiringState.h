// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/States/CGWeaponState.h"
#include "CGWeaponFiringState.generated.h"

/**
 * 
 */
UCLASS(CustomConstructor)
class UCGWeaponFiringState : public UCGWeaponState
{
	GENERATED_BODY()

public:
	UCGWeaponFiringState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{}

	
	
	
};
