// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGMarksmanRifle.h"


ACGMarksmanRifle::ACGMarksmanRifle(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WeaponConfig.AmmoType = ECGAmmoType::T_ONE;
}