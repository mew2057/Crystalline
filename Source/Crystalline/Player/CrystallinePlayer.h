// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "CrystallinePlayer.generated.h"

/**
 * 
 */
UCLASS(config = Game)
class CRYSTALLINE_API ACrystallinePlayer : public ACharacter
{
	GENERATED_BODY()

public:
	/** Constuctor */
	ACrystallinePlayer(const FObjectInitializer& ObjectInitializer);

#pragma region Overriden_Functions

protected:
	// APawn Interface 
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of Apawn Interface.

	virtual void PostInitializeComponents() override;

	/** Updates the character on tick. */
	virtual void Tick(float DeltaSeconds) override;


#pragma endregion

#pragma region Components

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

private:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;
	
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

#pragma endregion
	
#pragma region Input_Control

#pragma region Fields

public:
	/** Get the running state. */
	UFUNCTION(BlueprintCallable, Category = Pawn)
	bool IsRunning() const;

	/** Get the Running Speed Modifier*/
	UFUNCTION(BlueprintCallable, Category = Pawn)
	float GetRunningSpeedModifier() const;

protected:
	/** The State Variable for the */
	UPROPERTY(VisibleAnywhere, Category = Movement)
	uint8 bRunning : 1;

	/** Defines the multiplicative increase of the running speed of the player. */
	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float RunningSpeedModifier;

	/** Governs the horizontal camera turn rate for controllers. Other factors may influence this rate. */
	UPROPERTY(VisibleAnywhere, Category = Camera)
	float BaseTurnRate;

	/** Governs the vertical camera turn rate for controllers. Other factors may influence this rate. */
	UPROPERTY(VisibleAnywhere, Category = Camera)
	float BaseLookUpRate;

#pragma endregion

#pragma region Functions

protected:

	/** Start Main Fire for the currently equipped weapon*/
	// TODO this needs to networked.
	void StartFire();

	/** Stop Main Fire for the currently equipped weapon*/
	// TODO this needs to networked.
	void StopFire();

	/** Reload the weapon. */
	void OnReload();

	/** Start the alternate fire. */
	void StartAltFire ();

	/** Stop the alternate fire (if appropriate). */
	void StopAltFire ();

	/** Changes the equipped weapon to the next one in the Inventory Weapon array. */
	void NextWeapon();

	/** Changes the equipped weapon to the previous one in the Inventory Weapon array. */
	void PreviousWeapon();
	
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	* Invoked for Controllers, the mouse uses the default yaw function.
	* @param Rate the Normalized rate, -1.0 <-> 1.0
	*/
	void TurnAtRate(float Rate);

	/**
	* Invoked for Controllers, the mouse uses the default pitch function.
	* @param Rate the Normalized rate, -1.0 <-> 1.0
	*/
	void LookUpAtRate(float Rate);

	/**
	* Toggles the player's speed between running and walking.
	*/
	void ToggleRunning();

#pragma endregion
	
#pragma endregion


// I'm not a Fan of "God classes", but I'd rather not wrestle with the editor too much. -John
#pragma region Inventory

#pragma region Fields

protected:

	/** The name of the Socket/Bone on the skeleton the weapon attaches to. */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	FName WeaponAttachPoint;

	/** The Default list of weapons the player is carrying. */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TArray<TSubclassOf<class ACrystallineWeapon>> DefaultWeaponClasses;

	UPROPERTY(Transient, Replicated) // Transient- Empty on creation; Replicated- Replicated on server. 
	TArray<class ACrystallineWeapon*> Weapons; // This list shouldn't change after spawn.

	/** The currently equipped weapon for the player. */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	class ACrystallineWeapon* CurrentWeapon;

	/** The index of the currently equipped weapon. */
	uint32 WeaponIndex;
	

#pragma region Functions
public:
	/** Retrieve the FName of the weapon attach point. */
	FName GetWeaponAttachPoint() const;

	/**
	 * Gets the currently equpped weapon.
	 * @return A reference to the weapon held by the player.
	 */
	class ACrystallineWeapon* GetCurrentWeapon() const;


	/**
	* Gets the weapon in reserve for the player.
	* @return A reference to the off hand weapon for the player.
	*/
	class ACrystallineWeapon* GetSecondaryWeapon() const;


protected:
	/** Spawns the inventory of the player, used on respawn/construction. */
	void SpawnInventory();

	/** Despawns the inventory of the player, used on death. */
	void DestroyInventory();

	/** Adds a weapon to the inventory in a standardized manner. */
	void AddWeapon(ACrystallineWeapon* Weapon);

	/** Removes a weapon from the inventory in a standardized manner. */
	void RemoveWeapon(ACrystallineWeapon* Weapon);


	void EquipWeapon(class ACrystallineWeapon* NewWeapon);

	void SetCurrentWeapon(class ACrystallineWeapon* NewWeapon, class ACrystallineWeapon* LastWeapon = NULL);

	/** current weapon rep handler */
	UFUNCTION()
	void OnRep_CurrentWeapon(class ACrystallineWeapon* LastWeapon);

	UFUNCTION(reliable, server, WithValidation)
	void ServerEquipWeapon(class ACrystallineWeapon* NewWeapon);

	


#pragma endregion


};
