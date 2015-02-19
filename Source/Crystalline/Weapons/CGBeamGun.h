// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/CGCrystalGun.h"
#include "CGBeamGun.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGBeamGun : public ACGCrystalGun
{
	GENERATED_BODY()
public:
	void PostInitializeComponents();

	/** Overrides the hitscan.*/
	virtual void FireHitScan() override;
	virtual void StopFire() override;
	virtual void ProcessHitScanConfirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandSeed, float Spread) override;
	virtual void StopWeaponFireSimulation() override;
	virtual void SpawnTrailEffect(const FVector& EndPoint) override;

protected:
	UPROPERTY(Transient, Replicated)
	class AActor* Target;

	/** The maximum angle that the beam can be pointed away from it's target.*/
	UPROPERTY(EditDefaultsOnly, Category=Config)
	float MaxAngle;
	
};
