// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "CGCharacter.generated.h"

/**
 * 
 */
UCLASS(config = Game)
class CRYSTALLINE_API ACGCharacter : public ACharacter
{
	GENERATED_BODY()

	ACGCharacter(const FObjectInitializer& PCIP);

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

protected:
	/** Max player shield amount. This is decayed before the health.*/
	UPROPERTY(EditDefaultsOnly, Category=Config)
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
	
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Begin the shield regeneration and set the player's CurrentHealth to max.*/
	void StartShieldRegen();

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
protected:
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

#pragma endregion

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region Inventory

protected:
	/** The name of the Socket/Bone on the skeleton the weapon attaches to. */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	FName WeaponAttachPoint;

	/** The Default list of weapons the player is carrying. */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<TSubclassOf<class ACGWeapon>> DefaultWeaponClasses;

	UPROPERTY(Transient, Replicated) // Transient- Empty on creation; Replicated- Replicated on server. 
	TArray<class ACGWeapon*> Weapons;

	/** The currently equipped weapon for the player. */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	class ACGWeapon* CurrentWeapon;

	//XXX This is getting removed when I get the crystal system in.
	/** The index of the currently equipped weapon. */
	uint32 WeaponIndex;


public:
	/**
	* Sets up the current weapon and triggers the OnEquip and OnUnequip calls.
	* @param NewWeapon The Weapon that is equipped.
	* @param LastWeapon [Optional] Included for the sake of ensuring replication unequips the right item.
	*/
	void SetCurrentWeapon(ACGWeapon* NewWeapon, ACGWeapon* LastWeapon = NULL);

	/** Spawns the base inventory as specified in the Defaul WeaponClasses array.*/
	void SpawnBaseInventory();

	/** 
	 * Adds the weapon to the Weapons Array. If the weapon is in the array don't add it again.
	 * Invokes the weapon's OnEnterInventory.
	 * @param NewWeapon the candidate weapon for addition.
	 */
	void AddWeapon(ACGWeapon* NewWeapon);

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
