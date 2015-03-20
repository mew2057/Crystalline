// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "Pickups/CGAmmoPickup.h"
#include "CGBotInventory.h"




void ACGBotInventory::DestroyInventory()
{
	Super::DestroyInventory();

	// This code is buggy and can lead to crashes.
	//FActorSpawnParameters SpawnInfo;
	//SpawnInfo.bNoCollisionFail = true;
	//ACGAmmoPickup * TempPickup;
	//
	//if (Role == ROLE_Authority)
	//{
	//	// Spawn T1
	//	TempPickup = GetWorld()->SpawnActor<ACGAmmoPickup>(
	//		AmmoPickupTemplate,
	//		CGOwner->GetActorLocation(),
	//		FRotator::ZeroRotator,
	//		SpawnInfo);
	//	TempPickup->Initialize(CGOwner, TierOneAmmo, ECGAmmoType::T_ONE);
	//
	//	// Spawn T2
	//	TempPickup = GetWorld()->SpawnActor<ACGAmmoPickup>(
	//		AmmoPickupTemplate,
	//		CGOwner->GetActorLocation(),
	//		FRotator::ZeroRotator,
	//		SpawnInfo);
	//	TempPickup->Initialize(CGOwner, TierTwoAmmo, ECGAmmoType::T_TWO);
	//}

}

void ACGBotInventory::InitializeInventory(const FCGDefaultWeaponConfig& Config)
{
	Super::InitializeInventory(Config);

	TierOneAmmo = Config.TierOneAmmoConfig.AmmoCarried;
	

}