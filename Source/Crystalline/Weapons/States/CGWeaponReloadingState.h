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


	virtual void EnterState() override
	{
		// Stop reloading if it's not possible.
		if (!GetOuterACGWeapon()->GetCanReload())
		{
			UE_LOG(LogTemp, Log, TEXT("Can't reload"));
			// The owner can't fire so tell it to stop firing.
			GetOuterACGWeapon()->StopReload();
			return;
		}

		const float ReloadTime = GetOuterACGWeapon()->GetReloadTime();
		UE_LOG(LogTemp, Log, TEXT("Reload Start"));

		if (ReloadTime > .0f)
		{
			GetCGOwner()->GetWorldTimerManager().SetTimer(this, 
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
		GetCGOwner()->GetWorldTimerManager().ClearTimer(this, &UCGWeaponReloadingState::ReloadFinished);
	}
	
	
};
