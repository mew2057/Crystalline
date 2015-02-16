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

	/**Damage ratio relative to the Impact damage. (.5 is half the damage of the supplied impact.*/
	/*UPROPERTY(EditDefaultsOnly, Category = Projectile)
	float SplashDamageRatio;
	*/

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	float SplashRange;

protected:

	/**Spawns the radial damagae event. Plays visuals of the explosion.*/
	void Explode(const FVector& Epicenter);

	
	
	
};
