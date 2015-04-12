// Fill out your copyright notice in the Description pconst age of Project Settings.

#include "Crystalline.h"
#include "CGCrystalGun.h"



ACGCrystalGun::ACGCrystalGun(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer),
	AmmoInClip(0),
	Ammo(0)
{
}

void ACGCrystalGun::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ClipPercentPerShot = 1.f / (float)AmmoConfig.ShotsPerClip;
	FlashLevel = FMath::Floor(AmmoConfig.ShotsPerClip * AmmoConfig.AmmoPerShot * CG_PERCENT_FLASH);
}

#pragma region Ammo

void ACGCrystalGun::GiveAmmo(int32 NewAmmo)
{
	if (Role == ROLE_Authority)
	{
		Ammo = FMath::Min(AmmoConfig.AmmoCapacity, Ammo + NewAmmo);

		// Give the player enough ammo to fill up.
		int32 AmmoOverFlow = Ammo % AmmoConfig.AmmoPerShot;
		if (AmmoOverFlow > 0)
		{
			Ammo += AmmoConfig.AmmoPerShot - AmmoOverFlow;
		}
	}
}


void ACGCrystalGun::UseAmmo()
{
	if (Role == ROLE_Authority)
	{
		AmmoInClip = AmmoInClip - AmmoConfig.AmmoPerShot;
	}
}

bool ACGCrystalGun::CanFire(bool InitFireCheck) const
{
	return AmmoInClip - AmmoConfig.AmmoPerShot >= 0;
}

float ACGCrystalGun::GetClipPercent() const
{
	return (float)AmmoInClip / (AmmoConfig.ShotsPerClip * AmmoConfig.AmmoPerShot);
}

float ACGCrystalGun::GetShotsPerClip() const
{
	return AmmoConfig.ShotsPerClip;
}


float ACGCrystalGun::GetReloadTime() const
{
	return AmmoConfig.ReloadTime;
}

bool ACGCrystalGun::CanReload() const
{
	// If we have ammo and we've actually fired something.
	return Ammo > 0 && AmmoInClip < (AmmoConfig.ShotsPerClip* AmmoConfig.AmmoPerShot);
}

void ACGCrystalGun::ApplyReload()
{
	int32 Difference = (AmmoConfig.ShotsPerClip * AmmoConfig.AmmoPerShot ) - AmmoInClip;
	Difference = Ammo < Difference ? Ammo : Difference;

	UE_LOG(LogTemp, Warning, TEXT("Shots Per Clip: %d, Ammo Per Shot: %d, Ammo In Clip: %d"), AmmoConfig.ShotsPerClip, AmmoConfig.AmmoPerShot, AmmoInClip);

	Ammo -= Difference;
	AmmoInClip += Difference;

	UE_LOG(LogTemp, Warning, TEXT("AMMO AFTER RELOAD %d %d : DIFFERENCE : %d"), Ammo, AmmoInClip, Difference);

}

void ACGCrystalGun::InitializeAmmo(const FCGCrystalAmmo& AmmoStruct)
{
	Ammo = AmmoStruct.AmmoCarried;
	AmmoConfig.AmmoCapacity = AmmoStruct.MaxAmmoCarried;
	UE_LOG(LogTemp, Warning, TEXT("AMMO INITIAL %d %d"), Ammo, AmmoInClip);
	ApplyReload();

};

void ACGCrystalGun::CopyAmmo(int32 NewAmmo, int32 NewAmmoInClip)
{
	AmmoInClip = NewAmmoInClip;

	UE_LOG(LogTemp, Warning, TEXT("AMMO BEFORE COPY %d %d"), NewAmmo, NewAmmoInClip);

	UE_LOG(LogTemp, Warning, TEXT("AMMO BEFORE COPY %d %d"), Ammo, AmmoInClip);

	// TODO Fix Shotgun bug.
	// Make sure the player always has "round numbers" for ammo.
	int32 AmmoOverFlow = AmmoInClip % AmmoConfig.AmmoPerShot;
	UE_LOG(LogTemp, Warning, TEXT("Overflow: %d, InClip: %d, Per Shot: %d"), AmmoOverFlow, AmmoInClip, AmmoConfig.AmmoPerShot);
	if (AmmoOverFlow > 0)
	{
		AmmoInClip += AmmoConfig.AmmoPerShot - AmmoOverFlow;
	}

	Ammo = NewAmmo;

	// Make sure the player always has "round numbers" for ammo.
	AmmoOverFlow = Ammo % AmmoConfig.AmmoPerShot;
	UE_LOG(LogTemp, Warning, TEXT("Overflow: %d, InClip: %d, Per Shot: %d"), AmmoOverFlow, AmmoInClip, AmmoConfig.AmmoPerShot);
	if (AmmoOverFlow > 0)
	{
		Ammo += AmmoConfig.AmmoPerShot - AmmoOverFlow;
	}

	// TODO Modify so the energy doesn't exceed the Clipsize.
	const int32 Overflow = AmmoInClip - (AmmoConfig.ShotsPerClip * AmmoConfig.AmmoPerShot);
	UE_LOG(LogTemp, Warning, TEXT("Overflow: %d, InClip: %d, Per Shot: %d"), Overflow, AmmoInClip, AmmoConfig.AmmoPerShot);
	if (Overflow > 0)
	{
		AmmoInClip -= Overflow;
		Ammo += FMath::Min(Overflow, AmmoConfig.AmmoCapacity);
	}
	UE_LOG(LogTemp, Warning, TEXT("AMMO AFTER COPY %d %d"), Ammo, AmmoInClip);

}

#pragma endregion

void ACGCrystalGun::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ACGCrystalGun, Ammo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ACGCrystalGun, AmmoInClip, COND_OwnerOnly);

}