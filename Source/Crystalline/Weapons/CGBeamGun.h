// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/CGCrystalGun.h"
#include "CGBeamGun.generated.h"
#define BEAM_COLOR_OVER_LIFE FName(TEXT("InstanceColorScaleOverLife"))

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

	void DealDamageBeam(const FHitResult& Impact, const FVector& ShootDir,float Dist);

protected:
	/** How strong is the gun's lock? 0: none, 1: full drift compatibilty*/
	UPROPERTY(Replicated)
	float LockStrength;

	UPROPERTY(Transient, Replicated)
	AActor* Target;

	/** The maximum angle that the beam can be pointed away from it's target.*/
	UPROPERTY(EditDefaultsOnly, Category=Config)
	float MaxAngle;

	/** Defines the damage curve for the weapon relative to the distance.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	UDistributionFloatConstantCurve* DamageCurve;

	/** The connected beam patricle for when the player has a target. */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	UParticleSystem* ConnectedBeam;
	
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FLinearColor HitColor;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	FLinearColor NoHitColor;
};
