// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "Weapons/States/CGWeaponState.h"
#include "Weapons/States/CGWeaponActiveState.h"
#include "Weapons/States/CGWeaponInactiveState.h"
#include "Weapons/States/CGWeaponEquippingState.h"
#include "Weapons/States/CGWeaponUnequippingState.h"
#include "Weapons/States/CGWeaponReloadingState.h"
#include "Weapons/States/CGWeaponFiringState.h"

#include "CGWeapon.h"

ACGWeapon::ACGWeapon(const FObjectInitializer& PCIP) : Super(PCIP)
{

	ActiveState      = PCIP.CreateDefaultSubobject<UCGWeaponActiveState>(this, TEXT("StateActive"));
	InactiveState    = PCIP.CreateDefaultSubobject<UCGWeaponInactiveState>(this, TEXT("StateInactive"));
	EquippingState   = PCIP.CreateDefaultSubobject<UCGWeaponEquippingState>(this, TEXT("StateEquipping"));
	UnequippingState = PCIP.CreateDefaultSubobject<UCGWeaponUnequippingState>(this, TEXT("StateUnequipping"));
	ReloadingState   = PCIP.CreateDefaultSubobject<UCGWeaponReloadingState>(this, TEXT("StateReloading"));
	FiringState      = PCIP.CreateDefaultSubobject<UCGWeaponFiringState>(this, TEXT("StateFiring"));
	

}


void ACGWeapon::PostInitializeComponents()
{


}


#pragma region Set Functions
void ACGWeapon::SetCGOwner(ACGCharacter* NewOwner)
{
	if (CGOwner != NewOwner)
	{
		CGOwner = NewOwner;
		SetOwner(CGOwner);
	}
}

#pragma endregion


////////////////////////////////////////////////////////////////////////////////////////

#pragma region State Functions

void ACGWeapon::OnEnterInventory(ACGCharacter* NewOwner)
{
	SetCGOwner(NewOwner);
}

void ACGWeapon::OnExitInventory()
{
	// If this has authorative control, remove null the pawn.
	if (Role == ROLE_Authority)
	{
		SetCGOwner(NULL);
	}
	//TODO Remove attachment.

}

void ACGWeapon::OnEquip()
{
	// Timer and reload.
	//AttachMeshToPawn();
	// TODO add switch animation id appropriate.
}

void ACGWeapon::OnUnEquip()
{
	//DetachMeshFromPawn();
}

void ACGWeapon::StartFire()
{
	/*
	if (Role < ROLE_Authority)
	{
		ServerStartFire();
	}

	// Firing is set to be true.
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		// TODO state update
		StartBurst();
	}*/
}

void ACGWeapon::StopFire()
{
	/**
	bWantsToFire = false;
	StopBurst();
	*/
}

#pragma endregion

void ACGWeapon::GotoState(UCGWeaponState* NewState)
{
	// STATE transition happens here.
}




void ACGWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACGWeapon, CGOwner);
}

void ACGWeapon::OnRep_CGOwner() 
{ 
	CGOwner ? OnEnterInventory(CGOwner) : OnExitInventory(); 
}
