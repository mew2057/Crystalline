// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/States/CGWeaponState.h"
#include "CGWeaponReloadingState.generated.h"

/**
 * 
 */
UCLASS(CustomConstructor)
class UCGWeaponReloadingState : public UCGWeaponState
{
	GENERATED_BODY()
	
public:	
	UCGWeaponReloadingState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{}
	/**Timer Handle for the Reload timer.*/
	FTimerHandle TimerHandle_Reload;

	virtual void EnterState() override
	{
		// Stop reloading if it's not possible.
		if (!GetOuterACGWeapon()->CanReload())
		{
			// The owner can't fire so tell it to stop firing.
			GetOuterACGWeapon()->StopReload();
			return;
		}

		const float ReloadTime = GetOuterACGWeapon()->GetReloadTime();

		if (ReloadTime > .0f)
		{
			GetCGOwner()->GetWorldTimerManager().SetTimer(TimerHandle_Reload, this,
				&UCGWeaponReloadingState::ReloadFinished, ReloadTime, false);
		}
		else
		{
			ReloadFinished();
		}
	}

	void ReloadFinished()
	{
		GetOuterACGWeapon()->ApplyReload();
		GetOuterACGWeapon()->StopReload();
	}

	virtual void EndState() override
	{
		GetCGOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_Reload);
	}

	// You should not be able to cancel this with fire.
	virtual void StartFire() override {}

	
	
};
