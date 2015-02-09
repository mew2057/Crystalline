// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CrystallinePlayer.h"
#include "CrystallinePlayerMoveComponent.h"


ACrystallinePlayer::ACrystallinePlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrystallinePlayerMoveComponent>(ACharacter::CharacterMovementComponentName))
{
	// Create a CameraComponent	
	FirstPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;


	// Creates a mesh component to be used in the first person view. This is edited in the blueprint.
	Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("CharacterMesh1P"));
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->bOnlyOwnerSee = true; // NOTE this should be true.
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -90); // Relative location of the mesh to the origin of the player.
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);

	// Set the Collision Capsule Size (TODO hitbox!)
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Movement
	// Set turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	bRunning = false;

	RunningSpeedModifier = 2.2f;


	BaseHealth = 100.0f;
	MaxHealth = 100.0f;
	Health = BaseHealth;
	bAlive = true;

}

void ACrystallinePlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	

	if (Role == ROLE_Authority)
	{
		SpawnInventory();
	}
}

void ACrystallinePlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


float ACrystallinePlayer::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	// TODO make this mean something
	//// Modify based on game rules.
	//AShooterGameMode* const Game = GetWorld()->GetAuthGameMode<AShooterGameMode>();
	//Damage = Game ? Game->ModifyDamage(Damage, this, DamageEvent, EventInstigator, DamageCauser) : 0.f;

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;

		bAlive = Health > 0;
	}

	return ActualDamage;
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


ACrystallineWeapon* ACrystallinePlayer::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

ACrystallineWeapon* ACrystallinePlayer::GetSecondaryWeapon() const
{
	// XXX Might be room for improvement here, adding a weapon counter may remove some overhead. -John
	if (Weapons.Num() > 1)
	{
		return Weapons[(WeaponIndex + 1) % Weapons.Num()];
	}
	else
	{
		return nullptr;
	}
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

	InputComponent->BindAction("Fire", IE_Pressed, this, &ACrystallinePlayer::StartFire);
	InputComponent->BindAction("Fire", IE_Released, this, &ACrystallinePlayer::StopFire);

	InputComponent->BindAction("Reload", IE_Pressed, this, &ACrystallinePlayer::OnReload);

	// This is either ADS or something else, weapon dependant.
	InputComponent->BindAction("AltFire", IE_Pressed, this, &ACrystallinePlayer::StartAltFire);
	InputComponent->BindAction("AltFire", IE_Released, this, &ACrystallinePlayer::StopAltFire);


	InputComponent->BindAction("NextWeapon", IE_Pressed, this, &ACrystallinePlayer::NextWeapon);
	InputComponent->BindAction("PreviousWeapon", IE_Pressed, this, &ACrystallinePlayer::PreviousWeapon);

	
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

//////////////////////////////
// Weapon Functions
void ACrystallinePlayer::StartFire()
{
	if (CurrentWeapon)
	{
		// TODO add more checks and networking stuff here.
		CurrentWeapon->StartFire();
	}
}

void ACrystallinePlayer::StopFire()
{
	if (CurrentWeapon)
	{
		// TODO add more checks and networking stuff here.
		CurrentWeapon->StopFire();
	}
	 // TODO have a stop fire action.
	 // Likey use Tick.
}

void ACrystallinePlayer::OnReload()
{
	// Check if the gun can reload.
		// Reload.

}

void ACrystallinePlayer::StartAltFire()
{
	UE_LOG(LogTemp, Log, TEXT("start alt fire"));

	if (CurrentWeapon)
	{
		// TODO add more checks and networking stuff here.
		//CurrentWeapon->StartFire();
	}
}

void ACrystallinePlayer::StopAltFire()
{
	UE_LOG(LogTemp, Log, TEXT("stop alt fire"));

	// TODO have a stop fire action.
	// Likey use Tick.
}

void ACrystallinePlayer::NextWeapon()
{
	if (Weapons.Num() > 0)
	{
		// TODO Add animation call.
		WeaponIndex = (WeaponIndex + 1) % Weapons.Num();
		EquipWeapon(Weapons[WeaponIndex]);
	}

}

void ACrystallinePlayer::PreviousWeapon()
{
	// Cache the number of weapons.
	const int32 NumberOfWeapons = Weapons.Num();

	// Check for undeflow.
	if (NumberOfWeapons > 1)
	{
		// TODO Add animation call.MoveForward
		// This removes the need for a branch.
		WeaponIndex = (WeaponIndex - 1 + NumberOfWeapons) % NumberOfWeapons;

		EquipWeapon(Weapons[WeaponIndex]);
	}
}


//////////////////////////////
// Movement Functions
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

#pragma region Inventory

void ACrystallinePlayer::SpawnInventory()
{
	// TODO make sure the invokee owns the inventory in question.

	if (Role < ROLE_Authority)
	{
		return;
	}

	const int32 NumWeapons = DefaultWeaponClasses.Num();

	for (int32 i = 0; i < NumWeapons; ++i)
	{
		// If it exists spawn the weapon.
		if (DefaultWeaponClasses[i])
		{
			FActorSpawnParameters spawnInfo;
			// Spawn the weapon even if it is collinding with an object.
			spawnInfo.bNoCollisionFail = true;

			// Spawn an actor of type ACrystallineWeapon with the DefaultWeaponClasses[i] as the archetype, and the spawnInfo settings.
			ACrystallineWeapon* NewWeapon = GetWorld()->SpawnActor<ACrystallineWeapon>(DefaultWeaponClasses[i], spawnInfo);

			AddWeapon(NewWeapon);
		}
	}

	// TODO actually equip the weapon.

	if (NumWeapons > 0 && Weapons[0])
	{
		WeaponIndex = 0;
		EquipWeapon(Weapons[0]);
	}

}

void ACrystallinePlayer::DestroyInventory()
{
	for (int32 i = Weapons.Num() - 1; i > 0; --i)
	{
		ACrystallineWeapon* Weapon = Weapons[i];
		if (Weapon)
		{
			RemoveWeapon(Weapon);
			// Invoke the weapon's destroy command.

			Weapon->Destroy();
		}
	}

	// Eject ammunition as appropriate.
}

void ACrystallinePlayer::AddWeapon(ACrystallineWeapon* Weapon)
{
	// Only the Serever can add a weapon!
	if (Weapon && Role == ROLE_Authority)
	{
		Weapons.AddUnique(Weapon);
		Weapon->OnEnterInventory(this);
	}
}


void ACrystallinePlayer::RemoveWeapon(ACrystallineWeapon* Weapon)
{
	// Only the server can remove a weapon.
	if (Weapon && Role == ROLE_Authority)
	{
		Weapon->OnExitInventory();
		Weapons.RemoveSingle(Weapon);

	}
}

void ACrystallinePlayer::EquipWeapon(ACrystallineWeapon* NewWeapon)
{
	if (NewWeapon)
	{
		if (Role == ROLE_Authority)
		{
			SetCurrentWeapon(NewWeapon);
		}
		else
		{
			ServerEquipWeapon(NewWeapon);
		}
	}
}

bool ACrystallinePlayer::ServerEquipWeapon_Validate(ACrystallineWeapon* NewWeapon)
{
	return true;
}


void ACrystallinePlayer::ServerEquipWeapon_Implementation(ACrystallineWeapon* NewWeapon)
{
	EquipWeapon(NewWeapon);
}

void ACrystallinePlayer::SetCurrentWeapon(ACrystallineWeapon* NewWeapon, ACrystallineWeapon* LastWeapon)
{
	ACrystallineWeapon* LocalLastWeapon = NULL;

	if (LastWeapon != NULL)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}

	if (LocalLastWeapon)
	{
		LocalLastWeapon->OnUnEquip();
	}

	CurrentWeapon = NewWeapon;

	if (NewWeapon)
	{
		CurrentWeapon->SetOwningPawn(this);
		CurrentWeapon->OnEquip();
	}
}

FName ACrystallinePlayer::GetWeaponAttachPoint() const
{
	return WeaponAttachPoint;
}


#pragma endregion

//////////////////////////////////////////////////////////////////////////
// Replication

// XXX There is something here that I'm not sure of.
void ACrystallinePlayer::OnRep_CurrentWeapon(ACrystallineWeapon* LastWeapon)
{

	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void ACrystallinePlayer::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// Only local owner.
	DOREPLIFETIME_CONDITION(ACrystallinePlayer, Weapons, COND_OwnerOnly);


	// Everyone.
	DOREPLIFETIME(ACrystallinePlayer, CurrentWeapon);
	DOREPLIFETIME(ACrystallinePlayer, Health);
}



//////////////////////////////////////////////////////////////////////////


float ACrystallinePlayer::GetHealth() const
{
	return Health;
}

float ACrystallinePlayer::GetMaxHealth() const
{
	return MaxHealth;
}