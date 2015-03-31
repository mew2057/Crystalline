// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/CGCrystalGun.h"
#include "CGMarksmanRifle.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGMarksmanRifle : public ACGCrystalGun
{
	GENERATED_BODY()

	ACGMarksmanRifle(const FObjectInitializer& ObjectInitializer);
	
	virtual void DealDamage_Instant(const FHitResult& Impact, const FVector& ShootDir) override;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float HeadShotModifier;
	
	
};
