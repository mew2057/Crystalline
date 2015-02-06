// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CrystallineAssualtRifle.h"

ACrystallineAssualtRifle::ACrystallineAssualtRifle(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}


void ACrystallineAssualtRifle::FireWeapon()
{
	// Generate a seed for the weapon fire, then create a stream from it.
	int32 FireSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(FireSeed);

	// Perform a raycast from the crosshair in to the world space.
	// Get the starting location and rotation for the player.
	const FVector StartTrace = GetCameraAim();
	const FVector AimDir = GetCameraLocation();

	// Adds variation to the bullet.
	FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, HSpreadCurrent, VSpreadCurrent);

	// Specify the end point for the weapon's fire.
	FVector EndTrace = StartTrace + AimDir * WeaponConfig.WeaponRange;

	// Get the Impact for the weapon trace then confirm whether or not it hit a player.
	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	ProcessHitScan(Impact, StartTrace, ShootDir, FireSeed, VSpreadCurrent, HSpreadCurrent);

	// Update the spread for the weapon.
	HSpreadCurrent = FMath::Min(WeaponConfig.HSpreadMax, HSpreadCurrent + WeaponConfig.HSpreadIncrement);
	VSpreadCurrent = FMath::Min(WeaponConfig.VSpreadMax, VSpreadCurrent + WeaponConfig.VSpreadIncrement);
}

void ACrystallineAssualtRifle::UseAmmo()
{

}

bool ACrystallineAssualtRifle::CanFire()
{
	return true;
}


bool ACrystallineAssualtRifle::ServerNotifyHit_Validate(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float VSpread, float HSpread)
{
	return true;
}

void ACrystallineAssualtRifle::ServerNotifyHit_Implementation(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float VSpread, float HSpread)
{
	// Do Stuff in here
	// TODO read and re-read the example in ShooterWeapon_Instant.

	// If the weapon has an instigator and we hit.
	if (Instigator && Impact.bBlockingHit)
	{
		ProcessHitScan_Confirmed(Impact, GetMuzzleLocation(), ShootDir, RandomSeed, VSpread, HSpread);
		// TODO add cheat checking
	}

}

bool ACrystallineAssualtRifle::ServerNotifyMiss_Validate(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float VSpread, float HSpread)
{
	return true;
}

void ACrystallineAssualtRifle::ServerNotifyMiss_Implementation(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float VSpread, float HSpread)
{
	const FVector Origin = GetMuzzleLocation();

	// play FX on remote clients
	HitNotify.Origin = Origin;
	HitNotify.RandSeed = RandomSeed;
	HitNotify.VSpread = VSpread;
	HitNotify.HSpread = HSpread;

	// play FX locally
	if (GetNetMode() != NM_DedicatedServer)
	{
		const FVector EndTrace = Origin + ShootDir *WeaponConfig.WeaponRange;
		SpawnTrailEffect(EndTrace);
	}
	
}

void ACrystallineAssualtRifle::ProcessHitScan(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandSeed, float VSpread, float HSpread)
{
	// If the player is local and we're networked (if we aren't we don't need to care about what the server says)
	if (OwningPawn && OwningPawn->IsLocallyControlled() && GetNetMode() == NM_Client)
	{
		// If the actor is controlled by the server notify the server it was hit.
		if (Impact.GetActor() && Impact.GetActor()->GetRemoteRole() == ROLE_Authority)
		{
			ServerNotifyHit(Impact, ShootDir, RandSeed, VSpread, HSpread);
		}
		else if (Impact.GetActor() == NULL) // If there is no actor, then check to see if there was a blocking hit.
		{
			if (Impact.bBlockingHit)
			{
				ServerNotifyHit(Impact, ShootDir, RandSeed, VSpread, HSpread);
			}
			else
			{
				ServerNotifyMiss( ShootDir, RandSeed, VSpread, HSpread);
			}
		}
	}

	// Locally handle hit confirmation.
	// This will deal damage and ouput the FX.
	ProcessHitScan_Confirmed(Impact, Origin, ShootDir, RandSeed, VSpread, HSpread);

}

void ACrystallineAssualtRifle::ProcessHitScan_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandSeed, float VSpread, float HSpread)
{
	UE_LOG(LogTemp, Log, TEXT("HITCONFIRMED"));

	// Deal Damage.
	if (ShouldDealDamage(Impact.GetActor()))
	{
		DealDamage(Impact, ShootDir);
	}
	// Play FX

	// This will trigger an OnRep that will prop to remote clients
	if (Role == ROLE_Authority)
	{
		HitNotify.Origin = Origin;
		HitNotify.RandSeed = RandSeed;
		HitNotify.VSpread = VSpread;
		HitNotify.HSpread = HSpread;
	}


	// Plays the local FX.
	if (GetNetMode() != NM_DedicatedServer)
	{
		FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : Origin + ShootDir * WeaponConfig.WeaponRange;

		// Do spawning here.
		SpawnTrailEffect(EndPoint);
		SpawnImpactEffects(Impact);
	}
}


void ACrystallineAssualtRifle::SpawnTrailEffect(const FVector& EndPoint)
{
	if (WeaponFXConfig.WeaponTrail)
	{
		UParticleSystemComponent* TrailPSC = UGameplayStatics::SpawnEmitterAtLocation(
			this, 
			WeaponFXConfig.WeaponTrail,
			GetMuzzleLocation());

		if (TrailPSC)
		{
			// Set the vector for the particle.
			TrailPSC->SetVectorParameter(WeaponFXConfig.TrailTargetParam, EndPoint);
		}
	}
}

void ACrystallineAssualtRifle::SpawnImpactEffects(const FHitResult& Impact)
{
	// TODO.
	// Check for valid impact.
	if (Impact.bBlockingHit)
	{
		FHitResult ValidImpact = Impact;

		// If the impact is invalid, we need to refire.
		if (!Impact.Component.IsValid())
		{
			// Recomputes the impact with a small trace.
			const FVector StartTrace = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;
			const FVector EndTrace = Impact.ImpactPoint - Impact.ImpactNormal * 10.0f;
			FHitResult Hit = WeaponTrace(StartTrace, EndTrace);
			ValidImpact = Hit;
		}

		// Spawn the impact effect.
		// TODO Implement the impact effect system.

	}
}

void ACrystallineAssualtRifle::SimulateHitScan(const FVector& Origin, int32 RandomSeed, float VSpread, float HSpread)
{
	FRandomStream WeaponRandomStream(RandomSeed);
	
	const FVector StartTrace = GetCameraAim();
	const FVector AimDir = GetCameraLocation();
	const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, HSpread, VSpread);
	const FVector EndTrace = StartTrace + AimDir * WeaponConfig.WeaponRange;

	// Get the Impact for the weapon trace then confirm whether or not it hit a player.
	FHitResult Impact = WeaponTrace(Origin, EndTrace);

	if (Impact.bBlockingHit)
	{
		SpawnTrailEffect(EndTrace);
		SpawnImpactEffects(Impact);
	}
	else
	{
		SpawnTrailEffect(EndTrace);
	}
}

void ACrystallineAssualtRifle::DealDamage(const FHitResult& Impact, const FVector& ShootDir)
{
	FPointDamageEvent PointDmg;
	PointDmg.DamageTypeClass = WeaponConfig.DamageType;
	PointDmg.HitInfo = Impact;
	PointDmg.ShotDirection = ShootDir;
	PointDmg.Damage = HitDamage;

	Impact.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, OwningPawn->Controller, this);

	UE_LOG(LogTemp, Log, TEXT("DAMAGE"));

}


bool ACrystallineAssualtRifle::ShouldDealDamage(AActor* TestActor) const
{
	// Only deal damage if the actor exists.
	if (TestActor)
	{
		// If this is the server, or we have authority over the other actor, OR if the actor is not being replicated.
		if (GetNetMode() != NM_Client ||
			TestActor->Role == ROLE_Authority ||
			TestActor->bTearOff)
		{
			return true;
		}
	}

	return false;
}



void ACrystallineAssualtRifle::ONRep_HitNotify()
{
	// Simulate the hit.
	SimulateHitScan(HitNotify.Origin, HitNotify.RandSeed, HitNotify.VSpread, HitNotify.HSpread);
}

void ACrystallineAssualtRifle::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// The owner doesn't need the replication because they already applied it.
	DOREPLIFETIME_CONDITION(ACrystallineAssualtRifle, HitNotify, COND_SkipOwner);
}

