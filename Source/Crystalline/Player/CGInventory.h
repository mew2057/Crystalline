// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CGInventory.generated.h"

#define TIER_ZERO_AMMO 0
#define TIER_ONE_AMMO 1
#define TIER_TWO_AMMO 2

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

	void InitializeInventory(const FCGDefaultWeaponConfig& Config);

	void AddWeapon(ACGWeapon* Weapon, ECrystalType Type = ECrystalType::NONE);

	void DestroyInventory();

	void ReconstructInventory();

	bool CanLoadCrystal(ECrystalType Crystal);

	void LoadCrystal(ECrystalType Crystal);

	FORCEINLINE ACGWeapon* GetWeapon(int32 Index) const { return Weapons[Index]; }

	FORCEINLINE int32 GetWeaponCount() const { return Weapons.Num(); }

	FORCEINLINE int32 GetWeaponIndex(ACGWeapon* Weapon) const { return Weapons.Find(Weapon); }

	void SetCGOwner(ACGCharacter* NewOwner);

	UFUNCTION()
	void OnRep_CGOwner();

	/** The ACGCharacter who owns this Inventory. */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CGOwner)
	ACGCharacter* CGOwner;
	
	/** The Tier 1 Crystal for the Player. (FORCE, ACCURACY, UTILITY)*/
	UPROPERTY(Transient, Replicated)
	ECrystalType TierOneCrystal;

	/** The Tier 2 Crystal for the Player. (POWER UP) */
	UPROPERTY(Transient, Replicated)
	ECrystalType TierTwoCrystal;
	
	/**The Weapons the player has available to them.*/
	UPROPERTY(Transient, Replicated)
	TArray<class ACGWeapon*> Weapons;

	/**This is the count of weapons that don't change with crystals.*/
	UPROPERTY(Transient)
	int32 StaticWeaponCount;

	/** Contains the weapon groups that the gun inventory is selected by. Managed by the server, then replicated to the client through Weapons.*/
	TMap<ECrystalType, TArray<class ACGWeapon*>> WeaponGroups;
};
