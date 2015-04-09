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
		// TODO check if this timer is active, if it is just return.
		//UE_LOG(LogTemp, Warning, TEXT("FIREDELAY %s"), GetCGOwner()->Role < ROLE_Authority ? TEXT("LOCAL") : TEXT("REMOTE"));
		if (!GetCGOwner()->GetWorldTimerManager().IsTimerActive(TimerHandle_Refire))
		{
			GetCGOwner()->GetWorldTimerManager().SetTimer(TimerHandle_Refire, this, &UCGWeaponFiringState::FireShot, TimeRemaining, false);	// Time for the first iteration.
		//	UE_LOG(LogTemp, Warning, TEXT("%f"), TimeRemaining);
		}
	}
	else
	{
	//	UE_LOG(LogTemp, Warning, TEXT("FIRENOW %s"), GetCGOwner()->Role < ROLE_Authority ? TEXT("LOCAL") : TEXT("REMOTE"));
 
		FireShot();
	}

}

void UCGWeaponFiringState::EndState() 
{
	//UE_LOG(LogTemp, Warning, TEXT("%s END STATE : %s"), GetCGOwner()->Role < ROLE_Authority ? TEXT("LOCAL") : TEXT("REMOTE"),
	//	GetCGOwner()->GetWorldTimerManager().IsTimerActive(TimerHandle_Refire) ? TEXT("Timer Active") : TEXT("Timer InActive"));


	// This occasionally crashes.
	GetCGOwner()->GetWorldTimerManager().ClearAllTimersForObject(this);

	// Reset any spread that may have developed for the gun.
	GetOuterACGWeapon()->CurrentSpread = GetOuterACGWeapon()->SpreadConfig.BaseSpread;

	GetOuterACGWeapon()->StopFiring();
}

void UCGWeaponFiringState::StopFire()
{
	GetOuterACGWeapon()->GotoState(GetOuterACGWeapon()->ActiveState);
}

void UCGWeaponFiringState::FireShot()
{

	if (GetOuterACGWeapon()->StartFiring())
	{
		GetOuterACGWeapon()->UseAmmo();

		// If the gun has automatic fire, set a timer to continue firing, IFF the fire was successful
		if (GetOuterACGWeapon()->WeaponConfig.bAutomaticFire && GetCGOwner())
		{
			GetCGOwner()->GetWorldTimerManager().SetTimer(TimerHandle_Refire,
				this,
				&UCGWeaponFiringState::FireShot,
				GetOuterACGWeapon()->WeaponConfig.TimeBetweenShots,
				false);
		}
	}
	
	/*UE_LOG(LogTemp, Warning, TEXT("%s :  %s"), 
		GetCGOwner()->GetWorldTimerManager().IsTimerActive(TimerHandle_Refire) ? TEXT("Timer Active") : TEXT("Timer InActive"),
		GetCGOwner()->Role < ROLE_Authority ? TEXT("LOCAL") : TEXT("REMOTE"));*/
	



}
