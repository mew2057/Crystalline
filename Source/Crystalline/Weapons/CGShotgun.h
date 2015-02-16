// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/CGCrystalGun.h"
#include "CGShotgun.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGShotgun : public ACGCrystalGun
{
	GENERATED_BODY()
	
public:

	virtual void SpawnProjectile(FVector Origin, FVector_NetQuantizeNormal ShootDir)  override;

protected:

	/**Refers to the number of bullets spawned on fire, not related to ammo.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	int32 BulletsPerShot;

};
