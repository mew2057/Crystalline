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

	virtual void Tick(float DeltaSecondss)
	{}
	
	// Do Nothing, only inactive/Equipping should process this.
	virtual bool StartEquip() { return false; }

	virtual bool StartUnequip()
	{
		GetOuterACGWeapon()->GotoState(GetOuterACGWeapon()->UnequippingState);
		return true;
	}


	virtual void StartReload()
	{
		GetOuterACGWeapon()->GotoState(GetOuterACGWeapon()->ReloadingState);
	}

	/** Override if firing is not legal from that state.
	@return True, if it is legal to fire in this state.
	*/
	virtual bool StartFire()
	{
		return false;
	}

	// ONLY firing states should hit this.
	virtual void StopFire()
	{}
	
	// Add more as needed.
	
};
