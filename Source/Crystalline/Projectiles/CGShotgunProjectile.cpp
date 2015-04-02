// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGShotgunProjectile.h"

ACGShotgunProjectile::ACGShotgunProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EffectiveDistance = 1250.f;
	OutOfRangeModifier = .25f;
}

void ACGShotgunProjectile::ProcessImpact(const FHitResult& Hit)
{
	if (Hit.GetActor())
	{
		FPointDamageEvent PointDmg;
		PointDmg.DamageTypeClass = DamageType;
		PointDmg.HitInfo = Hit;
		PointDmg.ShotDirection = Hit.ImpactNormal;

		// Hard drop off for damage at the extent of the effective range.
		const float EstDistTravelled = MovementComp->InitialSpeed * (GetWorld()->GetTimeSeconds() - CreationTime);
		PointDmg.Damage = ImpactDamage * (EstDistTravelled < EffectiveDistance ? 1 : OutOfRangeModifier);

		Hit.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, GetInstigatorController(), this);
	}
}