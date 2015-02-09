// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGCrystalGun.h"



ACGCrystalGun::ACGCrystalGun(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{

}

void ACGCrystalGun::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

#pragma region Ammo

void ACGCrystalGun::UseAmmo()
{
}

bool ACGCrystalGun::CanFire() const
{
	return false;
}

float ACGCrystalGun::GetClipPercent() const
{
	return 0.f;
}

#pragma endregion