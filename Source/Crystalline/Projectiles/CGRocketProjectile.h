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
	/** Processes the impact with projectile specific implementation details. */
	virtual void ProcessImpact(const FHitResult& Hit) override;

	

protected:

	
	
	
	
};
