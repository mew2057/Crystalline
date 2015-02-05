// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CrystallinePistol.h"

ACrystallinePistol::ACrystallinePistol(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsOverheated = false;
	bIsCoolingDown = false;
	WeaponHeat    = 0.f;
}

void ACrystallinePistol::FireWeapon()
{
	FVector StartTrace;
	FVector AimDir;
	GetCameraDetails(StartTrace, AimDir);

	FVector EndTrace = StartTrace + AimDir * WeaponRange;
	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);


	// This needs to be more robust for multi mesh solutions.
	FVector Origin = GetMuzzleLocation();
	FVector Direction = AimDir;

	if (Impact.bBlockingHit)
	{
		FVector MuzzleDir = (Impact.ImpactPoint - Origin).SafeNormal();

		bool bIntersecting = false;
		float GunDot = FVector::DotProduct(MuzzleDir, Direction);

		// If it's less than zero we're penetrating.
		if (GunDot < 0.0f)
		{
			bIntersecting = true;
		}
		else if (GunDot < 0.5f) // If there's an angle there's a chance of penetration
		{
			// TOOD can shoot through walls in doorways.
			// XXX the 150 may change.
			FVector WeaponTraceStart = Origin - GetMuzzleRotation() * 200.f;

			bIntersecting = WeaponTrace(WeaponTraceStart, Origin).bBlockingHit;

			// Basically the check finds the gun intersection.
			//				____________	
			//				|/
			//			    / (Gun)
			//	(Gun Inter)*|
			//				|
			//				|
		}

		if (bIntersecting)
		{
			Origin = Impact.ImpactPoint;
		}
		else 
		{
			Direction = MuzzleDir;
		}

	}

	ServerFireProjectile(Origin, Direction);
}

void ACrystallinePistol::UseAmmo()
{
	WeaponHeat = FMath::Min(WeaponHeat + ProjectileConfig.HeatPerShot, ProjectileConfig.MaxHeat);

	// May not need the overheated thing, but this is me trying to be sure we don't get too many timers running.
	if (WeaponHeat == ProjectileConfig.MaxHeat && !bIsOverheated)
	{

		bIsOverheated  = true;
		bIsCoolingDown = false;


		GetWorldTimerManager().SetTimer(this, 
			&ACrystallinePistol::HandleOverheatCooldown, 
			ProjectileConfig.OverheatTime, false);
	}
}

void ACrystallinePistol::HandleOverheatCooldown()
{
	bIsOverheated = false; 
	bIsCoolingDown = true;

	GetWorldTimerManager().SetTimer(this,
		&ACrystallinePistol::FinishCooldown,
		ProjectileConfig.MaxHeat/ProjectileConfig.CooldownPerSecond, false);
}


void ACrystallinePistol::FinishCooldown()
{
	bIsCoolingDown = false;
}


void ACrystallinePistol::Tick(float DeltaSeconds)
{
	if (!bIsOverheated)
		WeaponHeat = FMath::Max(0.f,WeaponHeat - (ProjectileConfig.CooldownPerSecond * DeltaSeconds));
}


bool ACrystallinePistol::CanFire()
{
	return !(bIsOverheated || bIsCoolingDown);
}

bool ACrystallinePistol::ServerFireProjectile_Validate(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
	return true;
}


void ACrystallinePistol::ServerFireProjectile_Implementation(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
	// Determine the spawn point and create a bullet to fire.
	FTransform BulletSpawn(ShootDir.Rotation(), Origin);
	ACrystallineProjectile* Bullet = Cast<ACrystallineProjectile>(
		UGameplayStatics::BeginSpawningActorFromClass(this, ProjectileConfig.ProjectileClass, BulletSpawn));

	if (Bullet)
	{
		Bullet->Instigator = Instigator;		
		Bullet->SetOwner(this);
		Bullet->SetVelocity(ShootDir); // This ensures the behavior matches it's intended use case.
		UGameplayStatics::FinishSpawningActor(Bullet, BulletSpawn);
	}
}


float ACrystallinePistol::GetClipPercent() const
{
	return WeaponHeat / ProjectileConfig.MaxHeat;
}


