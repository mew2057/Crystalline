// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGCharacter.h"
#include "CGCharacterMovementComponent.h"


#pragma region Overrides

ACGCharacter::ACGCharacter(const FObjectInitializer& PCIP)
	: Super(PCIP.SetDefaultSubobjectClass<UCGCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Create a CameraComponent	
	FirstPersonCameraComponent = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;


	// Creates a mesh component to be used in the first person view. This is edited in the blueprint.
	Mesh1P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("CharacterMesh1P"));
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->bOnlyOwnerSee = false; // NOTE this should be true.
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -90); // Relative location of the mesh to the origin of the player.
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);

	// Set the Collision Capsule Size (TODO hitbox!)
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Set turn rates for input
	BaseTurnRate   = 45.f;
	BaseLookUpRate = 45.f;

	MaxShield            = 100.0f;
	CurrentShield        = MaxShield;
	ShieldRegenPerSecond = 50.f;
	TimeToRegen			= 2.f;
	bShieldRegenerating  = false;

	MaxHealth     = 10.0f;
	CurrentHealth = MaxHealth;
	PendingWeapon = false;
}

void ACGCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Only the authority should spwan the inventory.
	if (Role == ROLE_Authority)
	{
		CurrentShield = MaxShield;
		CurrentHealth = MaxHealth;
		SpawnBaseInventory();
	}

}

void ACGCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bShieldRegenerating)
	{
		CurrentShield = FMath::Min(MaxShield, CurrentShield + ShieldRegenPerSecond * DeltaSeconds);

		// The Shield is regenerating while this is true.
		bShieldRegenerating = CurrentShield < MaxShield;
	}
}

float ACGCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		// Stop the regeneration of the player's shields.
		bShieldRegenerating = false;

		CurrentShield -= ActualDamage;

		if (CurrentShield < 0)
		{
			CurrentHealth += CurrentShield;
			CurrentShield = 0;
		}

		// Set a timer for to start the shield regeneration for the player, if one is set this should reset the time elapsed to zero.
		GetWorldTimerManager().SetTimer(this, &ACGCharacter::StartShieldRegen, TimeToRegen, false); // TODO Clear me on death!
	}

	return ActualDamage;
}


void ACGCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);

	// Actions
	InputComponent->BindAction("Jump", IE_Pressed,   this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released,  this, &ACharacter::StopJumping);
												     
	InputComponent->BindAction("Fire", IE_Pressed,   this, &ACGCharacter::StartFire);
	InputComponent->BindAction("Fire", IE_Released,  this, &ACGCharacter::StopFire);

	InputComponent->BindAction("Reload", IE_Pressed, this, &ACGCharacter::OnReload);

	/* XXX Not implemented!
	InputComponent->BindAction("AltFire", IE_Pressed,       this, &ACGCharacter::StartAltFire);
	InputComponent->BindAction("AltFire", IE_Released,      this, &ACGCharacter::StopAltFire);
	InputComponent->BindAction("ToggleRunning", IE_Pressed, this, &ACGCharacter::ToggleRunning);
	*/

	InputComponent->BindAction("NextWeapon",     IE_Pressed, this, &ACGCharacter::NextWeapon);
	InputComponent->BindAction("PreviousWeapon", IE_Pressed, this, &ACGCharacter::PreviousWeapon);

	// Axis
	InputComponent->BindAxis("MoveForward", this, &ACGCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight",   this, &ACGCharacter::MoveRight);

	///////////////
	// Look Around

	// Mouse Input
	InputComponent->BindAxis("Turn",   this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// Controller Input
	InputComponent->BindAxis("TurnRate",   this, &ACGCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUpRate", this, &ACGCharacter::LookUpAtRate);

	///////////////

}

void ACGCharacter::Restart()
{
	Super::Restart();

	/// XXX WRONG
	// This should be invoked after the inventory is spawned by the game mode.
	if (IsLocallyControlled() && Weapons.Num() > 0)
	{
		EquipWeapon(Weapons[0]);
	}
}

#pragma endregion



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region PlayerLife

void ACGCharacter::StartShieldRegen()
{
	bShieldRegenerating = true;

	// TODO Document why I decided to do this in the wiki or something. -John
	// The health is automatically recovered. The health is really only there to add the "shield break" feel.
	CurrentHealth = MaxHealth;
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region Movement

void ACGCharacter::MoveForward(float Val)
{
	if (Controller && Val != 0.0f)
	{
		// TODO falling.
		AddMovementInput(GetActorForwardVector(), Val);
	}
}

void ACGCharacter::MoveRight(float Val)
{
	if (Controller && Val != 0.0f)
	{
		// TODO falling.
		AddMovementInput(GetActorRightVector(), Val);
	}
}

void ACGCharacter::TurnAtRate(float Rate)
{
	// Use Delta to ensure smooth motion.
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}


void ACGCharacter::LookUpAtRate(float Rate)
{
	// Use Delta to ensure smooth motion.
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());

}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region Inventory

void ACGCharacter::SetCurrentWeapon(ACGWeapon* NewWeapon, ACGWeapon* LastWeapon)
{
	ACGWeapon* LocalLastWeapon = NULL;

	if (LastWeapon != NULL)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}
	
	// If there's an existing weapon it needs to be unequipped first.
	if (LocalLastWeapon != NULL && NewWeapon != NULL)
	{
		PendingWeapon = NewWeapon;
		// This may need something to prevent issues.
		// Add a check to see this is a legal operation.
		LocalLastWeapon->OnUnequip();
	}
	else if (NewWeapon != NULL)
	{
		PendingWeapon = NewWeapon;
		WeaponChanged();		
	}
}

void ACGCharacter::WeaponChanged()
{
	if (PendingWeapon != NULL)
	{
		CurrentWeapon = PendingWeapon;
		PendingWeapon = NULL;
		CurrentWeapon->SetCGOwner(this);
		CurrentWeapon->OnEquip();
	}
	else if (CurrentWeapon != NULL)
	{
		CurrentWeapon->SetCGOwner(this);
		CurrentWeapon->OnEquip();
	}
}

void ACGCharacter::SpawnBaseInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	const int32 NumDefaultWeapons = DefaultWeaponClasses.Num();

	for (int i = 0; i < NumDefaultWeapons; ++i)
	{
		// If it exists spawn the weapon.
		if (DefaultWeaponClasses[i])
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.bNoCollisionFail = true;

			// Spawn an actor of type ACrystallineWeapon with the DefaultWeaponClasses[i] as the archetype, and the spawnInfo settings.
			ACGWeapon* NewWeapon = GetWorld()->SpawnActor<ACGWeapon>(DefaultWeaponClasses[i], SpawnInfo);

			AddWeapon(NewWeapon);
		}
	}
	
	if (Weapons.Num() > 0)
	{
		WeaponIndex = 0;
		EquipWeapon(Weapons[0]);
	}

}

void ACGCharacter::AddWeapon(ACGWeapon* NewWeapon)
{
	if (NewWeapon && Role == ROLE_Authority)
	{
		Weapons.AddUnique(NewWeapon);
		NewWeapon->OnEnterInventory(this);
	}
}

void ACGCharacter::EquipWeapon(ACGWeapon* Weapon)
{
	if (Weapon)
	{
		// Only the authority should change the current weapon.
		if (Role == ROLE_Authority)
		{
			SetCurrentWeapon(Weapon);
		}
		else
		{
			ServerEquipWeapon(Weapon);
		}
	}
}


void ACGCharacter::ClientSetWeapon_Implementation(ACGWeapon* Weapon)
{
	SetCurrentWeapon(Weapon);
	
	// Tell the server that it needs to set the weapon to be safe.
	if (Role < ROLE_Authority)
	{
		ServerEquipWeapon(Weapon);
	}
}

bool ACGCharacter::ServerEquipWeapon_Validate(ACGWeapon* NewWeapon)
{
	return true;
}


void ACGCharacter::ServerEquipWeapon_Implementation(ACGWeapon* NewWeapon)
{
	if (NewWeapon != NULL)
	{
		SetCurrentWeapon(NewWeapon);
	}
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region Input
/** Initiates the fire for current weapon. */
void ACGCharacter::StartFire()
{
	 // TODO cache fire?
	if (IsLocallyControlled() && CurrentWeapon != NULL)
	{
		CurrentWeapon->StartFire();
	}
}

/** Stops the fire for current weapon. */
void ACGCharacter::StopFire()
{
	if (CurrentWeapon != NULL)
	{
		CurrentWeapon->StopFire();
	}
}

/** Triggers the reload for current weapon. */
void ACGCharacter::OnReload()
{

}

/** Changes the equipped weapon to the next one in the Inventory Weapon array. */
void ACGCharacter::NextWeapon()
{
	// Cache the number of weapons.
	const int32 NumberOfWeapons = Weapons.Num();

	if (NumberOfWeapons > 1)
	{
		WeaponIndex = (WeaponIndex + 1) % NumberOfWeapons;
		EquipWeapon(Weapons[WeaponIndex]);
	}
}

/** Changes the equipped weapon to the previous one in the Inventory Weapon array. */
void ACGCharacter::PreviousWeapon()
{
	// Cache the number of weapons.
	const int32 NumberOfWeapons = Weapons.Num();

	// Check for undeflow.
	if (NumberOfWeapons > 1)
	{
		WeaponIndex = (WeaponIndex - 1 + NumberOfWeapons) % NumberOfWeapons;

		EquipWeapon(Weapons[WeaponIndex]);
	}
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region Replication

void ACGCharacter::OnRep_CurrentWeapon(ACGWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void ACGCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	// Not sure about this one.
	DOREPLIFETIME_CONDITION(ACGCharacter, Weapons, COND_OwnerOnly);


	// Everyone.
	DOREPLIFETIME(ACGCharacter, CurrentWeapon);
	DOREPLIFETIME(ACGCharacter, CurrentHealth);
	DOREPLIFETIME(ACGCharacter, CurrentShield);

}

#pragma endregion