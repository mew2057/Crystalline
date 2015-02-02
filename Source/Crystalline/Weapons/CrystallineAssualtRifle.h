// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/CrystallineWeapon.h"
#include "CrystallineAssualtRifle.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class CRYSTALLINE_API ACrystallineAssualtRifle : public ACrystallineWeapon
{
	GENERATED_BODY()
	

protected:

	virtual void UpdateWeapon(float DeltaSeconds) override;

	virtual bool CanFire() override;

	/** Fires the Assault Rifle projectile. */
	virtual void FireWeapon() override;

	virtual void UseAmmo() override;

};
