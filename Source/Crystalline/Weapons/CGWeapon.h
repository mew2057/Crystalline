// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CGWeapon.generated.h"

/**
 * 
 */
UCLASS( Abstract,  Config=Game)
class CRYSTALLINE_API ACGWeapon : public AActor
{
	GENERATED_UCLASS_BODY()

	// Give the states access to the weapon.
	friend class UCGWeaponState;
	friend class UCGWeaponActiveState;
	friend class UCGWeaponInactiveState;
	friend class UCGWeaponEquippingState;
	friend class UCGWeaponUnequippingState;
	friend class UCGWeaponFiringState;
	friend class UCGWeaponReloadingState;

	
//	ACGWeapon(const FObjectInitializer& PCIP);
	
	virtual void PostInitializeComponents() override;

	/***/
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CGOwner)
	ACGCharacter* CGOwner;

	UFUNCTION()
	void OnRep_CGOwner();


	/** Invoked when a weapon enters the inventory of a player or bot. */
	void OnEnterInventory(class ACGCharacter* CGOwner);

	/** Invoked when a weapon exits the inventory of a player or bot. */
	void OnExitInventory();

	/** Invoked when the weapon is equipped by a player or bot. */
	virtual void OnEquip();

	/** Invoked when the weapon is unequipped by a player or bot. */
	virtual void OnUnEquip();


	/** Starts the firing of a weapon if possible. */
	virtual void StartFire();

	/** Ends the firing of the weapon, stop in StartFire for non automatic weapons. */
	virtual void StopFire();

	/**
	 * Sets the owner of the weapon. 
	 * @param The new owner of this weapon.	
	 */
	void SetCGOwner(ACGCharacter* NewOwner);

	/** Retrieves the owner of the weapon. */
	FORCEINLINE ACGCharacter* GetCGOwner() const { return CGOwner; };

	void GotoState(UCGWeaponState* NewState);


protected:
	UPROPERTY(BlueprintReadOnly, Category = "States")
	 UCGWeaponState* CurrentState;

	UPROPERTY(Instanced, BlueprintReadOnly, Category = States)
	TSubobjectPtr<class UCGWeaponActiveState> ActiveState;

	UPROPERTY(Instanced, BlueprintReadOnly, Category = States)
		TSubobjectPtr<class UCGWeaponInactiveState > InactiveState;

	UPROPERTY(Instanced, BlueprintReadOnly, Category = States)
		TSubobjectPtr<class UCGWeaponEquippingState> EquippingState;

	UPROPERTY(Instanced, BlueprintReadOnly, Category = States)
		TSubobjectPtr<class UCGWeaponUnequippingState> UnequippingState;

	UPROPERTY(Instanced, BlueprintReadOnly, Category = States)
		TSubobjectPtr<class UCGWeaponReloadingState> ReloadingState;
	
	UPROPERTY(Instanced, BlueprintReadOnly, Category = States)
		TSubobjectPtr<class UCGWeaponFiringState> FiringState;

	//UCGWeaponState;
};
