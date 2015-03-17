// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Pickups/CGCrystal.h"
#include "Misc/CGTypes.h"
#include "CGCharacter.generated.h"

// TODO move me somewhere sane.


/**
 * 
 */
UCLASS(config = Game)
class CRYSTALLINE_API ACGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACGCharacter(const FObjectInitializer& PCIP);

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetPlayerDefaults() override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	
	bool CantDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	bool Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser);

	void OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser);
	
	void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser);

	void ReplicateHit(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser, bool bKilled);

	/**Determines if an impact is a head shot.*/
	bool IsHeadShot(const FVector& ImpactLocation, const FVector& ShotDir);

	UFUNCTION()
	void OnRep_LastHit();

	void StopAllAnim();

	/**Invoked on player death.*/
	virtual void TornOff() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual void Restart() override;

	/** Make Sure the inventory is destroyed. */
	virtual void Destroyed() override;

	UFUNCTION(BlueprintCallable, Category = "Game|HUD")
	void SetPromptMessage(bool bSetPrompt, const FString& Message, int32 ButtonID);

	virtual bool IsFirstPerson();

	bool IsAlive();

	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	FRotator GetAimOffsets() const;

protected:
	/**Timer Handle for the Shield Regeneration timer.*/
	FTimerHandle TimerHandle_ShieldRegen;
	
	/** The name of the Socket/Bone on the skeleton that constitutes the player head.. */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	FName HeadBone;

	/**The radius of the head.*/
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float HeadRadius;

	/** The offset in the Z plane for the head from the head bone. XXX HACK!*/
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float HeadOffset;

	/** Max player shield amount. This is decayed before the health.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	float MaxShield;
	
	/** The rate of regeneration for the shield. */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	float ShieldRegenPerSecond;

	/** The current shield amount, if zero the player is susceptible to death.*/
	UPROPERTY(Transient, Replicated)
	float CurrentShield;

	/** The time until the player's shields begin to regenerate after being hit. */
	float TimeToRegen;

	/** Tracks when the shield is regenerating for the tick.*/
	uint32 bShieldRegenerating : 1;

	/** The maximum health for the player. This is reset on shield regeneration.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	float MaxHealth;

	/** The current health, damaged only after the shield pops.*/
	UPROPERTY(Transient, Replicated)
	float CurrentHealth;
	
	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastHit )
	FCGHitInfo LastHit;

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Begin the shield regeneration and set the player's CurrentHealth to max.*/
	void StartShieldRegen();


public:

	//FIXME move these variables back to private!
	///////////////////////////////////////////////////////////
	// Zoom
	// FIXME this may not work with the network!
	/** Tracks whether or not the weapon is currently zoomed.*/
	UPROPERTY(Transient, Replicated)
	uint32 bZoomed : 1;

	UPROPERTY(Transient)
	uint32 bZooming : 1;

	/** Defines the zoom factor for the weapon. */
	UPROPERTY(Transient)
	struct FCGZoom CurrentZoom;

	/** The FOV in the general case for the player.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	float FOVDefault;
	///////////////////////////////////////////////////////////

	FORCEINLINE USkeletalMeshComponent* GetPawnMesh() { return IsFirstPerson() ? Mesh1P : GetMesh(); }

	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	FORCEINLINE float GeCurrentShield() const { return CurrentShield; }
	FORCEINLINE float GetCurrentHealth() const{ return CurrentHealth; }
	FORCEINLINE float GetShieldPercent() const { return CurrentShield/MaxShield; }
	


#pragma region Movement

	/** Governs the horizontal camera turn rate for controllers. Other factors may influence this rate. */
	UPROPERTY(VisibleAnywhere, Category = Camera)
	float BaseTurnRate;

	/** Governs the vertical camera turn rate for controllers. Other factors may influence this rate. */
	UPROPERTY(VisibleAnywhere, Category = Camera)
	float BaseLookUpRate;

protected:
	/**
	 * Moves the player pawn forward at the speed set CGMovementComponent.
	 * @param Val Multiplicative factor.
	 */
	void MoveForward(float Val);

	/**
	* Moves the player pawn right at the speed set CGMovementComponent.
	* @param Val Multiplicative factor.
	*/
	void MoveRight(float Val);

	/**
	* Turns the pawn at the rate of Rate times BaseTurnRate.
	* @param Rate The multiplicative factor of the turn.
	*/
	void TurnAtRate(float Rate);

	/**
	* Piteches the pawn's camera at the BaseLookUp rate times the Rate.
	* @param Rate The multiplicative factor of the pitch.
	*/
	void LookUpAtRate(float Rate);
	
#pragma endregion

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region Input
public:
	/** Initiates the fire for current weapon. */
	void StartFire();

	/** Stops the fire for current weapon. */
	void StopFire();

	/** Triggers the reload for current weapon. */
	void OnReload();

	/** Changes the equipped weapon to the next one in the Inventory Weapon array. */
	void NextWeapon();

	/** Changes the equipped weapon to the previous one in the Inventory Weapon array. */
	void PreviousWeapon();

	/** Zooms the player's view, may trigger ADS. */
	void StartZoom();

	/** Unzooms the player's view, may stop ADS. */
	void StopZoom();

	void SetZoom(bool bZoom);

	// This will eventually telegraph the ADS animation montage to clients.
	UFUNCTION(reliable, server, WithValidation)
	void ServerSetZoom(bool bZoom);
	virtual bool ServerSetZoom_Validate(bool bZoom);
	virtual void ServerSetZoom_Implementation(bool bZoom);
	
	/** Triggers the action button response.*/
	void OnActionButton();

	UFUNCTION(reliable, server, WithValidation)
	void ServerPickUpCrystal();
	virtual bool ServerPickUpCrystal_Validate();
	virtual void ServerPickUpCrystal_Implementation();

	void PickupCrystal();

	/** Pops the most powerful crystal in the gun. */
	void OnPopCrystal();

	UFUNCTION(reliable, server, WithValidation)
	void ServerPopCrystal();
	virtual bool ServerPopCrystal_Validate();
	virtual void ServerPopCrystal_Implementation();

	void PopCrystal();


#pragma endregion

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region Inventory

protected:
	/** The name of the Socket/Bone on the skeleton the weapon attaches to. */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	FName WeaponAttachPoint;
	
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	FCGDefaultWeaponConfig DefaultWeaponConfig;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TSubclassOf<class ACGInventory> DefaultInventoryClass;

	/** The currently equipped weapon for the player. */
	UPROPERTY(BlueprintReadOnly, Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	class ACGWeapon* CurrentWeapon;

	/** The offhand weapon for the player. */
	UPROPERTY(BlueprintReadOnly, Transient)
	class ACGWeapon* OffHandWeapon;

	/** A pending weapon for equips. */
	UPROPERTY(Transient)
	ACGWeapon* PendingWeapon;
	
	UPROPERTY(Transient, ReplicatedUsing = OnRep_PendingCrystalPickup)
	class ACGCrystal* PendingCrystalPickup;
	

public:


	//FIXME Move to private
	UPROPERTY(BlueprintReadOnly,Transient, Replicated)
	class ACGInventory* Inventory;

	// TODO make me private
	/** Tracks whether or not the player is attempting to shoot the gun.*/
	UPROPERTY(Transient)
	uint32 bWantsToFire : 1;

	/**Prevents the player from dying twice.*/
	UPROPERTY(Transient)
	uint32 bIsDying : 1;

	/**
	* Sets up the current weapon and triggers the OnEquip and OnUnequip calls.
	* @param NewWeapon The Weapon that is equipped.
	*/
	void SetCurrentWeapon(ACGWeapon* NewWeapon, ACGWeapon* LastWeapon = NULL);

	FORCEINLINE ACGWeapon*  GetCurrentWeapon() const { return CurrentWeapon; };

	/** Returns the next weapon in the array. */
	FORCEINLINE ACGWeapon* GetOffHandWeapon() const { return OffHandWeapon; };
	
	/*
	 * Invoked when the weapon has been changed, the character performs the actual equip call.
	 */
	void WeaponChanged();


	/** Spawns the base inventory as specified in the Defaul WeaponClasses array.*/
	void SpawnBaseInventory();

	/** Destroys the inventory to ensure we don't have any stragglers on death.*/
	void DestroyInventory();

	/** 
	 * Adds the weapon to the Weapons Array. If the weapon is in the array don't add it again.
	 * Invokes the weapon's OnEnterInventory.
	 * @param NewWeapon the candidate weapon for addition.
	 */
	void AddWeapon(ACGWeapon* Weapon, ECGCrystalType Type = ECGCrystalType::NONE);

	/**
	 * [server,client] Equips the supplied weapon to the player.
	 * @param The weapon to equip.
	 */
	void EquipWeapon(ACGWeapon* Weapon);

	/**
	* [server]Equips the supplied weapon to the player.
	* @param The weapon to equip.
	*/
	UFUNCTION(reliable, server, WithValidation)
	void ServerEquipWeapon(ACGWeapon* Weapon);
	virtual bool ServerEquipWeapon_Validate(ACGWeapon* Weapon);
	virtual void ServerEquipWeapon_Implementation(ACGWeapon* Weapon);

	/** Invoked when the player begins to Overlap with a Crystal Pickup, triggers a prompt.*/
	void OnStartCrystalOverlap(class ACGCrystal* Crystal);

	/** Invoked when the player is no longer overlapping a crystal, verifies that the crystal is the one currently overlapped.*/
	void OnStopCrystalOverlap(class ACGCrystal* Crystal);

	UFUNCTION()
	void OnRep_PendingCrystalPickup();

	UFUNCTION()
	void OnRep_CrystalChanged();


	bool GiveAmmo(ECGAmmoType AmmoType, int32 Ammo);

	/** Retrieves the Weapon attach point's name. TODO make this return the actual appropriate point.*/
	FORCEINLINE FName GetWeaponAttachPoint() const { return WeaponAttachPoint; };

#pragma endregion

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region Replication
protected:
	/** Ensures remote clients have the same weapon information for the player.*/
	UFUNCTION()
	void OnRep_CurrentWeapon(ACGWeapon* LastWeapon);


#pragma endregion

};
