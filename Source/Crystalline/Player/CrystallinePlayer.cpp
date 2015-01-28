// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CrystallinePlayer.h"

ACrystallinePlayer::ACrystallinePlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//////////////////////////////////////////////////////////////////////////
// Input
void ACrystallinePlayer::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);

	// Actions

	// Axis
	InputComponent->BindAxis("MoveForward", this, &ACrystallinePlayer::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ACrystallinePlayer::MoveRight);

	// Look Around.

}

void ACrystallinePlayer::MoveForward(float Val)
{
	// TODO check for controller.
	if (Val != 0.0f)
	{
		// TODO falling.

		AddMovementInput(GetActorForwardVector(), Val);
	}
}

void ACrystallinePlayer::MoveRight(float Val)
{
	// TODO check for controller.
	if ( Val != 0.0f)
	{
		// TODO falling.

		AddMovementInput(GetActorRightVector(), Val);
	}

}


