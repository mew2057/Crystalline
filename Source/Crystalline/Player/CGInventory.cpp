// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGInventory.h"

ACGInventory::ACGInventory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	TierOneCrystal = ECGCrystalType::NONE;
	TierTwoCrystal = ECGCrystalType::NONE;

	bReplicates = true;
	bOnlyRelevantToOwner = true;
	
}

void ACGInventory::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	TierOneCrystal = ECGCrystalType::NONE;
	TierTwoCrystal = ECGCrystalType::NONE;
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

	if (Config.CoreWeapon)
	{
		AddWeapon(GetWorld()->SpawnActor<ACGWeapon>(Config.CoreWeapon, SpawnInfo));
	}

	// Note all guns from this point on are assumed to be crystal guns.
	ACGCrystalGun* TempCrystalGun;
	if (Config.CoreCrystalGun)
	{
		TempCrystalGun = GetWorld()->SpawnActor<ACGCrystalGun>(Config.CoreCrystalGun, SpawnInfo);
		TempCrystalGun->InitializeAmmo(Config.TierZeroAmmoConfig);
		AddWeapon(TempCrystalGun);
	}

	// Get the number of weaopns that were properly defined.
	StaticWeaponCount = Weapons.Num();
	
	const int32 NumWeaponGroups = Config.CrystalGunGroups.Num();
	for (int i = 0; i < NumWeaponGroups; ++i)
	{
		// If it exists spawn the weapon.
		if (Config.CrystalGunGroups[i].TierOneGun)
		{
			// Add the Tier OneGun
			TempCrystalGun = GetWorld()->SpawnActor<ACGCrystalGun>(Config.CrystalGunGroups[i].TierOneGun, SpawnInfo);
			TempCrystalGun->InitializeAmmo(Config.TierOneAmmoConfig);
			AddWeapon(TempCrystalGun, Config.CrystalGunGroups[i].TierOneCrystalType);
		}

		if (Config.CrystalGunGroups[i].TierTwoGun)
		{
			// Add the TierTwo Gun
			TempCrystalGun = GetWorld()->SpawnActor<ACGCrystalGun>(Config.CrystalGunGroups[i].TierTwoGun, SpawnInfo);
			TempCrystalGun->InitializeAmmo(Config.TierTwoAmmoConfig);
			AddWeapon(TempCrystalGun, Config.CrystalGunGroups[i].TierOneCrystalType);
		}
	}

	// Constructs the inventory for the player.
	ReconstructInventory();
}

void ACGInventory::AddWeapon(ACGWeapon* Weapon, ECGCrystalType Type)
{
	// If the weapon wasn't set and we're not the server, don't bother.
	if (Weapon == NULL || Role < ROLE_Authority)
	{
		return;
	}

	// If no type is specified, assume that it's a default weapon.
	if (Type == ECGCrystalType::NONE)
	{
		Weapons.AddUnique(Weapon);
	}
	else
	{
		if (!WeaponGroups.Contains(Type))
		{
			TArray<class ACGWeapon*> NewWeapons;
			WeaponGroups.Add(Type);
		}

		WeaponGroups[Type].AddUnique(Weapon);
	}

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

			// TODO refactor, this is a quick and dirty ammo pickuip spawner.
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

	// Flag that the weapon group actually exists.
	bool bTierOneDefined = WeaponGroups.Contains(TierOneCrystal);

	// If the tier is defined copy what we can.
	if (bTierOneDefined )
	{
		const int32 WeaponCount = Weapons.Num();
		const int32 GroupCount = WeaponGroups[TierOneCrystal].Num();
		ACGCrystalGun * ReceivingGun;
		ACGCrystalGun * TempGun;
		for (int32 i = StaticWeaponCount, j = 0; i < WeaponCount && j < GroupCount; ++i, ++j)
		{
			// XXX this may be overkill, but I'm a bit overly cautious.
			ReceivingGun = Cast<ACGCrystalGun>(WeaponGroups[TierOneCrystal][j]);
			TempGun = Cast<ACGCrystalGun>(Weapons[i]);
			if (ReceivingGun && TempGun)
			{
				ReceivingGun->CopyAmmo(TempGun);
			}
		}
	}

	// Clear the elements that need to be cleared.
	Weapons.RemoveAt(StaticWeaponCount, Weapons.Num() - StaticWeaponCount, true);

	if (bTierOneDefined)
	{
		// Determine the number of weapons to be transfered over.
		int32 CopyLength = FMath::Min(WeaponGroups[TierOneCrystal].Num(), TierTwoCrystal == ECGCrystalType::NONE ? 1 : 2);

		for (int32 i = 0; i < CopyLength; ++i)
		{
			Weapons.AddUnique(WeaponGroups[TierOneCrystal][i]);
		}
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
		((Crystal > ECGCrystalType::POWER_UP && TierOneCrystal != Crystal) ||
		(Crystal <= ECGCrystalType::POWER_UP && TierTwoCrystal != Crystal));
}

void ACGInventory::LoadCrystal(ECGCrystalType Crystal)
{
	bool bIsDirty = false;

	// Tier1 crystal
	if (Crystal > ECGCrystalType::POWER_UP && TierOneCrystal != Crystal)
	{
		TierOneCrystal = Crystal;
		bIsDirty = true;
	}
	else if (Crystal > ECGCrystalType::NONE && TierTwoCrystal != Crystal)
	{
		TierTwoCrystal = Crystal;
		bIsDirty = true;
	}

	if (bIsDirty)
	{
		ReconstructInventory();
		// TODO trigger inventory rebuild.
	}
}

void ACGInventory::PopAllCrystals()
{
	// Clear the Crystals and rebuild inventory.
	TierOneCrystal = ECGCrystalType::NONE;
	TierTwoCrystal = ECGCrystalType::NONE;

	ReconstructInventory();
}

void ACGInventory::PopBestCrystal()
{
	if (TierTwoCrystal != ECGCrystalType::NONE)
	{
		TierTwoCrystal = ECGCrystalType::NONE;
	}
	else if (TierOneCrystal != ECGCrystalType::NONE)
	{
		TierOneCrystal = ECGCrystalType::NONE;
	}

	ReconstructInventory();
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
	DOREPLIFETIME_CONDITION(ACGInventory, TierTwoCrystal, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ACGInventory, Weapons, COND_OwnerOnly);
	
	DOREPLIFETIME(ACGInventory, CGOwner);
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
