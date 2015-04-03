// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CGInventory.generated.h"


/** Defines the ammo cache stucts. */
USTRUCT()
struct FCGAmmo
{
	GENERATED_USTRUCT_BODY()

	/** Ammo loaded into the gun. */
	int32 AmmoInClip;

	/** Ammo in the player's inventory for the gun.*/
	int32 Ammo;

	FCGAmmo()
	{
		Ammo = -1;
		AmmoInClip = -1;
	}
};


/** XXX I feel as though the inventory has far too much repeated data, but it works.
 *  Defines the inventory system for crystalline. By design the server is authoritative on what guns are available, and caching the ammo of the player for each ammo type.
 */
UCLASS()
class CRYSTALLINE_API ACGInventory : public AActor
{
	GENERATED_BODY()

public:
	ACGInventory(const FObjectInitializer& ObjectInitializer);

	/** Nulls out the crystals held in the inventory. */
	void PostInitializeComponents();

	/**
	 * Initializes the inventory: builds the AmmoCache and WeaponGroups Maps, Constructs the Weapon Actors, and Reconstructs the Inventory.
	 *
	 * @param Config The weapon configuration structure that defines the weapon "graph".
	 */
	void InitializeInventory(const FCGDefaultWeaponConfig& Config);

	/**
	 * Adds a weapon to the active weapon inventory (array).
	 *
	 * @param Weapon The weapon to be added to the Weapon Array.
	 */
	void AddToWeapons(ACGWeapon* Weapon);

	/**
	 * Adds a weapon to the mapping of weapons in our inventory.
	 *
	 * @param Weapon The Weapon to add to the WeaponGroups Map.
	 * @param Type The Type of the weapon being added.
	 */
	void AddToWeaponMap(ACGWeapon* Weapon, ECGCrystalType Type = ECGCrystalType::NONE);

	/**
	 * Destroys the Map and despawns all of the weapons. Spawns any AmmoPickups as is required.
	 */
	void DestroyInventory();

	/**
	 * Handles Crystal Popping/Loading, Copies ammo between Crystal Guns as appropriate.
	 */
	void ReconstructInventory();

	/**
	 * Determines if the weapon may recieve a crystal of th especified type.
	 *
	 * @param Crystal The Type of the crystal (e.g. Power)
	 * @return True if the Crystal may be loaded into the gun.
	 */
	bool CanLoadCrystal(ECGCrystalType Crystal);

	/**
	 * Adds A Crystal to the inventory. Reconstructs the Inventory.
	 *
	 * @param Crystal The Crystal being slotted.
	 */
	void LoadCrystal(ECGCrystalType Crystal);

	/**
	 * Pops any and all crystals in the inventory then reconstructs it.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game|Inventory")
	void PopAllCrystals();

	/**
	 * Pops the Crystals in reverse order, e.g. T2 -> T1
	 */
	void PopBestCrystal();

	/**
	 * Assigns the owner of the Inventory.
	 *
	 * @param NewOwner The owner of this invetory.
	 */
	void SetCGOwner(ACGCharacter* NewOwner);

	/** XXX This may not be necessary.
	 * Ensures that the owner is properly set on the clients.
	 */
	UFUNCTION()
	void OnRep_CGOwner();

	/**
	 * Gives the inventory an allotment of ammo of the specified type.
	 *
	 * @param AmmoType The Type of ammo to give to the inventory.
	 * @param Ammo The ammount of ammo to give to the inventory.
	 * @return True if the ammo has successfully been acquired.
	 */
	bool GiveAmmo(ECGAmmoType AmmoType, int32 Ammo);

	/** XXX Should this do index checking?
	* Getter for a weapon at the supplied index.
	*
	* @param Index The index of the requested weapon.
	*/
	FORCEINLINE ACGWeapon* GetWeapon(int32 Index) const { return Weapons[Index]; }

	/**
	* Gets the count of the weapons in the available Inventory.
	*
	* @return The Number of weapons usable in the inventory.
	*/
	FORCEINLINE int32 GetWeaponCount() const { return Weapons.Num(); }

	/**
	* Finds the index of the weapon specfied in the parameter list.
	*
	* @param Weapon The weapon to find the index of.
	* @return The index of the weapon specified in the parameter list, -1 if not found.
	*/
	FORCEINLINE int32 GetWeaponIndex(ACGWeapon* Weapon) const { return Weapons.Find(Weapon); }

	// XXX There might be a better way to do this.
	/**
	* Determines the next available weapon, based upon the supplied weapon.
	*
	* @param Weapon The weapon The next weapon is being determined from.
	* @return The next weapon based upon the supplied parameter.
	*/
	FORCEINLINE ACGWeapon* GetNextWeapon(ACGWeapon* Weapon) const { return Weapons[(Weapons.Find(Weapon) + 1) % Weapons.Num()]; }

	/**
	 * Retrieves the TierOneCrsytal from the Inventory.
	 * 
	 * @return The Tier One Crystal slotted.
	 */
	FORCEINLINE ECGCrystalType GetTierOneCrystal(){ return TierOneCrystal; }

protected:
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

	/** Contains the weapon groups that the gun inventory is selected by. Managed by the server, then replicated to the client through Weapons. */
	TMap<ECGCrystalType, TArray<class ACGWeapon*>> WeaponGroups;

	/** Contains the cached ammo for the weapons a player can hold. This ensures that the ammo is preserved across multiple guns. Ammo is copied between guns. */
	TMap<ECGAmmoType, FCGAmmo> AmmoCache;

	// XXX Should this live here?
	/** The template for ammo pickups, defined in editor. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ACGAmmoPickup> AmmoPickupTemplate;
};
