// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Misc/CGTypes.h"

#include "CGWeapon.generated.h"

#define WEAPON_TRACE_TAG FName("WeaponTrace")

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
	virtual void Tick(float DeltaSeconds) override;


#pragma region Visuals
protected:
	/** weapon mesh: 1st person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

#pragma endregion 


public:
	////////////////////////////
	// Gettors
	float GetCurrentSpread();


	////////////////////////////
	// Config

	/** Generic weapon configuration settings.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FCGWeaponConfig WeaponConfig;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	FCGProjectileData ProjectileConfig;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	FCGSpreadData SpreadConfig;

	/** Generic weapon effect configuration settings.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FCGWeaponFXData WeaponFXConfig;

	/** Generic weapon HUD configuration settings.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FCGWeaponHUDData WeaponHUDConfig;
	
	/**Defines the zoom factor for the weapon.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FCGZoom WeaponZoomConfig;
	

	////////////////////////////
	// Mutable weapon fields

	// Keeps track of the current burst. Ammo Doesn't need to be replicated.
	UPROPERTY(Transient, ReplicatedUsing=OnRep_BurstCount)
	int32 BurstCount;

	/** The time of the last shot by the weapon.*/
	UPROPERTY()
	float LastFireTime;

	UPROPERTY()
	float CurrentSpread;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_HitNotify)
	FCGInstantHit HitNotify;

	/**The percentage of the clip fired by a single shot.*/
	UPROPERTY()
	float ClipPercentPerShot;

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
#pragma region State Functions

	/** Invoked when a weapon enters the inventory of a player or bot. */
	void OnEnterInventory(class ACGCharacter* CGOwner);

	/** Invoked when a weapon exits the inventory of a player or bot. */
	void OnExitInventory();

	/** Invoked when the weapon is equipped by a player or bot. */
	virtual void OnEquip();

	/** Invoked when the weapon is unequipped by a player or bot. */
	virtual void OnUnequip();

	virtual void OnStartReload();
	
	UFUNCTION(server, reliable, WithValidation)
	void ServerStartReload();
	
	void StopReload();

	// TODO find a better way?
	/**Invoked by the server when a timer based call finishes execution.*/
	UFUNCTION(client, reliable)
	void ClientCheckQueuedInput();


	// This is managed by the weapon, not the state at the present.
	// FIXME
	virtual void StartOverheat();

	// Gives the Reload state the information it requires to spawn a timer.
	virtual float GetReloadTime() const;

	virtual bool HasManualReload() const { return WeaponConfig.bHasManualReload; }

	virtual bool CanReload() const;
		
	virtual void ApplyReload();



#pragma endregion

#pragma region Fire Functions

	/** Starts the firing of a weapon if possible. */
	virtual void StartFire();

	UFUNCTION(server, reliable, WithValidation)
	void ServerStartFire();

	/** Ends the firing of the weapon, stop in StartFire for non automatic weapons. */
	virtual void StopFire();

	UFUNCTION(server, reliable, WithValidation)
	void ServerStopFire();
	
	virtual bool StartFiring();

	virtual void StopFiring();

	virtual void StartWeaponFireSimulation();

	virtual void StopWeaponFireSimulation();

#pragma endregion

#pragma region Projectile

	void FireProjectile();

	UFUNCTION(server, reliable, WithValidation)
	void ServerFireProjectile(FVector Origin, FVector_NetQuantizeNormal ShootDir);

	virtual void SpawnProjectile(FVector Origin, FVector_NetQuantizeNormal ShootDir);

#pragma endregion

#pragma region Hit Scan

	virtual void FireHitScan();

	void ProcessHitScan(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandSeed, float Spread);
	
	virtual void ProcessHitScanConfirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandSeed, float Spread);

	/** server notified of hit from client to verify */
	UFUNCTION(reliable, server, WithValidation)
	void ServerNotifyHit(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float Spread);

	/** server notified of miss to show trail FX */
	UFUNCTION(unreliable, server, WithValidation)
	void ServerNotifyMiss(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float Spread);

	UFUNCTION()
	void OnRep_HitNotify();

	void SimulateHitScan(const FVector& Origin, const FVector& ShotDir);

	virtual void SpawnTrailEffect(const FVector& EndPoint);

	// TODO
	void SpawnHitEffect(const FHitResult& Impact);

	bool ShouldDealDamage_Instant(AActor* TestActor) const;

	void DealDamage_Instant(const FHitResult& Impact, const FVector& ShootDir);

#pragma endregion

#pragma region Ammo

	virtual void GiveAmmo(int32 Ammo);

	virtual void UseAmmo();

	// If set to false it's a check by something like CGCharacter.
	virtual bool CanFire(bool InitFireCheck = false) const;

	virtual float GetClipPercent() const;

	float GetPercentPerShot() const { return ClipPercentPerShot; }

	virtual int32 GetAmmo() const { return 0; }

	virtual int32 GetAmmoInClip() const { return 0; }


#pragma endregion

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
