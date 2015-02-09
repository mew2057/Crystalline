// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/CGWeapon.h"
#include "CGCrystalGun.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGCrystalGun : public ACGWeapon
{
	GENERATED_BODY()
	
public:
	ACGCrystalGun(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

#pragma region Ammo

	virtual void UseAmmo();

	virtual bool CanFire() const;

	virtual float GetClipPercent() const;

#pragma endregion

protected:
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FCGAmmoData AmmoConfig;
	
	
};
