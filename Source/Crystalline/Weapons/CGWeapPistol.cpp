// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "Weapons/States/CGWeaponReloadOverheatState.h"
#include "CGWeapPistol.h"



ACGWeapPistol::ACGWeapPistol(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WeaponConfig.bUsesProjectile = true;
	WeaponConfig.bHasManualReload = false;

	ReloadingState = ObjectInitializer.CreateDefaultSubobject<UCGWeaponReloadOverheatState>(this, TEXT("StateOverheating"));
}

void ACGWeapPistol::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	WeaponHeat = 0;
	ClipPercentPerShot = OverheatConfig.HeatPerShot / OverheatConfig.MaxHeat;
}


void ACGWeapPistol::Tick(float DeltaSeconds)
{
	if (!bIsOverheated)
		WeaponHeat = FMath::Max(0.f, WeaponHeat - (OverheatConfig.CooldownPerSecond * DeltaSeconds));

	Super::Tick(DeltaSeconds);

}


#pragma region Ammo

void ACGWeapPistol::UseAmmo()
{
	WeaponHeat = FMath::Min(WeaponHeat + OverheatConfig.HeatPerShot, OverheatConfig.MaxHeat);
}

bool ACGWeapPistol::CanFire(bool InitFireCheck) const
{
	// XXX Clean this code up.
	// TODO is this a good check?
	if (CurrentState == ReloadingState)
	{
		return WeaponHeat == 0.f;
	}
	else
	{
		// XXX find a better way!
		return WeaponHeat + OverheatConfig.HeatPerShot <= OverheatConfig.MaxHeat || InitFireCheck;
	}	
}

float ACGWeapPistol::GetClipPercent() const
{
	return  WeaponHeat / OverheatConfig.MaxHeat;
}

// This gun can only reload if it can't fire.
bool ACGWeapPistol::CanReload() const
{
	return WeaponHeat + OverheatConfig.HeatPerShot >= OverheatConfig.MaxHeat;
}

void ACGWeapPistol::StartOverheat()
{
	bIsOverheated = true;

	GetWorldTimerManager().SetTimer(TimerHandle_Overheat, this,
		&ACGWeapPistol::BeginCooldown,
		OverheatConfig.OverheatTime, false);
}

void ACGWeapPistol::BeginCooldown()
{
	bIsOverheated = false;
	GetWorldTimerManager().ClearTimer(TimerHandle_Overheat);
}

#pragma endregion

void ACGWeapPistol::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ACGWeapPistol, WeaponHeat, COND_OwnerOnly);
}