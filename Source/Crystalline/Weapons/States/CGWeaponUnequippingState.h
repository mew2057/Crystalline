// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/States/CGWeaponState.h"
#include "CGWeaponUnequippingState.generated.h"

/**
 * 
 */
UCLASS(CustomConstructor)
class  UCGWeaponUnequippingState : public UCGWeaponState
{
	GENERATED_BODY()

protected:


public:

	UCGWeaponUnequippingState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{
	}

	virtual void EnterState() override
	{
		GetCGOwner()->GetWorldTimerManager().SetTimer(this, &UCGWeaponUnequippingState::UnequipFinished, GetOuterACGWeapon()->WeaponConfig.EquipTime);
	}
	
	void UnequipFinished()
	{
		// XXX May need to track time in here at some point, but I'm going to wait for Matt's anims, because that's polish. -John
		GetOuterACGWeapon()->DetachMeshFromPawn();
		GetOuterACGWeapon()->GotoState(GetOuterACGWeapon()->InactiveState);
		GetCGOwner()->WeaponChanged();
	}

	virtual void EndState() override
	{
		GetCGOwner()->GetWorldTimerManager().ClearTimer(this, &UCGWeaponUnequippingState::UnequipFinished);
	}

	
};
