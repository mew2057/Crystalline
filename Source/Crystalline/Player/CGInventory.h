// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CGInventory.generated.h"

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

	void AddWeapon(ACGWeapon* Weapon, ECGCrystalType Type = ECGCrystalType::NONE);

	virtual void DestroyInventory();

	void ReconstructInventory();

	bool CanLoadCrystal(ECGCrystalType Crystal);

	void LoadCrystal(ECGCrystalType Crystal);

	UFUNCTION(BlueprintCallable, Category = "Game|Inventory")
	void PopAllCrystals();

	FORCEINLINE ACGWeapon* GetWeapon(int32 Index) const { return Weapons[Index]; }

	FORCEINLINE int32 GetWeaponCount() const { return Weapons.Num(); }

	FORCEINLINE int32 GetWeaponIndex(ACGWeapon* Weapon) const { return Weapons.Find(Weapon); }

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

	/** The Tier 2 Crystal for the Player. (POWER UP) */
	UPROPERTY(Transient, Replicated)
	ECGCrystalType TierTwoCrystal;
	
	/**The Weapons the player has available to them.*/
	UPROPERTY(Transient, Replicated)
	TArray<class ACGWeapon*> Weapons;

	/**This is the count of weapons that don't change with crystals.*/
	UPROPERTY(Transient)
	int32 StaticWeaponCount;

	/** Contains the weapon groups that the gun inventory is selected by. Managed by the server, then replicated to the client through Weapons.*/
	TMap<ECGCrystalType, TArray<class ACGWeapon*>> WeaponGroups;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ACGAmmoPickup> AmmoPickupTemplate;
};
