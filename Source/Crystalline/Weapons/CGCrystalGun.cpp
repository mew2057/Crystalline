// Fill out your copyright notice in the Description pconst age of Project Settings.

#include "Crystalline.h"
#include "CGCrystalGun.h"



ACGCrystalGun::ACGCrystalGun(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{

}

void ACGCrystalGun::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ClipPercentPerShot = 1 / AmmoConfig.ShotsPerClip;
}

#pragma region Ammo

void ACGCrystalGun::GiveAmmo(int32 NewAmmo)
{
	UE_LOG(LogTemp, Log, TEXT("Give Ammo %d"), NewAmmo);

	Ammo = FMath::Min(AmmoConfig.AmmoCapacity, Ammo + NewAmmo);
}


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
	return (float)(AmmoInClip * AmmoConfig.AmmoPerShot) / AmmoConfig.ShotsPerClip;
}

float ACGCrystalGun::GetReloadTime() const
{
	return AmmoConfig.ReloadTime;
}

bool ACGCrystalGun::CanReload() const
{
	// If we have ammo and we've actually fired something.
	return Ammo > 0 && (AmmoInClip * AmmoConfig.AmmoPerShot) < AmmoConfig.ShotsPerClip;
}

void ACGCrystalGun::ApplyReload()
{
	int32 Difference = (AmmoConfig.ShotsPerClip * AmmoConfig.AmmoPerShot ) - AmmoInClip;
	Difference = Ammo < Difference ? Ammo : Difference;

	Ammo -= Difference;
	AmmoInClip += Difference;
}

void ACGCrystalGun::InitializeAmmo(const FCGCrystalAmmo& AmmoStruct)
{
	Ammo = AmmoStruct.AmmoCarried;
	AmmoConfig.AmmoCapacity = AmmoStruct.MaxAmmoCarried;

	ApplyReload();
};

void ACGCrystalGun::CopyAmmo(const ACGCrystalGun* Other)
{
	AmmoInClip = Other->AmmoInClip;
	Ammo = Other->Ammo;

	// TODO Modify so the energy doesn't exceed the Clipsize.
	const int32 Overflow = AmmoInClip - (AmmoConfig.ShotsPerClip * AmmoConfig.AmmoPerShot);
	if (Overflow > 0)
	{
		AmmoInClip -= Overflow;
		Ammo += FMath::Min(Overflow, AmmoConfig.AmmoCapacity);
	}
}

#pragma endregion

void ACGCrystalGun::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ACGCrystalGun, Ammo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ACGCrystalGun, AmmoInClip, COND_OwnerOnly);

}