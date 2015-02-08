// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/States/CGWeaponState.h"
#include "CGWeaponEquippingState.generated.h"

/**
 * 
 */
UCLASS(CustomConstructor)
class UCGWeaponEquippingState : public UCGWeaponState
{
	GENERATED_BODY()

public:
	UCGWeaponEquippingState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{}

	virtual void EnterState() override
	{}

	virtual void StartEquip() override
	{}


	virtual void EquipFinished() 
	{
		//GetOuterACGWeapon()->Goto
		// GOTO Active State
	}

};
