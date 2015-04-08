// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/States/CGWeaponState.h"
#include "CGWeaponReloadOverheatState.generated.h"

/**
 * XXX FIXME TODO THIS IS A HACK! Redo me.
 * XXX Tightly Coupled
 */
UCLASS()
class CRYSTALLINE_API UCGWeaponReloadOverheatState : public UCGWeaponState
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	float OverheatTime;

public:

	virtual void EnterState() override
	{
		// Make sure the overheat guage is fully maxed.
		GetOuterACGWeapon()->UseAmmo();
		GetOuterACGWeapon()->StartOverheat();
		GetOuterACGWeapon()->PlayReload();
	}

	void Tick(float DeltaSeconds)
	{
		// If the player should be able to fire, then allow them to fire.
		if (GetOuterACGWeapon()->CanFire())
		{
			GetOuterACGWeapon()->StopReload();
		}
	}

	virtual void EndState() override
	{
		GetCGOwner()->GetWorldTimerManager().ClearAllTimersForObject(this);	
	}
};
