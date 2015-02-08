// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CGWeaponState.generated.h"

/**
 * Base Definition for the weapon states, defines general behavior for the classes.
 */
UCLASS(DefaultToInstanced, EditInlineNew, Within = CGWeapon)
class UCGWeaponState : public UObject
{
	GENERATED_BODY()


public:
	UCGWeaponState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{}


	inline ACGCharacter* GetCGOwner()
	{
		return GetOuterACGWeapon()->GetCGOwner();
	}

	virtual UWorld* GetWorld() const override
	{
		return GetOuterACGWeapon()->GetWorld();
	}

	virtual void EnterState()
	{}
	
	virtual void EndState() 
	{}
	
	// Do Nothing, only inactive/Equipping should process this.
	virtual void StartEquip() 
	{
		
	}

	virtual void StartUnequip()
	{
		GetOuterACGWeapon()->GotoUnequippingState();
	}


	virtual void StartReload()
	{}

	virtual void FinishReload()
	{}
	
	virtual void StartFire()
	{}

	virtual void FinishFire()
	{}
	
	// Add more as needed.
	
};
