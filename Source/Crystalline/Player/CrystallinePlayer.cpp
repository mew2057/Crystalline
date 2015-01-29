// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CrystallinePlayer.h"
#include "CrystallinePlayerMoveComponent.h"


ACrystallinePlayer::ACrystallinePlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrystallinePlayerMoveComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set the Collision Capsule Size (TODO hitbox!)
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Movement
	// Set turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	bRunning = false;

	RunningSpeedModifier = 2.2f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Creates a mesh component to be used in the first person view. This is edited in the blueprint.
	Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);	// Only the owner of this mesh will see it.
	Mesh1P->AttachParent = FirstPersonCameraComponent; // Sets the mesh as a child of the first person camera.
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f); // Relative location of the mesh to the origin of the player.
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;




}

//////////////////////////////////////////////////////////////////////////
#pragma region Gettors
bool ACrystallinePlayer::IsRunning() const
{
	return bRunning;
}

float ACrystallinePlayer::GetRunningSpeedModifier() const
{
	return RunningSpeedModifier;
}
#pragma endregion


//////////////////////////////////////////////////////////////////////////
#pragma region Input

void ACrystallinePlayer::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);

	// Actions
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("ToggleRunning", IE_Pressed, this, &ACrystallinePlayer::ToggleRunning);

	// Axis
	InputComponent->BindAxis("MoveForward", this, &ACrystallinePlayer::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ACrystallinePlayer::MoveRight);
	
	///////////////
	// Look Around

	// Mouse Input
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// Controller Input
	InputComponent->BindAxis("TurnRate", this, &ACrystallinePlayer::TurnAtRate);
	InputComponent->BindAxis("LookUpRate", this, &ACrystallinePlayer::LookUpAtRate);

	///////////////

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

void ACrystallinePlayer::TurnAtRate(float Rate)
{
	// Use Delta to ensure smooth motion.
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}


void ACrystallinePlayer::LookUpAtRate(float Rate)
{
	// Use Delta to ensure smooth motion.
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());

}

void ACrystallinePlayer::ToggleRunning()
{
	bRunning = !bRunning;
}

#pragma endregion

//////////////////////////////////////////////////////////////////////////
