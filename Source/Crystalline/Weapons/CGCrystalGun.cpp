// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "Weapons/CGAmmo.h"
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
	CrystalAmmo->AmmoInClip = CrystalAmmo->AmmoInClip - AmmoConfig.AmmoPerShot;
}

bool ACGCrystalGun::CanFire(bool InitFireCheck) const
{
	return CrystalAmmo->AmmoInClip - AmmoConfig.AmmoPerShot >= 0;
}

float ACGCrystalGun::GetClipPercent() const
{
	return (float)CrystalAmmo->AmmoInClip / AmmoConfig.ClipSize;
}

float ACGCrystalGun::GetReloadTime() const
{
	return AmmoConfig.ReloadTime;
}

bool ACGCrystalGun::GetCanReload() const
{
	// If we have CrystalAmmo->AmmoCarriedand we've actually fired something.
	return CrystalAmmo->AmmoCarried> 0 && CrystalAmmo->AmmoInClip < AmmoConfig.ClipSize;
}

void ACGCrystalGun::ApplyReload()
{
	int32 Difference = AmmoConfig.ClipSize - CrystalAmmo->AmmoInClip;
	Difference = CrystalAmmo->AmmoCarried< Difference ? CrystalAmmo->AmmoCarried: Difference;

	CrystalAmmo->AmmoCarried-= Difference;
	CrystalAmmo->AmmoInClip += Difference;
}

#pragma endregion

void ACGCrystalGun::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	// Not sure about this one.
	DOREPLIFETIME_CONDITION(ACGCrystalGun, CrystalAmmo, COND_OwnerOnly);



}