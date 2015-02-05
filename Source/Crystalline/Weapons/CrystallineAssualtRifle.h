// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/CrystallineWeapon.h"
#include "CrystallineAssualtRifle.generated.h"

/**
 * A Hitscan weapon, the root of the crystal gun hierarchy.
 */
UCLASS(Abstract)
class CRYSTALLINE_API ACrystallineAssualtRifle : public ACrystallineWeapon
{
	GENERATED_UCLASS_BODY()

protected:

	virtual bool CanFire() override;

	/** Fires the Assault Rifle projectile. */
	virtual void FireWeapon() override;

	virtual void UseAmmo() override;

	virtual void ProcessHitScan(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandSeed, float VSpread, float HSpread);

	/** server notified of hit from client to verify */
	UFUNCTION(reliable, server, WithValidation)
	void ServerNotifyHit(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float VSpread, float HSpread);

	/** server notified of miss to show trail FX */
	UFUNCTION(unreliable, server, WithValidation)
	void ServerNotifyMiss(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float VSpread, float HSpread);

};
