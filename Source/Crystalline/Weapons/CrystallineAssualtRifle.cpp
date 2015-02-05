// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CrystallineAssualtRifle.h"

ACrystallineAssualtRifle::ACrystallineAssualtRifle(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}


void ACrystallineAssualtRifle::FireWeapon()
{
	// Generate a seed for the weapon fire, then create a stream from it.
	int32 FireSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(FireSeed);

	// Perform a raycast from the crosshair in to the world space.
	// Get the starting location and rotation for the player.
	FVector StartTrace;
	FRotator CamRot;
	OwningPawn->Controller->GetPlayerViewPoint(StartTrace, CamRot);

	// Adds variation to the bullet.
	FVector ShootDir = WeaponRandomStream.VRandCone(CamRot.Vector(), HSpreadCurrent, VSpreadCurrent);

	// Specify the end point for the weapon's fire.
	FVector EndTrace = StartTrace + CamRot.Vector() * WeaponRange;

	// Get the Impact for the weapon trace then confirm whether or not it hit a player.
	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	ProcessHitScan(Impact, StartTrace, ShootDir, FireSeed, VSpreadCurrent, HSpreadCurrent);

	// Update the spread for the weapon.
	HSpreadCurrent = FMath::Min(HSpreadMax, HSpreadCurrent + HSpreadIncrement);
	VSpreadCurrent = FMath::Min(VSpreadMax, VSpreadCurrent + VSpreadIncrement);
}

void ACrystallineAssualtRifle::UseAmmo()
{

}

bool ACrystallineAssualtRifle::CanFire()
{
	return false;
}


bool ACrystallineAssualtRifle::ServerNotifyHit_Validate(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float VSpread, float HSpread)
{
	return true;
}

void ACrystallineAssualtRifle::ServerNotifyHit_Implementation(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float VSpread, float HSpread)
{
	// Do Stuff in here
	// TODO read and re-read the example in ShooterWeapon_Instant.

}

bool ACrystallineAssualtRifle::ServerNotifyMiss_Validate(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float VSpread, float HSpread)
{
	return true;
}

void ACrystallineAssualtRifle::ServerNotifyMiss_Implementation(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float VSpread, float HSpread)
{
	// TODO Implement

	// TODO make a hit notify and play the trail effect.
}

void ACrystallineAssualtRifle::ProcessHitScan(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandSeed, float VSpread, float HSpread)
{
	// If the player is local and we're networked (if we aren't we don't need to care about what the server says)
	if (OwningPawn && OwningPawn->IsLocallyControlled() && GetNetMode() == NM_Client)
	{
		// If the actor is controlled by the server notify the server it was hit.
		if (Impact.GetActor() && Impact.GetActor()->GetRemoteRole() == ROLE_Authority)
		{
			ServerNotifyHit(Impact, ShootDir, RandSeed, VSpread, HSpread);
		}
		else if (Impact.GetActor() == NULL) // If there is no actor, then check to see if there was a blocking hit.
		{
			if (Impact.bBlockingHit)
			{
				ServerNotifyHit(Impact, ShootDir, RandSeed, VSpread, HSpread);
			}
			else
			{
				ServerNotifyMiss( ShootDir, RandSeed, VSpread, HSpread);
			}
		}
	}

	// Locally handle hit confirmation.
	// This will deal damage and ouput the FX.
}

