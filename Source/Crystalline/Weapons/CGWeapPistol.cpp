// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "Weapons/States/CGWeaponReloadOverheatState.h"
#include "CGWeapPistol.h"



ACGWeapPistol::ACGWeapPistol(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WeaponConfig.bUsesProjectile = true;
	ReloadingState = ObjectInitializer.CreateDefaultSubobject<UCGWeaponReloadOverheatState>(this, TEXT("StateOverheating"));

}

void ACGWeapPistol::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	WeaponHeat = 0;
}


void ACGWeapPistol::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bIsOverheated)
		WeaponHeat = FMath::Max(0.f, WeaponHeat - (OverheatConfig.CooldownPerSecond * DeltaSeconds));
}


#pragma region Ammo

void ACGWeapPistol::UseAmmo()
{
	WeaponHeat = FMath::Min(WeaponHeat + OverheatConfig.HeatPerShot, OverheatConfig.MaxHeat);
}

bool ACGWeapPistol::CanFire() const
{
	// TODO is this a good check?
	if (CurrentState == ReloadingState)
	{
		return WeaponHeat == 0.f;
	}
	else
	{
		return WeaponHeat + OverheatConfig.HeatPerShot <= OverheatConfig.MaxHeat;
	}
	
}

float ACGWeapPistol::GetClipPercent() const
{
	return  WeaponHeat / OverheatConfig.MaxHeat;
}

void ACGWeapPistol::StartCooldown()
{
	bIsOverheated = true;
}

void ACGWeapPistol::EndCooldown()
{
	bIsOverheated = false;
}

#pragma endregion