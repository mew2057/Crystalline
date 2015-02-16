// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/States/CGWeaponState.h"
#include "CGWeaponInactiveState.generated.h"

/**
 * 
 */
UCLASS(CustomConstructor)
class UCGWeaponInactiveState : public UCGWeaponState
{
	GENERATED_BODY()

public:
	UCGWeaponInactiveState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{}
	

	virtual void StartEquip() override
	{
		GetOuterACGWeapon()->GotoEquippingState();
	}

	// Firing is NOT Legal from the Inactive State
	// No children of the Inactive state may change this behavior.
	virtual void StartFire() final { }
	virtual void StartReload() final { }

};
