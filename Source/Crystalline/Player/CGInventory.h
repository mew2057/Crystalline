// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CGInventory.generated.h"

#define INVENTORY_NUM_WEAPONS 2

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGInventory : public AActor
{
	GENERATED_BODY()

public:
	ACGInventory(const FObjectInitializer& ObjectInitializer);

	void PostInitializeComponents();

	virtual void InitializeInventory(const FCGDefaultWeaponConfig& Config);

	/**Adds a weapon to the active weapon inventory.*/
	void AddToWeapons(ACGWeapon* Weapon);

	/**Adds a weapon to the mapping of weapons in our inventory.*/
	void AddToWeaponMap(ACGWeapon* Weapon, ECGCrystalType Type = ECGCrystalType::NONE);

	virtual void DestroyInventory();

	void ReconstructInventory();

	bool CanLoadCrystal(ECGCrystalType Crystal);

	void LoadCrystal(ECGCrystalType Crystal);

	UFUNCTION(BlueprintCallable, Category = "Game|Inventory")
	void PopAllCrystals();

	/**Pops the Crystals in reverse order, e.g. T2 -> T1*/
	void PopBestCrystal();

	FORCEINLINE ACGWeapon* GetWeapon(int32 Index) const { return Weapons[Index]; }

	FORCEINLINE int32 GetWeaponCount() const { return Weapons.Num(); }

	FORCEINLINE int32 GetWeaponIndex(ACGWeapon* Weapon) const { return Weapons.Find(Weapon); }

	// XXX There might be a better way to do this.
	FORCEINLINE ACGWeapon* GetNextWeapon(ACGWeapon* Weapon) const { return Weapons[(Weapons.Find(Weapon) + 1) % Weapons.Num()]; }


	void SetCGOwner(ACGCharacter* NewOwner);

	UFUNCTION()
	void OnRep_CGOwner();

	bool GiveAmmo(ECGAmmoType AmmoType, int32 Ammo);

	/** The ACGCharacter who owns this Inventory. */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CGOwner)
	ACGCharacter* CGOwner;
	
	/** The Tier 1 Crystal for the Player. (FORCE, ACCURACY, UTILITY)*/
	UPROPERTY(Transient, Replicated)
	ECGCrystalType TierOneCrystal;

	/**The Weapons the player has available to them.*/
	UPROPERTY(Transient, Replicated)
	TArray<class ACGWeapon*> Weapons;

	/**The index of the crystal gun.*/
	UPROPERTY(Transient)
	int32 CrystalGunIndex;

	/** Contains the weapon groups that the gun inventory is selected by. Managed by the server, then replicated to the client through Weapons.*/
	TMap<ECGCrystalType, TArray<class ACGWeapon*>> WeaponGroups;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ACGAmmoPickup> AmmoPickupTemplate;
};
