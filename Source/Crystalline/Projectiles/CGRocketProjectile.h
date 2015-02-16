// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Projectiles/CGProjectile.h"
#include "CGRocketProjectile.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGRocketProjectile : public ACGProjectile
{
	GENERATED_BODY()

public:

	ACGRocketProjectile(const FObjectInitializer& ObjectInitializer);

	/** Processes the impact with projectile specific implementation details. */
	virtual void ProcessImpact(const FHitResult& Hit) override;
	
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	float SplashRange;
	
protected:

	/**Spawns the radial damagae event. Plays visuals of the explosion.*/
	void Explode(const FVector& Epicenter, float Range);

	/** Premature explosion, occurs at double the effective range, but commits the user to one shot.*/
	void PlannedExplosion();	
};
