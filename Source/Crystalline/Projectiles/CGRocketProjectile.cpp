// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGRocketProjectile.h"




void ACGRocketProjectile::ProcessImpact(const FHitResult& Hit)
{
	// Radial Damage needs to be applied.
	/*
	if (Hit.GetActor())
	{
		FPointDamageEvent PointDmg;
		//PointDmg.DamageTypeClass = WeaponConfig.DamageType;
		PointDmg.HitInfo = Hit;		
		PointDmg.ShotDirection = Hit.ImpactNormal;
		PointDmg.Damage = ImpactDamage; // This needs to move.


		Hit.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, GetInstigatorController(), this);

		FPointDamageEvent RadialDamage;
	}*/
}
