// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGRocketProjectile.h"

ACGRocketProjectile::ACGRocketProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SplashRange = 500.f;
}



void ACGRocketProjectile::ProcessImpact(const FHitResult& Hit)
{
//	Super::ProcessImpact(const FHitResult& Hit);

	// Moves the origin of the explosion out of the wall.
	FVector ExplosionPoint = Hit.ImpactPoint + Hit.ImpactNormal * 10.f;

	// Apply radial damage at the impact point.
	Explode(ExplosionPoint, SplashRange);
}

void ACGRocketProjectile::Explode(const FVector& Epicenter,float Range)
{
	// TODO DamageType!
	UGameplayStatics::ApplyRadialDamage(this, 
		ImpactDamage, Epicenter, Range, NULL, TArray<AActor*>(), this, GetInstigatorController());

	// TODO  Add Impulse!

	// TODO Play Effect!


}

void ACGRocketProjectile::PlannedExplosion()
{
	// Only the Authority can trigger a planned explosion.
	if (Role == ROLE_Authority)
	{
		Explode(GetActorLocation, SplashRange * 2);
		PrepForDestroy();
	}
}