// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGShotgunProjectile.h"

ACGShotgunProjectile::ACGShotgunProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EffectiveDistance = 1250.f;
	OutOfRangeModifier = .25f;
}

float ACGShotgunProjectile::GetPointDamage()
{
	// Compute the estimated distance travelled by this projectile (specifics don't matter in this case).
	const float EstDistTravelled = MovementComp->InitialSpeed * (GetWorld()->GetTimeSeconds() - CreationTime);

	// Adjust the damage based on whether or not the hit is out of the shotgun's range.
	return ImpactDamage * (EstDistTravelled < EffectiveDistance ? 1 : OutOfRangeModifier);
}