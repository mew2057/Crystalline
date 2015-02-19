// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "Projectiles/CGProjectile.h"
#include "CGShotgun.h"

ACGShotgun::ACGShotgun(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WeaponConfig.AmmoType = ECGAmmoType::T_ONE;
}

void ACGShotgun :: SpawnProjectile(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{

	int32 FireSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(FireSeed);
	ACGProjectile* SGBullet;

	/**	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoCollisionFail = true;
	*/
	for (int32 i = 0; i < BulletsPerShot; ++i)
	{
		
		const FVector InitialRotation = WeaponRandomStream.VRandCone(ShootDir, GetCurrentSpread());
		const FTransform BulletSpawn(InitialRotation.Rotation(), Origin);

		SGBullet = Cast<ACGProjectile>(UGameplayStatics::BeginSpawningActorFromClass(this, ProjectileConfig.ProjectileClass, BulletSpawn, true));

		if (SGBullet)
		{
			SGBullet->Instigator = Instigator;
			SGBullet->SetOwner(this);
			SGBullet->SetVelocity(InitialRotation); // This ensures the behavior matches it's intended use case.
			SGBullet->ImpactDamage = WeaponConfig.BaseDamage;
			SGBullet->SetLifeSpan(ProjectileConfig.ProjectileLife);
			UGameplayStatics::FinishSpawningActor(SGBullet, BulletSpawn);
			SGBullet = NULL;
		}
	}
	
}
