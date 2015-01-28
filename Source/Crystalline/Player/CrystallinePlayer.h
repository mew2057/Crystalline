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

	

protected:

	/** Main Fire for the currently equipped weapon*/
	// TODO this needs to networked.
	void OnFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

protected:
	// APawn Interface 
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of Apawn Interface.


};
