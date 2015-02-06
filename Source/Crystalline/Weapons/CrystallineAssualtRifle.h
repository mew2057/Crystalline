// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/CrystallineWeapon.h"
#include "CrystallineAssualtRifle.generated.h"

USTRUCT()
struct FInstantHit
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector Origin;

	UPROPERTY()
	float VSpread;

	UPROPERTY()
	float HSpread;

	UPROPERTY()
	uint32 RandSeed;
};

/**
 * A Hitscan weapon, the root of the crystal gun hierarchy.
 */
UCLASS(Abstract)
class CRYSTALLINE_API ACrystallineAssualtRifle : public ACrystallineWeapon
{
	GENERATED_UCLASS_BODY()

protected:

#pragma region Fields
	/***/
	UPROPERTY(Transient, ReplicatedUsing = ONRep_HitNotify)
	FInstantHit HitNotify;
	
	/** damage amount TODO MOVETHIS TO ROOT */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	int32 HitDamage;

	

#pragma endregion

	virtual bool CanFire() override;

	/** Fires the Assault Rifle projectile. */
	virtual void FireWeapon() override;

	virtual void UseAmmo() override;

	virtual void ProcessHitScan(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandSeed, float VSpread, float HSpread);

	virtual void ProcessHitScan_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandSeed, float VSpread, float HSpread);

	/** server notified of hit from client to verify */
	UFUNCTION(reliable, server, WithValidation)
	void ServerNotifyHit(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float VSpread, float HSpread);

	/** server notified of miss to show trail FX */
	UFUNCTION(unreliable, server, WithValidation)
	void ServerNotifyMiss(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float VSpread, float HSpread);

	UFUNCTION()
	void ONRep_HitNotify();

	void SpawnTrailEffect(const FVector& EndPoint);

	void SpawnImpactEffects(const FHitResult& Impact);

	void SimulateHitScan(const FVector& Origin, int32 RandomSeed, float VSpread, float HSpread);

	bool ShouldDealDamage(AActor* TestActor) const;

	void DealDamage(const FHitResult& Impact, const FVector& ShootDir);

};
