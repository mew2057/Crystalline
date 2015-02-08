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
	GENERATED_BODY()

	// Give the states access to the weapon.
	friend class UCGWeaponState;
	friend class UCGWeaponActiveState;
	friend class UCGWeaponInactiveState;
	friend class UCGWeaponEquippingState;
	friend class UCGWeaponUnequippingState;
	friend class UCGWeaponFiringState;
	friend class UCGWeaponReloadingState;

public:
	ACGWeapon(const FObjectInitializer& ObjectInitializer);
	
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

#pragma region Visuals
protected:
	/** weapon mesh: 1st person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; };


	/** The ACGCharacter holding this weapon. Replication triggers the inventory update functions. */
	ACGCharacter* CGOwner;

public:

	UFUNCTION()
	void OnRep_CGOwner();

	/** Invoked when a weapon enters the inventory of a player or bot. */
	void OnEnterInventory(class ACGCharacter* CGOwner);

	/** Invoked when a weapon exits the inventory of a player or bot. */
	void OnExitInventory();

	/** Invoked when the weapon is equipped by a player or bot. */
	virtual void OnEquip();

	/** Invoked when the weapon is unequipped by a player or bot. */
	virtual void OnUnequip();


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
	ACGCharacter* GetCGOwner() const;


#pragma region Visuals

protected:
	/** Adds the mesh to the owner's mesh. */
	void AttachMeshToPawn();

	/** Removes the mesh from the owner's mesh. */
	void DetachMeshFromPawn();

#pragma endregion

#pragma region State Management
public :
	/**
	* Attempts to transition current state to the supplied state.
	*/
	void GotoState(UCGWeaponState* NewState);

	/**Switches the weapon to the equipping state.*/
	void GotoEquippingState();

	/**Switches the weapon to the firing state.*/
	void GotoFiringState();

	/**Switches the weapon to the unequipping state.*/
	void GotoUnequippingState();

protected:
	UPROPERTY(BlueprintReadOnly, Category = States)
	UCGWeaponState* CurrentState;

	UPROPERTY(Instanced, EditAnywhere, BlueprintReadWrite, Category = "States")
	UCGWeaponState* ActiveState;

	UPROPERTY(Instanced, EditAnywhere, BlueprintReadWrite, Category = States)
	UCGWeaponState* InactiveState;

	UPROPERTY(Instanced, EditAnywhere, BlueprintReadWrite, Category = States)
	UCGWeaponState* EquippingState;

	UPROPERTY(Instanced, EditAnywhere, BlueprintReadWrite, Category = States)
	UCGWeaponState* UnequippingState;

	UPROPERTY(Instanced, EditAnywhere, BlueprintReadWrite, Category = States)
	UCGWeaponState* ReloadingState;
	
	UPROPERTY(Instanced, EditAnywhere, BlueprintReadWrite, Category = States)
	UCGWeaponState* FiringState;

#pragma endregion

};
