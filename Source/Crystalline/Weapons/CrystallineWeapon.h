// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CrystallineWeapon.generated.h"

UENUM()
namespace CrystallineWeaponState
{
	enum Type
	{
		Idle,
		Firing,
		Reloading,
		Equipping,
		Unusable, // This is a general case, if unusable the weapon may not be fired.
	};
}

// Defines the ammo type used in the weapons
UENUM() 
namespace CrystallineAmmo
{
	enum Type
	{
		Cooldown, // The gun doesn't use traditional ammo, but overheats over time.
		Crystal    // The Crystal gun's ammo.
	};
}

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class CRYSTALLINE_API ACrystallineWeapon : public AActor
{
	GENERATED_UCLASS_BODY()	

#pragma region Overriden_Functions

	virtual void PostInitializeComponents() override;

#pragma endregion

#pragma region MeshData

private:
	/** weapon mesh: 1st person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

protected:
	/** Returns Mesh1P subobject **/
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }


#pragma endregion

#pragma region Fields

protected:
	//////////////////////////
	// Timing
	
	UPROPERTY(EditDefaultsOnly, Category = Timing)
	float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, Category = Timing)
	float ReloadTime;
	
	////////////////////////////
	//// Ammo Details

	/** The type of ammo used in the gun. */
	UPROPERTY(EditDefaultsOnly, Category = Ammunition)
	TEnumAsByte <CrystallineAmmo::Type> AmmoType; // NOTE: TEnumAsByte is needed for blueprint exposed enumerated types.
	
	/** The number of rounds available to the gun. */
	int32 Rounds;

	/** The number of rounds in a single ammo clip */
	int32 RoundsPerClip;


	////////////////////////////
	// Visual Effects

	/** The socket on the gun model that corresponds to the muzzle of the gun. */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	FName MuzzleSocket; 

	/** The Muzzle flash for the gun. */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* MuzzleFlash;
	
	// NOTE: Impact needs to be defined in the child classes.


#pragma endregion



#pragma region Functions

public:
	/** Starts the firing of a weapon if possible. */
	virtual void StartFire();

	/** Ends the firing of the weapon, stop in StartFire for non automatic weapons. */
	virtual void StopFire();

protected:
	
	/** Fires the weapon, Implementation is weapon specific. */
	virtual void FireWeapon() PURE_VIRTUAL(ACrystallineWeapon::FireWeapon,);

#pragma endregion



};
