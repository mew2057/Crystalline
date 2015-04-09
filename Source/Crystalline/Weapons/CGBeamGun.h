// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/CGCrystalGun.h"
#include "CGBeamGun.generated.h"

/**
 * The Beam Gun, Fires a bolt of energy that "latches" on the enemy at the expense of DPS. Like the Pistol this is a bit of a special unicorn weapon.
 */
UCLASS()
class CRYSTALLINE_API ACGBeamGun : public ACGCrystalGun
{
	GENERATED_BODY()
public:
	ACGBeamGun(const FObjectInitializer& ObjectInitializer);

	void PostInitializeComponents();

	/** Overrides the hitscan.*/
	virtual void FireHitScan() override;
	virtual void StopFire() override;

	virtual void StopWeaponFireSimulation() override;
	virtual void SpawnTrailEffect(const FVector& EndPoint) override;

	UFUNCTION(reliable, server, WithValidation)
	void ServerNotifyBeamFire(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir);
	virtual bool ServerNotifyBeamFire_Validate(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir);
	virtual void ServerNotifyBeamFire_Implementation(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir);
	void ProcessBeam(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir);

protected:
	UPROPERTY(Transient, Replicated)
	AActor* Target;

	/** The maximum angle that the beam can be pointed away from it's target.*/
	UPROPERTY(EditDefaultsOnly, Category=Config)
	float MaxAngle;
	
};
