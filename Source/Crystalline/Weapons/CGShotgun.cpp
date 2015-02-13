// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "Projectiles/CGProjectile.h"
#include "CGShotgun.h"

void ACGShotgun :: SpawnProjectile(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{

	int32 FireSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(FireSeed);
	ACGProjectile* Bullet;
	
	for (int32 i = 0; i < BulletsPerShot; ++i)
	{
		
		const FVector InitialRotation = WeaponRandomStream.VRandCone(ShootDir, CurrentSpread, CurrentSpread);
		const FTransform BulletSpawn(InitialRotation.Rotation(), Origin);

		Bullet = Cast<ACGProjectile>(UGameplayStatics::BeginSpawningActorFromClass(this, ProjectileConfig.ProjectileClass, BulletSpawn));

		if (Bullet)
		{
			Bullet->Instigator = Instigator;
			Bullet->SetOwner(this);
			Bullet->SetVelocity(InitialRotation); // This ensures the behavior matches it's intended use case.
			Bullet->ImpactDamage = WeaponConfig.BaseDamage;
			UGameplayStatics::FinishSpawningActor(Bullet, BulletSpawn);
			Bullet = NULL;
		}
	}
	
}
