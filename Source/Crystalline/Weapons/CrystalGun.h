// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/CrystallineWeapon.h"
#include "CrystalGun.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACrystalGun : public ACrystallineWeapon
{
	GENERATED_BODY()
	
	
	virtual void UpdateWeapon(float DeltaSeconds) override;

	virtual bool CanFire() override;

	virtual void FireWeapon() override;

	virtual void UseAmmo() override;


	
};
