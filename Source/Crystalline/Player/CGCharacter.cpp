// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGCharacter.h"
#include "GameModes/CGBaseGameMode.h"
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
	Mesh1P->bOnlyOwnerSee = true; 
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -90); // Relative location of the mesh to the origin of the player.
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh1P->SetHiddenInGame(false);


	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);


	// Set the Collision Capsule Size (TODO hitbox!)
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	// Set turn rates for input
	BaseTurnRate   = 45.f;
	BaseLookUpRate = 45.f;

	MaxShield            = 100.0f;
	CurrentShield        = MaxShield;
	WarningShieldPercent = .42f;
	ShieldRegenPerSecond = 50.f;
	ShieldTimeToRegen    = 2.f;
	bShieldRegenerating  = false;

	bZooming	  = false;
	bZoomed       = false;
	MaxHealth     = 10.0f;
	CurrentHealth = MaxHealth;
	PendingWeapon = NULL;
	bWantsToFire  = false;
	bIsDying = false;
	HeadRadius = 16.f;
	HeadOffset = 6.f;
	HeadBone = FName("Skull");
	StrafeFactor = .8f;
	// TODO preload Inventory!
}

void ACGCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Initializes the meshes
	UpdatePawnMeshes();
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

	// XXX ZOOM ZOOM ZOOM
	if (IsLocallyControlled() && bZooming )
	{
		float CurrentFOV = FirstPersonCameraComponent->FieldOfView;
		//
		CurrentFOV = FMath::FInterpTo(CurrentFOV, CurrentZoom.TargetZoom, DeltaSeconds, CurrentZoom.ZoomSpeed);
		
		if (FMath::Abs(CurrentFOV - CurrentZoom.TargetZoom) < 1.f)
		{
			bZooming = false;
			CurrentFOV = CurrentZoom.TargetZoom;
		}

		FirstPersonCameraComponent->FieldOfView = CurrentFOV;
	}
}

void ACGCharacter::SetPlayerDefaults()
{
	// Init the Zoom to be sure.
	CurrentZoom.InitZoom();
	FirstPersonCameraComponent->FieldOfView = FOVDefault;

	// Only the authority should spwan the inventory.
	if (Role == ROLE_Authority)
	{
		CurrentShield = MaxShield;
		CurrentHealth = MaxHealth;
		SpawnBaseInventory();
	}
}

void ACGCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	
	// Move to 1st Person.
	UpdatePawnMeshes();

	// XXX Replace me with a function call.
	// Ensures that the current weapon is properly equipped.
	if (CurrentWeapon)
	{
		CurrentWeapon->SetCGOwner(this);
		CurrentWeapon->OnEquip();
	}
}

void ACGCharacter::UpdatePawnMeshes()
{
	bool const bFirstPerson = IsFirstPerson();

	Mesh1P->MeshComponentUpdateFlag = !bFirstPerson ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Mesh1P->SetOwnerNoSee(!bFirstPerson);

	GetMesh()->MeshComponentUpdateFlag = bFirstPerson ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetOwnerNoSee(bFirstPerson);
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

		if (CurrentHealth <= 0)
		{
			//Die
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			// Set a timer for to start the shield regeneration for the player, if one is set this should reset the time elapsed to zero.
			GetWorldTimerManager().SetTimer(TimerHandle_ShieldRegen, this, &ACGCharacter::StartShieldRegen, ShieldTimeToRegen, false); // TODO Clear me on death!
			
			// TODO Feedback from hit, e.g. force feedback and direction.
			PlayHit(ActualDamage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
		}		
	}

	return ActualDamage;
}

bool ACGCharacter::CantDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const
{
	return bIsDying
		|| Role != ROLE_Authority //Not Authority.
		|| IsPendingKill()	// Destroyed.
		|| GetWorld()->GetAuthGameMode() == NULL // No Game Mode.
		|| GetWorld()->GetAuthGameMode()->GetMatchState() == MatchState::LeavingMap; // Leaving the Match
}


bool ACGCharacter::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	if (CantDie(KillingDamage, DamageEvent, Killer, DamageCauser))
	{
		return false;
	}

	CurrentHealth = FMath::Min(0.0f, CurrentHealth);

	// Get the damage Type then  get the killer, mainly for environmental damage.s
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	AController* const KilledPlayer = (Controller != NULL) ? Controller : Cast<AController>(GetOwner());
	GetWorld()->GetAuthGameMode<ACGBaseGameMode>()->Killed(Killer, KilledPlayer,  DamageType);


	NetUpdateFrequency = GetDefault<ACGCharacter>()->NetUpdateFrequency;
	GetCharacterMovement()->ForceReplicationUpdate();

	OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);

	return true;
}

void ACGCharacter::OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	// If already dying, don't bother dying again.
	if (bIsDying)
	{
		return;
	}

	// Stop Replicating the Character and start dying.
	bReplicateMovement = false;
	bTearOff = true;
	bIsDying = true;
	
	// If the authority
	if (Role == ROLE_Authority)
	{
		// Play death feedback.
		ReplicateHit(KillingDamage, DamageEvent, PawnInstigator, DamageCauser, true);
		
		// TODO force feedback.
	}

	// Disables collisions.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	//TODO  Update Meshes,  stop all warnings.

	// Clears out any and all timers for the object.
	GetWorldTimerManager().ClearAllTimersForObject(this);

	// Set the meshes to the appropriate 3rd person view.
	UpdatePawnMeshes();

	// Clear the inventory, we don't want any guns.
	DestroyInventory();

	// Detach the controller from the pawn, so respawn can work.
	DetachFromControllerPendingDestroy();

	// Clean up the HUD XXX this doesn't work for clients!
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	ACGPlayerHUD* HUD = PlayerController ? Cast<ACGPlayerHUD>(PlayerController->GetHUD()) : NULL;
	if (HUD)
	{
		HUD->SetScoreboardVisibility(true);
		HUD->SetPromptMessage(false);
	}

	// Once the Pawn is destroyed the playercontroller will spawn a new pawn, see UnFreeze()
	StopAllAnim();

	/// Ragdoll
	/////////////////////////////////////////////////////
	
	if (GetMesh() && GetMesh()->GetPhysicsAsset())
	{
		static FName CollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetCollisionProfileName(CollisionProfileName);
		
		SetActorEnableCollision(true);
		
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = true;

		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->SetComponentTickEnabled(false);
	}
	
	/////////////////////////////////////////////////////

	SetLifeSpan(5.f);

	// Play the hit notifier for killer.
	if (PawnInstigator && PawnInstigator != this && PawnInstigator->IsLocallyControlled())
	{
		APlayerController* InstigatorPC = Cast<APlayerController>(PawnInstigator->Controller);
		ACGPlayerHUD* InstHUD = InstigatorPC ? Cast<ACGPlayerHUD>(InstigatorPC->GetHUD()) : NULL;
		if (InstHUD)
		{
			InstHUD->NotifyHitConfirmed();
		}
	}
}

void ACGCharacter::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	if (Role == ROLE_Authority)
	{
		// Play death feedback.
		ReplicateHit(DamageTaken, DamageEvent, PawnInstigator, DamageCauser, false);

		// TODO force feedback.
	}

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	ACGPlayerHUD* HUD = PlayerController ? Cast<ACGPlayerHUD>(PlayerController->GetHUD()) : NULL;
	if (HUD)
	{
		//(DamageCauser->GetActorLocation() - GetActorLocation()).GetSafeNormal2D()
		HUD->NotifyHitTaken(DamageCauser);
	}

	if (PawnInstigator && PawnInstigator != this && PawnInstigator->IsLocallyControlled())
	{
		APlayerController* InstigatorPC = Cast<APlayerController>(PawnInstigator->Controller);
		ACGPlayerHUD* InstHUD = InstigatorPC ? Cast<ACGPlayerHUD>(InstigatorPC->GetHUD()) : NULL;
		if (InstHUD)
		{
			InstHUD->NotifyHitConfirmed();
		}
	}
	
	// Applies an impulse to the character. This was outlined in Unreal Sample Projects.
	if (DamageTaken > 0.f)
	{
		ApplyDamageMomentum(DamageTaken, DamageEvent, PawnInstigator, DamageCauser);
	}	
}

void ACGCharacter::ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser, bool bKilled)
{
	// TODO manage Same Frame Damage.
	// Don't replicate again.
	if (bKilled && LastHit.bKillingHit)
	{
		return;
	}

	LastHit.Damage = Damage;
	LastHit.Instigator = Cast<ACGCharacter>(PawnInstigator);
	LastHit.DamageCauser = DamageCauser;
	LastHit.bKillingHit = bKilled;
	LastHit.DamageEvent = DamageEvent;
	LastHit.EnsureReplication();
}

bool ACGCharacter::IsHeadShot(const FVector& ImpactLocation, const FVector& ShotDir)
{
	// Offset the head.
	FVector Head = GetMesh()->GetSocketLocation(HeadBone);
	Head.Z += HeadOffset;

	// PointDistToLine is used because it effectively provides penetration for the bullet (if I'm understanding it right).
	// Due to the capsule collider we don't always have the impact location touching the head radius.
	// Computes the distance, but this is not called every frame.
	return FMath::PointDistToLine(Head, ShotDir, ImpactLocation) <= HeadRadius;
}

void ACGCharacter::OnRep_LastHit()
{

	if (LastHit.bKillingHit)
	{
		// TODO Clean up damage event stuff.
		OnDeath(LastHit.Damage, LastHit.DamageEvent, LastHit.Instigator.Get(), LastHit.DamageCauser.Get());
	}
	else
	{
		PlayHit(LastHit.Damage, LastHit.DamageEvent, LastHit.Instigator.Get(), LastHit.DamageCauser.Get());
	}
}

float ACGCharacter::PlayAnimMontage(class UAnimMontage * AnimMontage, float InPlayRate, FName StartSectionName)
{
	USkeletalMeshComponent* SkeletalMesh = GetPawnMesh();
	if (AnimMontage && SkeletalMesh && SkeletalMesh->AnimScriptInstance)
	{
		return SkeletalMesh->AnimScriptInstance->Montage_Play(AnimMontage, InPlayRate);
	}
	return 0.f;
}

void ACGCharacter::StopAnimMontage(class UAnimMontage* AnimMontage)
{
	USkeletalMeshComponent* SkeletalMesh = GetPawnMesh();
	if (AnimMontage && SkeletalMesh && SkeletalMesh->AnimScriptInstance &&
		SkeletalMesh->AnimScriptInstance->Montage_IsPlaying(AnimMontage))
	{
		SkeletalMesh->AnimScriptInstance->Montage_Stop(AnimMontage->BlendOutTime);
	}
}


void ACGCharacter::StopAllAnim()
{
	USkeletalMeshComponent* SkeletalMesh = GetPawnMesh();
	if (SkeletalMesh && SkeletalMesh->AnimScriptInstance)
	{
		SkeletalMesh->AnimScriptInstance->Montage_Stop(0.f);
	}
}

void ACGCharacter::TornOff()
{
	SetLifeSpan(5.f);
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

	InputComponent->BindAction("ActionButton", IE_Pressed, this, &ACGCharacter::OnActionButton);
	InputComponent->BindAction("PopCrystalButton", IE_Pressed, this, &ACGCharacter::OnPopCrystal);

	InputComponent->BindAction("Zoom", IE_Pressed, this, &ACGCharacter::StartZoom);
	InputComponent->BindAction("Zoom", IE_Released, this, &ACGCharacter::StopZoom);

/*
void ACGCharacter::SetPromptMessage(bool bSetPrompt, const FString& Message, int32 ButtonID)
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	ACGPlayerHUD* HUD = PlayerController ? Cast<ACGPlayerHUD>(PlayerController->GetHUD()) : NULL;
	if (HUD)
	{
		HUD->SetPromptMessage(bSetPrompt, Message, ButtonID);
	}
}*/

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
	/*
	/// XXX WRONG
	// This should be invoked after the inventory is spawned by the game mode.
	if (IsLocallyControlled() && Weapons.Num() > 0)
	{
		EquipWeapon(Weapons[0]);
	}*/
}

/** Make Sure the inventory is destroyed. */
void ACGCharacter::Destroyed() 
{
	Super::Destroyed();
	DestroyInventory();
}

#pragma endregion


void ACGCharacter::SetPromptMessage(bool bSetPrompt, const FString& Message, int32 ButtonID)
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	ACGPlayerHUD* HUD = PlayerController ? Cast<ACGPlayerHUD>(PlayerController->GetHUD()) : NULL;
	if (HUD)
	{
		HUD->SetPromptMessage(bSetPrompt, Message, ButtonID);
	}
}

bool ACGCharacter::IsFirstPerson()
{
	return Controller && Controller->IsLocalPlayerController();
}

bool ACGCharacter::IsAlive()
{
	return CurrentHealth > 0;
}


FRotator ACGCharacter::GetAimOffsets() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region PlayerLife

void ACGCharacter::StartShieldRegen()
{

	GetWorldTimerManager().ClearTimer(TimerHandle_ShieldRegen);

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
		AddMovementInput(GetActorRightVector(), Val * StrafeFactor);
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

ACGWeapon* ACGCharacter::GetOffHandWeapon() const
{ 
	return Inventory ? Inventory->GetNextWeapon(CurrentWeapon) : NULL; 
};


void ACGCharacter::WeaponChanged()
{
	// FIXME make this less jarring when you have time for polish -John.
	// Reset the zoom.
	bZoomed = bZooming = false;
	FirstPersonCameraComponent->FieldOfView = FOVDefault;

	if (PendingWeapon != NULL)
	{
		CurrentWeapon = PendingWeapon;
		PendingWeapon = NULL;
		CurrentWeapon->SetCGOwner(this);
		CurrentWeapon->OnEquip();
		CurrentZoom = CurrentWeapon->WeaponZoomConfig; // Change the zoom on weapon change.
	}
	else if (CurrentWeapon != NULL)
	{
		CurrentWeapon->SetCGOwner(this);
		CurrentWeapon->OnEquip();
		CurrentZoom = CurrentWeapon->WeaponZoomConfig; // Change the zoom on weapon change.
	}
}

void ACGCharacter::SpawnBaseInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoCollisionFail = true;

	Inventory = GetWorld()->SpawnActor<ACGInventory>(DefaultInventoryClass, SpawnInfo);
	if (Inventory)
	{
		Inventory->SetCGOwner(this);
		Inventory->InitializeInventory(DefaultWeaponConfig);
	}
}


void ACGCharacter::DestroyInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}
	
	if (Inventory)
	{
		Inventory->DestroyInventory();
		Inventory->Destroy();
	}
}



void ACGCharacter::EquipWeapon(ACGWeapon* Weapon, bool bCrystalChange)
{
	if (Weapon)
	{
		if (Role < ROLE_Authority)
		{
			ServerEquipWeapon(Weapon);
		}
		SetCurrentWeapon(Weapon);
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


void ACGCharacter::OnStartCrystalOverlap(class ACGCrystal* Crystal)
{
	// Only Execute this code if the crystal type is grabbable and the crystal is not null.
	if (Crystal && Inventory->CanLoadCrystal(Crystal->GetCrystalType()))
	{
		PendingCrystalPickup = Crystal;

		// If this is controlled locally post the prompt.
		if (IsLocallyControlled())
		{
			OnRep_PendingCrystalPickup();
		}
	}
}

void ACGCharacter::OnStopCrystalOverlap(class ACGCrystal* Crystal)
{
	// If the crystal is not the same, exit this logic since some degree of overlap shenanigans occured.
	if (PendingCrystalPickup && PendingCrystalPickup != Crystal)
	{
		return;
	}

	PendingCrystalPickup = NULL;

	// If this is controlled locally post the prompt.
	if (IsLocallyControlled())
	{
		OnRep_PendingCrystalPickup();
	}
}

void ACGCharacter::OnRep_PendingCrystalPickup()
{
	// Get the HUD
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	ACGPlayerHUD* HUD = PlayerController ? Cast<ACGPlayerHUD>(PlayerController->GetHUD()) : NULL;
	if (HUD )
	{

		if (PendingCrystalPickup == NULL)
		{
			HUD->SetPromptMessage(false);
		}
		else
		{
			HUD->SetPromptMessage(true, TEXT("to pickup crystal " + FString::FromInt((int8)(PendingCrystalPickup->GetCrystalType()))), ACTION_BUTTON);
		}
	}
}

bool ACGCharacter::GiveAmmo(ECGAmmoType AmmoType, int32 Ammo)
{
	return Inventory && Inventory->GiveAmmo(AmmoType, Ammo);
}


#pragma endregion

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region Input
/** Initiates the fire for current weapon. */
void ACGCharacter::StartFire()
{
	// TODO make sure bWantsToFire works.
	if (IsLocallyControlled() && CurrentWeapon != NULL )
	{
		if (CurrentWeapon->CanFire(true) && !bWantsToFire)
		{
			bWantsToFire = true;
			CurrentWeapon->StartFire();
		}
		else
		{
			CurrentWeapon->OnStartReload();
		}
	}
}

/** Stops the fire for current weapon. */
void ACGCharacter::StopFire()
{
	if (IsLocallyControlled() && CurrentWeapon != NULL && bWantsToFire)
	{
		bWantsToFire = false;
		CurrentWeapon->StopFire();
	}
}

/** Triggers the reload for current weapon. */
void ACGCharacter::OnReload()
{
	// Make sure the gun is able to be reloaded.
	if (IsLocallyControlled() && CurrentWeapon != NULL && CurrentWeapon->HasManualReload())
	{
		CurrentWeapon->OnStartReload();
	}
}

/** Changes the equipped weapon to the next one in the Inventory Weapon array. */
void ACGCharacter::NextWeapon()
{
	// Cache the number of weapons.
	const int32 NumberOfWeapons = Inventory->GetWeaponCount();
	int32 WeaponIndex = Inventory->GetWeaponIndex(CurrentWeapon);

	if (NumberOfWeapons > 1)
	{
		WeaponIndex = (WeaponIndex + 1) % NumberOfWeapons;
		EquipWeapon(Inventory->GetWeapon(WeaponIndex));
	}
}

/** Changes the equipped weapon to the previous one in the Inventory Weapon array. */
void ACGCharacter::PreviousWeapon()
{
	// Cache the number of weapons.
	const int32 NumberOfWeapons = Inventory->GetWeaponCount();
	int32 WeaponIndex = Inventory->GetWeaponIndex(CurrentWeapon);

	// Check for undeflow.
	if (NumberOfWeapons > 1)
	{
		WeaponIndex = (WeaponIndex - 1 + NumberOfWeapons) % NumberOfWeapons;
		EquipWeapon(Inventory->GetWeapon(WeaponIndex));
	}
}

// TODO make sure that if this gets interrupted the player will always return to their default zoom.
void ACGCharacter::StartZoom()
{
	if (CurrentWeapon && CurrentWeapon->CanZoom())
	{
		SetZoom(!bZoomed);
		CurrentZoom.BeginZoom(FOVDefault, bZoomed);
		bZooming = true;

		// Tell the server about the state change.
		if (Role < ROLE_Authority)
		{
			ServerSetZoom(bZoomed);
		}
	}
}

void ACGCharacter::StopZoom()
{
	// FIXME This is DuctTape for the zoom mechanic!
	// TODO replace with a state sytem.
	if (bZoomed && CurrentWeapon && CurrentWeapon->CanZoom())
	{
		SetZoom(!bZoomed);
		CurrentZoom.BeginZoom(FOVDefault, bZoomed);
		bZooming = true;

		// Tell the server about the state change.
		if (Role < ROLE_Authority)
		{
			ServerSetZoom(bZoomed);
		}
	}
}

void ACGCharacter::SetZoom(bool bZoom)
{
	bZoomed = bZoom;
}

void ACGCharacter::ServerSetZoom_Implementation(bool bZoom)
{
	SetZoom(bZoom);
}

bool ACGCharacter::ServerSetZoom_Validate(bool bZoom)
{
	return true;
}


void ACGCharacter::OnActionButton()
{
	// FIXME Might be more actions.
	if (PendingCrystalPickup && !CurrentWeapon->IsReloading())
	{
		// If we aren't the server, we better tell the server what we're doing.
		if (Role < ROLE_Authority)
		{
			ServerPickUpCrystal(); 
		}

		PickupCrystal();
	}
}

bool ACGCharacter::ServerPickUpCrystal_Validate()
{
	return true;
}

void ACGCharacter::ServerPickUpCrystal_Implementation()
{
	PickupCrystal();
}

void ACGCharacter::PickupCrystal()
{
	// Make sure the PendingCrystal is still there.
	if (PendingCrystalPickup)
	{
		// Cache the Crystal Type.
		ECGCrystalType CachedType = PendingCrystalPickup->GetCrystalType();
		// Pickup is valid.
		if (PendingCrystalPickup->Pickup())
		{
			// Load the crystal to the appropriate slot.
			Inventory->LoadCrystal(CachedType);
		}
	}
}

void ACGCharacter::OnPopCrystal()
{
	if (Inventory && Inventory->GetTierOneCrystal() != ECGCrystalType::NONE &&
		!CurrentWeapon->IsReloading())
	{
		// Make sure we control this player and it has an inventory.
		if (Role < ROLE_Authority)
		{
			ServerPopCrystal();
		}
		
		PopCrystal();		
	}
}

bool ACGCharacter::ServerPopCrystal_Validate()
{
	return true;
}

void ACGCharacter::ServerPopCrystal_Implementation()
{
	PopCrystal();
}

void ACGCharacter::PopCrystal()
{
	// Make sure the PendingCrystal is still there.
	// TODO work on the pop controls.
	if (Inventory)
	{
		Inventory->PopBestCrystal();
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
//	DOREPLIFETIME_CONDITION(ACGCharacter, Weapons, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ACGCharacter, PendingCrystalPickup, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ACGCharacter, Inventory, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ACGCharacter, bZoomed, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ACGCharacter, LastHit, COND_Custom);

	// Everyone.
	DOREPLIFETIME(ACGCharacter, CurrentWeapon);
	DOREPLIFETIME(ACGCharacter, CurrentHealth);
	DOREPLIFETIME(ACGCharacter, CurrentShield);

}

#pragma endregion