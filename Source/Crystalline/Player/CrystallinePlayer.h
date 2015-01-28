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

private:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;	
	
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

public:

	ACrystallinePlayer(const FObjectInitializer& ObjectInitializer);

	/** Get the running state. */
	UFUNCTION(BlueprintCallable, Category = Pawn)
	bool IsRunning() const;

	/** Get the Running Speed Modifier*/
	UFUNCTION(BlueprintCallable, Category = Pawn)
	float GetRunningSpeedModifier() const;
	

protected:

	UPROPERTY(VisibleAnywhere, Category = Movement)
	uint8 bRunning : 1;

	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float RunningSpeedModifier;

	/** Governs the horizontal camera turn rate for controllers. Other factors may influence this rate. */
	UPROPERTY(VisibleAnywhere, Category = Camera)
	float BaseTurnRate;

	/** Governs the vertical camera turn rate for controllers. Other factors may influence this rate. */
	UPROPERTY(VisibleAnywhere, Category = Camera)
	float BaseLookUpRate;

	/** Main Fire for the currently equipped weapon*/
	// TODO this needs to networked.
	void OnFire();

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



protected:
	// APawn Interface 
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of Apawn Interface.


};
