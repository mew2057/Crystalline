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

	/** Time to unequip. */
	UPROPERTY(EditDefaultsOnly)
	float UnequipTime;

public:

	UCGWeaponUnequippingState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{
		UnequipTime = .5f;
	}

	virtual void EnterState() override
	{
		// Owner not set on pistol in local!
		ACGWeapon* chaar = GetOuterACGWeapon();
		if (chaar)
			GetCGOwner()->GetWorldTimerManager().SetTimer(this, &UCGWeaponUnequippingState::UnequipFinished, UnequipTime);
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
