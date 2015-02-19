// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGCrystalGun.h"



ACGCrystalGun::ACGCrystalGun(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{

}

void ACGCrystalGun::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ClipPercentPerShot = AmmoConfig.AmmoPerShot / AmmoConfig.ClipSize;
}

#pragma region Ammo

void ACGCrystalGun::UseAmmo()
{
	AmmoInClip = AmmoInClip - AmmoConfig.AmmoPerShot;
}

bool ACGCrystalGun::CanFire(bool InitFireCheck) const
{
	return AmmoInClip - AmmoConfig.AmmoPerShot >= 0;
}

float ACGCrystalGun::GetClipPercent() const
{
	return (float)AmmoInClip / AmmoConfig.ClipSize;
}

float ACGCrystalGun::GetReloadTime() const
{
	return AmmoConfig.ReloadTime;
}

bool ACGCrystalGun::CanReload() const
{
	// If we have ammo and we've actually fired something.
	return Ammo > 0 && AmmoInClip < AmmoConfig.ClipSize;
}

void ACGCrystalGun::ApplyReload()
{
	int32 Difference = AmmoConfig.ClipSize - AmmoInClip;
	Difference = Ammo < Difference ? Ammo : Difference;

	Ammo -= Difference;
	AmmoInClip += Difference;
}

void ACGCrystalGun::InitializeAmmo(const FCGCrystalAmmo& AmmoStruct)
{
	AmmoInClip = AmmoStruct.AmmoInClip;
	Ammo = AmmoStruct.AmmoCarried;
	AmmoConfig.AmmoCapacity = AmmoStruct.MaxAmmoCarried;

	//XXX Should this exist?
	// Reload the gun if the base ammo isn't set.
	if (AmmoInClip <= 0)
	{
		ApplyReload();
	}
};

void ACGCrystalGun::CopyAmmo(const ACGCrystalGun* Other)
{
	AmmoInClip = Other->AmmoInClip;
	Ammo = Other->Ammo;
}

#pragma endregion

void ACGCrystalGun::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ACGCrystalGun, Ammo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ACGCrystalGun, AmmoInClip, COND_OwnerOnly);

}