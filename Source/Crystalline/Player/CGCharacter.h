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
	GENERATED_UCLASS_BODY()


	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/** Player health*/
	UPROPERTY(EditDefaultsOnly, Category=Config)
	float MaxShield;
	float CurrentShield;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	float MaxHealth;
	float CurrentHealth;
	

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

#pragma region Movement

	/** Governs the horizontal camera turn rate for controllers. Other factors may influence this rate. */
	UPROPERTY(VisibleAnywhere, Category = Camera)
		float BaseTurnRate;

	/** Governs the vertical camera turn rate for controllers. Other factors may influence this rate. */
	UPROPERTY(VisibleAnywhere, Category = Camera)
		float BaseLookUpRate;

	void MoveForward(float Val);

	void MoveRight(float Val);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);
	
#pragma endregion


#pragma region Input
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

#pragma region Inventory


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

	/** The index of the currently equipped weapon. */
	uint32 WeaponIndex;
	

	/**
	* Sets up the current weapon and triggers the OnEquip and OnUnequip calls.
	*
	*/
	void SetCurrentWeapon(ACGWeapon* NewWeapon, ACGWeapon* LastWeapon);
#pragma endregion


#pragma region Replication

	/** Ensures remote clients have the same weapon information for the player.*/
	UFUNCTION()
	void OnRep_CurrentWeapon(ACGWeapon* LastWeapon);


#pragma endregion

	
};
