// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CGWeapon.generated.h"


USTRUCT()
struct FCGWeaponConfig
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Timing)
	float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, Category = Timing)
	uint32 bAutomaticFire : 1;

	UPROPERTY(EditDefaultsOnly, Category = Timing)
	float EquipTime;

	UPROPERTY(EditDefaultsOnly, Category = Timing)
	float UnequipTime;

	UPROPERTY(EditDefaultsOnly, Category = WeaponAttributes)
	uint32 bUsesProjectile : 1;
	
	UPROPERTY(EditDefaultsOnly, Category = WeaponAttributes)
	TSubclassOf<UDamageType> DamageType;
	
	/** The damage caused by a single shot. */
	UPROPERTY(EditDefaultsOnly, Category=WeaponAttributes)
	float BaseDamage;

	/** The range at which the gun can be reliably shot to. In hitscan guns this is the maximu range of the gun.*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponAttributes)
	float WeaponRange;

	FCGWeaponConfig()
	{
		// Timing.
		TimeBetweenShots = .01;
		bAutomaticFire = false;
		EquipTime   = .05f;
		UnequipTime = .05f;
	
		// Attributes.
		bUsesProjectile = true;
		BaseDamage  = 10.f;
		WeaponRange = 10000.f;
	}
};

USTRUCT()
struct FCGProjectileData
{
	GENERATED_USTRUCT_BODY()

	/** The class of the Projectile that will be shot by the weapon. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ACGProjectile> ProjectileClass;

	/** The life span of the projectile before despawning. */
	UPROPERTY(EditDefaultsOnly)
	float ProjectileLife;

	/** Sets defaults */
	FCGProjectileData()
	{
		ProjectileClass = NULL;
		ProjectileLife = 2.0f;
	}
};

USTRUCT()
struct FCGHitScanData
{
	GENERATED_USTRUCT_BODY()	

	/** The angle (in Degrees) of the base spread for this gun.*/
	UPROPERTY(EditDefaultsOnly)
	float BaseSpread;

	/** The angle (in Degrees) of the max spread for this gun.*/
	UPROPERTY(EditDefaultsOnly)
	float MaxSpread;

	/** The angle (in Degrees) that the gun spreads each continuous shot.*/
	UPROPERTY(EditDefaultsOnly)
	float SpreadPerShot;

	FCGHitScanData()
	{
		MaxSpread     = 0.f;
		BaseSpread    = 0.f;
		SpreadPerShot = 0.f;
	}
};

USTRUCT()
struct FCGInstantHit
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector Origin;

	UPROPERTY()
	float Spread;

	UPROPERTY()
	uint32 RandSeed;
};

/** VFX and SFX related to the weapon.*/
USTRUCT()
struct FCGWeaponFXData
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

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	uint32 bDrawWeaponTrail : 1;

	/** The Weapon trail for the bullet. Typically only used by hit scan weapons. */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* WeaponTrail;

	// TODO impact.

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<UCameraShake> CameraShake;
	

	////////////////////////////
	// SFX

	// XXX Maybe this should be a sound cue to prevent non prepped sounds from being used?
	/** Played on weapon fire. */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	class USoundBase* FireSound;

	FCGWeaponFXData()
	{
		/** The default name of the Muzzle socket. */
		MuzzleSocket = TEXT("MuzzleFlashSocket");
		TrailTargetParam = TEXT("TrailEnd");
		bDrawWeaponTrail = false;

	}
};

/** HUD Textures and configuration.*/
USTRUCT()
struct FCGWeaponHUDData
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

	FCGWeaponHUDData()
	{
		// Default colors for the ammo readout.
		LowAmmoColor = FLinearColor(1, 0, 0, 1); // Red
		FullAmmoColor = FLinearColor(0, 1, 0, 1); // Green

		AmmoGuageWidth = 256.0f;
	}
};

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

#pragma endregion 


public:
	////////////////////////////
	// Config

	/** Generic weapon configuration settings.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FCGWeaponConfig WeaponConfig;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	FCGProjectileData ProjectileConfig;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	FCGHitScanData HitScanConfig;

	/** Generic weapon effect configuration settings.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FCGWeaponFXData WeaponFXConfig;

	/** Generic weapon HUD configuration settings.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FCGWeaponHUDData WeaponHUDConfig;
	

	////////////////////////////
	// Mutable weapon fields

	// Keeps track of the current burst. Ammo Doesn't need to be replicated.
	UPROPERTY(Transient, ReplicatedUsing=OnRep_BurstCount)
	int32 BurstCount;
	
	// TODO track ammo, but only replicate to owner.

	/** The time of the last shot by the weapon.*/
	UPROPERTY()
	float LastFireTime;

	UPROPERTY()
	float CurrentSpread;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_HitNotify)
	FCGInstantHit HitNotify;

	////////////////////////////
	//  Components

	/** Used to manage the flash particle system. */
	UPROPERTY(Transient)
	UParticleSystemComponent* MuzzleFlashComp;

	UPROPERTY(Transient)
	UParticleSystemComponent* TrailPSC;

	/** Used to manage audio playback. */
	UPROPERTY(Transient)
	UAudioComponent* FireAudioComponent;



	/** Returns Mesh1P subobject **/
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; };
	
	/** The ACGCharacter holding this weapon. Replication triggers the inventory update functions. */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CGOwner)
	ACGCharacter* CGOwner;

	UFUNCTION()
	void OnRep_CGOwner();


	////////////////////////////
	// State Functions 

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

	UFUNCTION(server, reliable, WithValidation)
	void ServerStartFire();

	/** Ends the firing of the weapon, stop in StartFire for non automatic weapons. */
	virtual void StopFire();

	UFUNCTION(server, reliable, WithValidation)
	void ServerStopFire();
	
	virtual void StartFiring();

	virtual void StopFiring();

	virtual void StartWeaponFireSimulation();

	virtual void StopWeaponFireSimulation();

#pragma region Projectile

	void FireProjectile();

	UFUNCTION(server, reliable, WithValidation)
	void ServerFireProjectile(FVector Origin, FVector_NetQuantizeNormal ShootDir);

#pragma endregion

#pragma region Hit Scan

	void FireHitScan();

	void ProcessHitScan(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandSeed, float Spread);
	
	void ProcessHitScanConfirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandSeed, float Spread);

	/** server notified of hit from client to verify */
	UFUNCTION(reliable, server, WithValidation)
	void ServerNotifyHit(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float Spread);

	/** server notified of miss to show trail FX */
	UFUNCTION(unreliable, server, WithValidation)
	void ServerNotifyMiss(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float Spread);

	UFUNCTION()
	void OnRep_HitNotify();

	void SimulateHitScan(const FVector& Origin, int32 RandomSeed, float Spread);

	void SpawnTrailEffect(const FVector& EndPoint);

	// TODO
	void SpawnHitEffect(const FHitResult& Impact);

	bool ShouldDealDamage_Instant(AActor* TestActor) const;

	void DealDamage_Instant(const FHitResult& Impact, const FVector& ShootDir);

#pragma endregion

	virtual void UseAmmo();

	/**Replicates weapon fire simulation. */
	UFUNCTION()
	void OnRep_BurstCount();

	/**
	 * Sets the owner of the weapon. 
	 * @param The new owner of this weapon.	
	 */
	void SetCGOwner(ACGCharacter* NewOwner);

	/** Retrieves the owner of the weapon. */
	FORCEINLINE ACGCharacter* GetCGOwner() const {	return CGOwner;	}

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

protected:
	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;


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

	/**Switches the weapon to the firing state, Checks Ammo, Overheat, etc. May be overridem for this reason.*/
	virtual void GotoFiringState();

public:

	UPROPERTY(BlueprintReadOnly)
	UCGWeaponState* CurrentState;

protected:

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
