// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Engine/Canvas.h"			/** Needed to include the canvas to get FCanvasIcon accessible in the weapon configuration. */
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

	/** weapon mesh: 1st person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;


	/** Returns Mesh1P subobject **/
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }


#pragma endregion

#pragma region Fields


	////////////////////////////
	// HUD Elements

	UPROPERTY(EditDefaultsOnly, Category = HUD)
	FCanvasIcon CrosshairIcon;

	UPROPERTY(EditDefaultsOnly, Category = HUD)
	FCanvasIcon AmmoGuageBGIcon;
	
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	FCanvasIcon AmmoGuageFGIcon;

	UPROPERTY(EditDefaultsOnly, Category = HUD)
	FCanvasIcon WeaponIcon;
	
	/** Color for when the player has expended all of their available ammo for a clip.*/
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	FLinearColor  LowAmmoColor;

	/** Color for when the player has expended none of their available ammo for a clip.*/
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	FLinearColor  FullAmmoColor;

	/** The width of the ammo clip in pixels*/
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	float AmmoGuageWidth;

	//////////////////////////
	// Timing
	
	/** The time when the gun last fired. */
	float LastFireTime;

	UPROPERTY(EditDefaultsOnly, Category = Timing)
	float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, Category = Timing)
	float ReloadTime;
	
	////////////////////////////
	//// Ammo Details
	// XXX This might be better in a case by case basis.
	/** The type of ammo used in the gun. */
	UPROPERTY(EditDefaultsOnly, Category = Ammunition)
	TEnumAsByte <CrystallineAmmo::Type> AmmoType; // NOTE: TEnumAsByte is needed for blueprint exposed enumerated types.
	
	///** The number of rounds available to the gun. */
	//float Shots;
	//
	///** The number of rounds in a single ammo clip */
	//UPROPERTY(EditDefaultsOnly, Category = Ammunition)
	//float ShotsPerClip;
	//
	//
	//float Clips;	 
	
	////////////////////////////
	// Visual Effects

	/** The socket on the gun model that corresponds to the muzzle of the gun. */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	FName MuzzleSocket; 

	/** The Muzzle flash for the gun. */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* MuzzleFlash;

	/** Used to manage the flash particle system. */
	UPROPERTY(Transient)
	UParticleSystemComponent* MuzzleFlashComp;

	/** The Weapon trail for the bullet. */
	//UPROPERTY(EditDefaultsOnly, Category = Effects)
	//UParticleSystem* WeaponTrail;

	// NOTE: Impact needs to be defined in the child classes.

	/** The pawn holding the weapon. */
	UPROPERTY(Transient, ReplicatedUsing=OnRep_OwningPawn)
	class ACrystallinePlayer* OwningPawn; // Class is needed (forward declaration).
	
	////////////////////////////
	// Sound Effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	class USoundBase* FireSound;


	////////////////////////////
	// Firing
	UPROPERTY(Transient)
	uint32 bWantsToFire : 1;
	


#pragma endregion



#pragma region Functions

public:
	////////////////////////////
	// Server Input handling

	/** [server] */
	UFUNCTION(reliable, server, WithValidation)
	void ServerStartFire();

	/** [server] */
	UFUNCTION(reliable, server, WithValidation)
	void ServerStopFire();


	/** [server] */
	UFUNCTION(reliable, server, WithValidation)
	void ServerStartReload();

	/** [server] */
	UFUNCTION(reliable, server, WithValidation)
	void ServerStopReload();



	UFUNCTION()
	void OnRep_OwningPawn();
	////////////////////////////

	/** Starts the firing of a weapon if possible. */
	virtual void StartFire();

	virtual void StartBurst();

	virtual void StopBurst();

	virtual void SimulateWeaponFire();

	/** [server] handles the weapon fire and ammo update.*/
	UFUNCTION(reliable, server, WithValidation)
	void ServerHandleFire();

	/** [server] & [local]: Performs the actual weapon fire */
	virtual void HandleFire();

	/** Ends the firing of the weapon, stop in StartFire for non automatic weapons. */
	virtual void StopFire();

	/** Invoked when a weapon enters the inventory of a player or bot. */
	virtual void OnEnterInventory(class ACrystallinePlayer* NewOwner);

	/** Invoked when a weapon exits the inventory of a player or bot. */
	virtual void OnExitInventory();

	/** Invoked when the weapon is equipped by a player or bot. */
	virtual void OnEquip();

	/** Invoked when the weapon is unequipped by a player or bot. */
	virtual void OnUnEquip();

	/** Sets the owner of the Weapon. */
	void SetOwningPawn(ACrystallinePlayer* Owner);

	/* @return True if the weapon is able to be used, Base Class defaults to true.**/
	virtual bool CanFire();


	////////////////////////////
	// HUD Helper

	/** Returns a number 0..1 representing how much of the clip has been used. (0 is none, 1 is everything).*/
	virtual float GetClipPercent();

protected:
	
	/** Fires the weapon, Implementation is weapon specific. */
	virtual void FireWeapon() PURE_VIRTUAL(ACrystallineWeapon::FireWeapon,);

	/** Uses the Weapon Ammo. */
	virtual void UseAmmo() PURE_VIRTUAL(ACrystallineWeapon::UseAmmo, );

	/** Attach the mesh to the owning pawn. */
	void ACrystallineWeapon::AttachMeshToPawn();

	/** Detach the mesh from the owning pawn. */
	void ACrystallineWeapon::DetachMeshFromPawn();

#pragma endregion



};
