// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Projectiles/CGProjectile.h"
#include "CGShotgunProjectile.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGShotgunProjectile : public ACGProjectile
{
	GENERATED_BODY()

public:
	
	ACGShotgunProjectile(const FObjectInitializer& ObjectInitializer);

	virtual void ACGShotgunProjectile::ProcessImpact(const FHitResult& Hit) override;

protected:

	/**The Effective Distance of the shotgun bullet.*/
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	float EffectiveDistance;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	float OutOfRangeModifier;
	
};
