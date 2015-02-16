// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/CGCrystalGun.h"
#include "CGBeamGun.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGBeamGun : public ACGCrystalGun
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(Transient, Replicated)
	TSubclassOf<class AActor> Target;

	/** The maximum angle that the beam can be pointed away from it's target.*/
	UPROPERTY(EditDefaultsOnly, Category=Config)
	float MaxAngle;
	
};
