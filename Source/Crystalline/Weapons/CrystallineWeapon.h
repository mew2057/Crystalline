// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Engine/Canvas.h"			/** Needed to include the canvas to get FCanvasIcon accessible in the weapon configuration. */
#include "CrystallineWeapon.generated.h"

/** A special tag for generic weapon traces. */
#define WEAPON_TRACE_TAG FName("WeaponTrace")

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

/** Defines weapon firing behavior.*/
USTRUCT()
struct FWeaponConfigData
{
	GENERATED_USTRUCT_BODY()

	/** If true the HandleFire Routine will call itself. */
	UPROPERTY(EditDefaultsOnly, Category = WeaponConfiguration)
	uint32 bAutomaticFire : 1;

	/** The range the weapon is usable at.*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponConfiguration)
	float WeaponRange;

	/** The ammo type of the gun. */
	UPROPERTY(EditDefaultsOnly, Category = WeaponConfiguration)
	TEnumAsByte <CrystallineAmmo::Type> AmmoType;

	/** type of damage */
	UPROPERTY(EditDefaultsOnly, Category = WeaponConfiguration)
	TSubclassOf<UDamageType> DamageType;


	/** The maximum Horizontal spread of the angle for the weapon in degrees.*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponConfiguration)
	float HSpreadMax;

	/** The base Horizontal spread of the angle for the weapon in degrees.*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponConfiguration)
	float HSpreadBase;
	
	/** The increment for the horizontal spread each fire.*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponConfiguration)
	float HSpreadIncrement;

	/** The maximum Vertical spread of the angle for the weapon in degrees.*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponConfiguration)
	float VSpreadMax;

	/** The base Vertical spread of the angle for the weapon in degrees.*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponConfiguration)
	float VSpreadBase;

	/** The increment for the vertical spread each fire.*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponConfiguration)
	float VSpreadIncrement;


	UPROPERTY(EditDefaultsOnly, Category = Timing)
	float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, Category = Timing)
	float ReloadTime;
	


	FWeaponConfigData()
	{
		////////////////////////////////
		// Fire settings.
		bAutomaticFire   = false;		
		WeaponRange      = 1000.f;
		AmmoType         = CrystallineAmmo::Cooldown;
		
		////////////////////////////////
		// Spread		
		HSpreadMax       = 0.f;
		HSpreadBase      = 0.f;
		HSpreadIncrement = 0.f;
		
		VSpreadMax		 = 0.f;
		VSpreadBase		 = 0.f;
		VSpreadIncrement = 0.f;

		////////////////////////////////
		// Time
		TimeBetweenShots = 0.f;
		ReloadTime       = 0.f;
	}
};

/** VFX and SFX related to the weapon.*/
USTRUCT()
struct FWeaponFXData
{
	GENERATED_USTRUCT_BODY()

	////////////////////////////
	// VFX

	/** The socket on the gun model that corresponds to the muzzle of the gun. */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	FName MuzzleSocket;

	/** The Muzzle flash for the gun. */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* MuzzleFlash;

	/** The name of the end of the trail in the material editor, should be in the target property.*/
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	FName TrailTargetParam;

	/** The Weapon trail for the bullet. Typically only used by hit scan weapons. */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* WeaponTrail;

	////////////////////////////
	// SFX

	/** Played on weapon fire. */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	class USoundBase* FireSound;

	FWeaponFXData()
	{
		/** The default name of the Muzzle socket. */
		MuzzleSocket     = TEXT("MuzzleFlashSocket");
		TrailTargetParam = TEXT("TrailEnd");
	}
};


/** HUD Textures and configuration.*/
USTRUCT()
struct FWeaponHUDData
{
	GENERATED_USTRUCT_BODY()

	/**The crosshair icon for the weapon. Base color should be white. */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	FCanvasIcon CrosshairIcon;

	/**The Ammo Guage Background for the weapon. Base color should be white. */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	FCanvasIcon AmmoGuageBGIcon;

	/**The Ammo Guage Foreground for the weapon. Base color should be white. */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	FCanvasIcon AmmoGuageFGIcon;
	
	/**The Weapon Icon for the weapon. Base color should be white. */
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

	FWeaponHUDData()
	{
		// Default colors for the ammo readout.
		LowAmmoColor   = FLinearColor(1, 0, 0, 1); // Red
		FullAmmoColor  = FLinearColor(0, 1, 0, 1); // Green

		AmmoGuageWidth = 256.0f;
	}
};


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

	/** The pawn holding the weapon. */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_OwningPawn)
	class ACrystallinePlayer* OwningPawn; // Class is needed (forward declaration).

	////////////////////////////
	// Config

	/** Generic weapon configuration settings.*/
	UPROPERTY(EditDefaultsOnly, Category =Config)
	FWeaponConfigData WeaponConfig;

	/** Generic weapon effect configuration settings.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FWeaponFXData WeaponFXConfig;

	/** Generic weapon HUD configuration settings.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FWeaponHUDData WeaponHUDConfig;

	////////////////////////////
	// Mutable weapon fields

	/** Half-angle (radians) of the horizontal spread cone.*/
	float HSpreadCurrent;

	/** Half-angle (radians) of the vertical spread cone.*/
	float VSpreadCurrent;

	/** The time when the gun last fired. */
	float LastFireTime;	

	////////////////////////////
	//  Components
	
	/** Used to manage the flash particle system. */
	UPROPERTY(Transient)
	UParticleSystemComponent* MuzzleFlashComp;

	/** Used to manage audio playback. */
	UPROPERTY(Transient)
	UAudioComponent* FireAudioComponent;

	////////////////////////////
	// State Variables

	/** Keeps track of player attempting to shoot.*/
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

	virtual void StopWeaponFireSimulation();

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
	// HUD Helpers

	/** Returns a number 0..1 representing how much of the clip has been used. (0 is none, 1 is everything).*/
	virtual float GetClipPercent() const;
	

	////////////////////////////
	// Weapon Transform Helpers

	/** Gets the aim vector for the player.*/
	FVector GetCameraAim() const;

	/** Gets the location of the player's camera.*/
	FVector GetCameraLocation() const;

	/** Gets the muzzle location for spawning projectiles and playing effects.*/
	FVector GetMuzzleLocation() const; 

	/** Gets the vector for the rotation of the muzzle.*/
	FVector GetMuzzleRotation() const;

	////////////////////////////



protected:
	
	/** Fires the weapon, Implementation is weapon specific. */
	virtual void FireWeapon() PURE_VIRTUAL(ACrystallineWeapon::FireWeapon,);

	/** Uses the Weapon Ammo. */
	virtual void UseAmmo() PURE_VIRTUAL(ACrystallineWeapon::UseAmmo, );

	/** Attach the mesh to the owning pawn. */
	void ACrystallineWeapon::AttachMeshToPawn();

	/** Detach the mesh from the owning pawn. */
	void ACrystallineWeapon::DetachMeshFromPawn();

	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;


#pragma endregion



};
