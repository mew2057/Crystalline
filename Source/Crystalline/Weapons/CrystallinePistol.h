// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/CrystallineWeapon.h"
#include "CrystallinePistol.generated.h"

USTRUCT()
struct FPistolProjectileData
{
	GENERATED_USTRUCT_BODY()
	/** The class of the Projectile that will be shot by the weapon. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ACrystallineProjectile> ProjectileClass;

	/** The life span of the projectile before despawning. */
	UPROPERTY(EditDefaultsOnly)
	float ProjectileLife;

	/** Sets defaults */
	FPistolProjectileData()
	{
		ProjectileClass = NULL;
		ProjectileLife = 2.0f;
	}
};

/**
 * A cooldown based projectile weapon.
 */
UCLASS(Abstract)
class CRYSTALLINE_API ACrystallinePistol : public ACrystallineWeapon
{
	GENERATED_BODY()

		/** The configuration for the pistol's projectile. */
		UPROPERTY(EditDefaultsOnly, Category = ProjectileConfig)
		FPistolProjectileData ProjectileConfig;

protected:

	/** Fires the pistol projectile. */
	virtual void FireWeapon() override;

	
};
