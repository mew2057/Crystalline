// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CGWeaponState.generated.h"

/**
 * Base Definition for the weapon states, defines general behavior for the classes.
 */
UCLASS(DefaultToInstanced, EditInlineNew, CustomConstructor, Within=CGWeapon)
class UCGWeaponState : public UObject
{
	GENERATED_UCLASS_BODY()

	UCGWeaponState(const FObjectInitializer& PCIP) : Super(PCIP)
	{}	

	
	inline ACGCharacter* GetCGOwner()
	{
		return GetOuterACGWeapon()->GetCGOwner();
	}

	virtual UWorld* GetWorld() const override
	{
		return GetOuterACGWeapon()->GetWorld();
	}
	

	
	
};
