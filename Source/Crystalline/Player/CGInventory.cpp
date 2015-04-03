// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGInventory.h"

ACGInventory::ACGInventory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	TierOneCrystal = ECGCrystalType::NONE;

	bReplicates = true;
	bOnlyRelevantToOwner = true;
}

void ACGInventory::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	TierOneCrystal = ECGCrystalType::NONE;
}


void ACGInventory::InitializeInventory(const FCGDefaultWeaponConfig& Config)
{
	// Make sure only the Authority is modifying this inventory!
	if (Role < ROLE_Authority)
	{
		return;
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoCollisionFail = true;
	CrystalGunIndex = 0;

	// Add the Core gun.
	if (Config.CoreWeapon)
	{
		AddToWeapons(GetWorld()->SpawnActor<ACGWeapon>(Config.CoreWeapon, SpawnInfo));
		CrystalGunIndex++;
	}

	// Add a weapon slot for the crystal gun.
	Weapons.AddZeroed();

	// NOTE: All guns from this point on are assumed to be crystal guns.
	ACGCrystalGun* TempCrystalGun;
	if (Config.CoreCrystalGun)
	{
		TempCrystalGun = GetWorld()->SpawnActor<ACGCrystalGun>(Config.CoreCrystalGun, SpawnInfo);
		TempCrystalGun->InitializeAmmo(Config.TierZeroAmmoConfig);
		AddToWeaponMap(TempCrystalGun); // Assumes the gun to have no typing.
	}
		
	const int32 NumWeaponGroups = Config.CrystalGunGroups.Num();
	for (int i = 0; i < NumWeaponGroups; ++i)
	{
		// If it exists spawn the weapon.
		if (Config.CrystalGunGroups[i].TierOneGun)
		{
			// Add the Tier OneGun
			TempCrystalGun = GetWorld()->SpawnActor<ACGCrystalGun>(Config.CrystalGunGroups[i].TierOneGun, SpawnInfo);
			TempCrystalGun->InitializeAmmo(Config.TierOneAmmoConfig);
			AddToWeaponMap(TempCrystalGun, Config.CrystalGunGroups[i].TierOneCrystalType);
		}
	}

	// Constructs the inventory for the player.
	ReconstructInventory();
}

void ACGInventory::AddToWeapons(ACGWeapon* Weapon)
{
	// If the weapon wasn't set and we're not the server, don't bother.
	if (Weapon == NULL || Role < ROLE_Authority)
	{
		return;
	}

	Weapons.AddUnique(Weapon);
}

void ACGInventory::AddToWeaponMap(ACGWeapon* Weapon, ECGCrystalType Type)
{
	// If the weapon wasn't set and we're not the server, don't bother.
	if (Weapon == NULL || Role < ROLE_Authority)
	{
		return;
	}

	// If the weapon is not contained in the map, create an array and add it.
	if (!WeaponGroups.Contains(Type))
	{
		TArray<class ACGWeapon*> NewWeapons;
		WeaponGroups.Add(Type);
	}
	
	// If the ammo type is not none and doesn't exist in our cache add it to the cache map.
	if (Weapon->WeaponConfig.AmmoType > ECGAmmoType::NONE && !AmmoCache.Contains(Weapon->WeaponConfig.AmmoType))
	{
		AmmoCache.Add(Weapon->WeaponConfig.AmmoType);
	}

	// Adds the weapon to the WeaponGroups.
	WeaponGroups[Type].AddUnique(Weapon);

	// Set the owner of the weapon to the Character.
	Weapon->OnEnterInventory(CGOwner);	
}

void ACGInventory::DestroyInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	// Destruction code.
	ACGWeapon* CachedWeapon;
	
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoCollisionFail = true;
	ACGAmmoPickup * TempPickup;

	// Destroy inventory weapons.
	for (int32 i = Weapons.Num() - 1; i >= 0; --i)
	{
		CachedWeapon = Weapons[i];
		if (CachedWeapon)
		{
			// TODO refactor, this is a quick and dirty ammo pickup spawner.
			if (AmmoPickupTemplate && CachedWeapon->WeaponConfig.AmmoType > ECGAmmoType::NONE)
			{
				TempPickup = GetWorld()->SpawnActor<ACGAmmoPickup>(
					AmmoPickupTemplate, 
					CGOwner->GetActorLocation(),
					FRotator::ZeroRotator,
					SpawnInfo);

				// Sanity check, crashes on round restart.
				if (TempPickup)
				{
					TempPickup->Initialize(CGOwner, CachedWeapon->GetDroppableAmmo(), CachedWeapon->WeaponConfig.AmmoType);
				}
			}
			 

			Weapons.RemoveAt(i);
			CachedWeapon->OnExitInventory();
			CachedWeapon->Destroy();
		}
	}

	// Clear the weapon groups.
	TArray<ECGCrystalType> Keys;
	TArray<ACGWeapon*> Values;
	WeaponGroups.GetKeys(Keys);
	for (int32 i = Keys.Num() - 1; i >= 0; --i)
	{
		Values = WeaponGroups[Keys[i]];

		for (int32 j = Values.Num() - 1; j >= 0; --j)
		{
			CachedWeapon = Values[j];
			if (CachedWeapon)
			{
				Values.RemoveAt(j);
				CachedWeapon->OnExitInventory();
				CachedWeapon->Destroy();
			}
		}
		WeaponGroups.Remove(Keys[i]);
	}
}

void ACGInventory::ReconstructInventory()
{
	// Only server has the right to reconstruct this!
	if (Role < ROLE_Authority)
	{
		return;
	}

	// Ensure that the crystal was defined.
	ACGCrystalGun* CurrentCrystalWeapon = Cast<ACGCrystalGun>(Weapons[CrystalGunIndex]);

	// If the ammo type is valid set the cached ammo.
	if (CurrentCrystalWeapon && AmmoCache.Contains(CurrentCrystalWeapon->WeaponConfig.AmmoType))
	{
		AmmoCache[CurrentCrystalWeapon->WeaponConfig.AmmoType].Ammo = CurrentCrystalWeapon->GetActualAmmo();
		AmmoCache[CurrentCrystalWeapon->WeaponConfig.AmmoType].AmmoInClip = CurrentCrystalWeapon->GetActualAmmoInClip();
	}

	ACGCrystalGun* NewWeapon = WeaponGroups.Contains(TierOneCrystal) ? Cast<ACGCrystalGun>(WeaponGroups[TierOneCrystal][0]) : NULL;

	// If a new weapon was present attempt to copy the ammo and move the new weapon to the crystalgun index.
	if (NewWeapon != NULL)
	{
		if (AmmoCache.Contains(NewWeapon->WeaponConfig.AmmoType) &&	AmmoCache[NewWeapon->WeaponConfig.AmmoType].Ammo >= 0)
		{
			NewWeapon->CopyAmmo(AmmoCache[NewWeapon->WeaponConfig.AmmoType].Ammo, AmmoCache[NewWeapon->WeaponConfig.AmmoType].AmmoInClip);
		}

		Weapons[CrystalGunIndex] = NewWeapon; 
	}
	else
	{
		// Fail!
	}

	// Equip the best possible weapon for the player.
	if (Weapons.Num() > 0)
	{
		CGOwner->EquipWeapon(Weapons[Weapons.Num() - 1]);
	}
}

bool ACGInventory::CanLoadCrystal(ECGCrystalType Crystal)
{
	return Crystal != ECGCrystalType::NONE &&
		(Crystal > ECGCrystalType::POWER_UP && TierOneCrystal != Crystal);
}

void ACGInventory::LoadCrystal(ECGCrystalType Crystal)
{
	// Tier1 crystal
	if (Crystal > ECGCrystalType::POWER_UP && TierOneCrystal != Crystal)
	{
		TierOneCrystal = Crystal;
		ReconstructInventory();
	}
}

void ACGInventory::PopAllCrystals()
{
	// Clear the Crystals and rebuild inventory.
	TierOneCrystal = ECGCrystalType::NONE;

	ReconstructInventory();
}

void ACGInventory::PopBestCrystal()
{
	if (TierOneCrystal != ECGCrystalType::NONE)
	{
		TierOneCrystal = ECGCrystalType::NONE;
		ReconstructInventory();
	}
}


void ACGInventory::SetCGOwner(ACGCharacter* NewOwner)
{
	if (CGOwner != NewOwner)
	{
		CGOwner = NewOwner;
		Instigator = NewOwner;

		SetOwner(CGOwner);
	}
}

void ACGInventory::OnRep_CGOwner()
{
	// TODO Replicate the inventory being loaded.
}

bool ACGInventory::GiveAmmo(ECGAmmoType AmmoType, int32 Ammo)
{
	bool bFound = false;
	for (int32 i = Weapons.Num() - 1; i >= 0; --i)
	{
		// TODO Make this reject if the player doesn't benefit from the pickup.
		if (Weapons[i]->WeaponConfig.AmmoType == AmmoType)
		{
			Weapons[i]->GiveAmmo(Ammo);
			bFound = true;
			break;
		}
	}

	return bFound;
}

void ACGInventory::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ACGInventory, TierOneCrystal, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ACGInventory, Weapons, COND_OwnerOnly);
	
	DOREPLIFETIME(ACGInventory, CGOwner); // This may not be necessary.
}


