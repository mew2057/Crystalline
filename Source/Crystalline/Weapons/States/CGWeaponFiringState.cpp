// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGWeaponFiringState.h"

// TODO Refactor this when I get it all working. Need to make sure that burst fire is possible.
void UCGWeaponFiringState::EnterState()
{
	// Always a positive time.
	const float TimeSinceShot =  GetOuterACGWeapon()->GetWorld()->GetTimeSeconds() - GetOuterACGWeapon()->LastFireTime;

	// If it's negative or zero then we can fire.
	const float TimeRemaining = FMath::Max( 0.f, GetOuterACGWeapon()->WeaponConfig.TimeBetweenShots - TimeSinceShot);

	// XXX verify this!
	if (TimeRemaining > 0 && GetOuterACGWeapon()->LastFireTime > 0.f)
	{
		GetCGOwner()->GetWorldTimerManager().SetTimer(this, &UCGWeaponFiringState::FireShot, TimeRemaining, false);	// Time for the first iteration.
	}
	else
	{
		FireShot();
	}

}

void UCGWeaponFiringState::EndState() 
{
	GetCGOwner()->GetWorldTimerManager().ClearAllTimersForObject(this);

	// Reset any spread that may have developed for the gun.
	GetOuterACGWeapon()->CurrentSpread = GetOuterACGWeapon()->HitScanConfig.BaseSpread;

	GetOuterACGWeapon()->StopFiring();
}

void UCGWeaponFiringState::StopFire()
{
	GetOuterACGWeapon()->GotoState(GetOuterACGWeapon()->ActiveState);
}

void UCGWeaponFiringState::FireShot()
{
	// If the gun has automatic fire, set a timer to continue firing, IFF the fire was successful
	if (GetOuterACGWeapon()->StartFiring() && GetOuterACGWeapon()->WeaponConfig.bAutomaticFire)
	{
		GetOuterACGWeapon()->UseAmmo();
		GetCGOwner()->GetWorldTimerManager().SetTimer(
			this,
			&UCGWeaponFiringState::FireShot,
			GetOuterACGWeapon()->WeaponConfig.TimeBetweenShots,
			false);
	}



}
