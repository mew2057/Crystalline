// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/CGWeapon.h"
#include "CGWeapAssaultRifle.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class CRYSTALLINE_API ACGWeapAssaultRifle : public ACGCrystalGun
{
	GENERATED_BODY()
public:
	ACGWeapAssaultRifle(const FObjectInitializer& ObjectInitializer);

};
