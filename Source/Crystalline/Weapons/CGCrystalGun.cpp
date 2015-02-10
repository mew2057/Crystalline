// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGCrystalGun.h"



ACGCrystalGun::ACGCrystalGun(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{

}

void ACGCrystalGun::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Ammo = AmmoConfig.AmmoCapacity;
	AmmoInClip = AmmoConfig.ClipSize;
}

#pragma region Ammo

void ACGCrystalGun::UseAmmo()
{
	AmmoInClip = AmmoInClip - AmmoConfig.AmmoPerShot;
}

bool ACGCrystalGun::CanFire() const
{
	return AmmoInClip - AmmoConfig.AmmoPerShot >= 0;
}

float ACGCrystalGun::GetClipPercent() const
{
	return AmmoInClip / AmmoConfig.ClipSize;
}

float ACGCrystalGun::GetReloadTime() const
{
	return AmmoConfig.ReloadTime;
}

bool ACGCrystalGun::GetCanReload() const
{
	return Ammo > 0;
}

void ACGCrystalGun::ApplyReload()
{
	int32 Difference = AmmoConfig.ClipSize - AmmoInClip;
	Difference = Ammo < Difference ? Ammo : Difference;

	Ammo -= Difference;
	AmmoInClip += Difference;
}

#pragma endregion

void ACGCrystalGun::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ACGCrystalGun, Ammo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ACGCrystalGun, AmmoInClip, COND_OwnerOnly);

}