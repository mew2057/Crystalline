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

protected:

public:
	UCGWeaponFiringState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{}

	virtual void EnterState() override;

	virtual void EndState() override;

	virtual void StopFire() override;

	// Fires what's constituted as a "shot", Different implementations of this state may differ.
	void FireShot();

	
	
};
