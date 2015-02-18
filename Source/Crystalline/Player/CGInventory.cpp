// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGInventory.h"

ACGInventory::ACGInventory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	TierOneCrystal = ECrystalType::NONE;
	TierTwoCrystal = ECrystalType::NONE;

	bReplicates = true;
}

void ACGInventory::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	TierOneCrystal = ECrystalType::NONE;
	TierTwoCrystal = ECrystalType::NONE;
}


void ACGInventory::InitializeInventory(FCGDefaultWeaponConfig Config)
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

	if (Config.CoreCrystalGun)
	{
		AddWeapon(GetWorld()->SpawnActor<ACGWeapon>(Config.CoreCrystalGun, SpawnInfo));
	}

	// Get the number of weaopns that were properly defined.
	StaticWeaponCount = Weapons.Num();

	const int32 NumWeaponGroups = Config.CrystalGunTiers.Num();
	for (int i = 0; i < NumWeaponGroups; ++i)
	{
		// If it exists spawn the weapon.
		if (Config.CrystalGunTiers[i].TierOneGun)
		{
			// Add the Tier OneGun
			AddWeapon(GetWorld()->SpawnActor<ACGWeapon>(Config.CrystalGunTiers[i].TierOneGun, SpawnInfo), Config.CrystalGunTiers[i].TierOneCrystalType);
		}

		if (Config.CrystalGunTiers[i].TierTwoGun)
		{
			// Add the TierTwo Gun
			AddWeapon(GetWorld()->SpawnActor<ACGWeapon>(Config.CrystalGunTiers[i].TierTwoGun, SpawnInfo), Config.CrystalGunTiers[i].TierOneCrystalType);
		}
	}

	// Constructs the inventory for the player.
	ReconstructInventory();
}

void ACGInventory::AddWeapon(ACGWeapon* Weapon, ECrystalType Type)
{
	// If the weapon wasn't set and we're not the server, don't bother.
	if (Weapon == NULL || Role < ROLE_Authority)
	{
		return;
	}

	// If no type is specified, assume that it's a default weapon.
	if (Type == ECrystalType::NONE)
	{
		Weapons.AddUnique(Weapon);
	}
	else
	{
		// FIXME breaks at runtime!
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
	// Destroy inventory weapons.
	for (int32 i = Weapons.Num() - 1; i >= 0; --i)
	{
		CachedWeapon = Weapons[i];
		if (CachedWeapon)
		{
			Weapons.RemoveSingle(CachedWeapon);
			CachedWeapon->OnExitInventory();
			CachedWeapon->Destroy();
		}
	}

	// Clear the weapon groups.
	TArray<ECrystalType> Keys;
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
				Values.RemoveSingle(CachedWeapon);
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

	// Clear the elements that need to be cleared.
	Weapons.RemoveAt(StaticWeaponCount, Weapons.Num() - StaticWeaponCount, true);

	if (TierOneCrystal != ECrystalType::NONE && WeaponGroups.Contains(TierOneCrystal))
	{
		// Determine the number of weapons to be transfered over.
		int32 CopyLength = FMath::Min(WeaponGroups[TierOneCrystal].Num(),TierTwoCrystal == ECrystalType::NONE ? 1 : 2);

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

bool ACGInventory::CanLoadCrystal(ECrystalType Crystal)
{
	return Crystal != ECrystalType::NONE &&
		((Crystal > ECrystalType::POWER_UP && TierOneCrystal != Crystal) ||
		(Crystal <= ECrystalType::POWER_UP && TierTwoCrystal != Crystal));
}

void ACGInventory::LoadCrystal(ECrystalType Crystal)
{
	bool bIsDirty = false;
	// Tier1 crystal
	if (Crystal > ECrystalType::POWER_UP && TierOneCrystal != Crystal)
	{
		TierOneCrystal = Crystal;
		bIsDirty = true;
	}
	else if (Crystal > ECrystalType::NONE && TierTwoCrystal != Crystal)
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

void ACGInventory::OnRep_CGOwner()
{
	// TODO Replicate the inventory being loaded.
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
