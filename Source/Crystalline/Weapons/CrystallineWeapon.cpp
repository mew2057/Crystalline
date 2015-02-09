// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CrystallineWeapon.h"

ACrystallineWeapon::ACrystallineWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Initializes the weapon mesh.
	Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh1P"));
	Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh1P->bChartDistanceFactor = false;                        // prevents the mesh from being added to the gloabal chart
	Mesh1P->bReceivesDecals = false;                             // Prevents decals from spawning on the gun.
	Mesh1P->CastShadow = false;                                  // Hides the shadow.
	Mesh1P->SetCollisionObjectType(ECC_WorldDynamic);			 //Sets the Collision channel of the gun.
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Ignores collisions.
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);       // No Collision response.
	Mesh1P->bOwnerNoSee = false;     // No Collision response.
	Mesh1P->bOnlyOwnerSee = true;

	RootComponent = Mesh1P;										 // Makes the first player mesh the root component.

	// Allows weapon to have a tick update (Necessary for some mechanics).
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bReplicateInstigator = true;
	bNetUseOwnerRelevancy = true;	

	LastFireTime = 0.0f;
	BurstCounter = 0;
}


void ACrystallineWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// Remove the pawn when everything is set up (hides the mesh);
	DetachMeshFromPawn();

	// If the ammo guage width is not set, then set it to the width of the gauge icon.
	if (WeaponHUDConfig.AmmoGuageWidth == 0.f)
	{
		WeaponHUDConfig.AmmoGuageWidth = WeaponHUDConfig.AmmoGuageFGIcon.UL;
	}
	//////////////////////////////////
	// Initialize the spread factor.

	// Halve the spread and convert to radians for the random cone.
	WeaponConfig.HSpreadMax       = FMath::DegreesToRadians(WeaponConfig.HSpreadMax * 0.5f);
	WeaponConfig.HSpreadBase      = FMath::DegreesToRadians(WeaponConfig.HSpreadBase * 0.5f);
	HSpreadCurrent				  = WeaponConfig.HSpreadBase;
	WeaponConfig.HSpreadIncrement = FMath::DegreesToRadians(WeaponConfig.HSpreadIncrement * 0.5f);

	WeaponConfig.VSpreadMax       = FMath::DegreesToRadians(WeaponConfig.VSpreadMax * 0.5f);
	WeaponConfig.VSpreadBase      = FMath::DegreesToRadians(WeaponConfig.VSpreadBase * 0.5f);
	VSpreadCurrent				 = WeaponConfig.VSpreadBase;
	WeaponConfig.VSpreadIncrement = FMath::DegreesToRadians(WeaponConfig.VSpreadIncrement * 0.5f);
}


void ACrystallineWeapon::StartFire()
{	
	if (Role < ROLE_Authority)
	{
		ServerStartFire();
	}

	// Firing is set to be true.
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		// TODO state update
		StartBurst();
	}
}

void ACrystallineWeapon::StopFire()
{
	bWantsToFire = false;
	StopBurst();
}

void ACrystallineWeapon::HandleFire()
{
	if (!CanFire())
	{
		return;
	}

	if (GetNetMode() != NM_DedicatedServer)
	{
		SimulateWeaponFire();
	}


	// TODO AmmoCheck
	// Fire if there's ammo
	if (OwningPawn && OwningPawn->IsLocallyControlled())
	{
		FireWeapon();
		UseAmmo();
		BurstCounter++;
	}


	if (OwningPawn && OwningPawn->IsLocallyControlled())
	{
		if (Role < ROLE_Authority)
		{
			ServerHandleFire();
		}
		// Trigger a reload if needed.

		// If this is an automatic weapon fire continue firing in TimeBetweenShots seconds.
		if (WeaponConfig.bAutomaticFire)
		{
			GetWorldTimerManager().SetTimer(this, &ACrystallineWeapon::HandleFire, WeaponConfig.TimeBetweenShots, false);
		}		
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

void ACrystallineWeapon::SimulateWeaponFire()
{
	// EARLY RETURN: The Server shouldn't play these effects, if it isn't firing (You misread this one john).
	/*if(Role == ROLE_Authority )
	{
		return;
	}
	*/

	// The sound effect.
	if (WeaponFXConfig.FireSound)
	{
		FireAudioComponent=	UGameplayStatics::PlaySoundAttached(WeaponFXConfig.FireSound, GetRootComponent());
	}

	// Muzzle Flash
	if (WeaponFXConfig.MuzzleFlash)
	{
		const FVector FlashPoint = Mesh1P->GetSocketLocation(WeaponFXConfig.MuzzleSocket);

		MuzzleFlashComp = UGameplayStatics::SpawnEmitterAttached(WeaponFXConfig.MuzzleFlash, Mesh1P, WeaponFXConfig.MuzzleSocket);
		// TODO CONFIGURE THIS!		
		MuzzleFlashComp->bOwnerNoSee = false;
		MuzzleFlashComp->bOnlyOwnerSee = false;		
	}

}

void ACrystallineWeapon::StopWeaponFireSimulation()
{

	// Clean up the components.
	if (MuzzleFlashComp != NULL)
	{
		MuzzleFlashComp->DeactivateSystem();
		MuzzleFlashComp = NULL;
	}

	// TODO Configure this more when we actually have audio.
	
	if (FireAudioComponent)
	{
		FireAudioComponent->FadeOut(0.1f, 0.0f);
		FireAudioComponent = NULL;
	}
}



void ACrystallineWeapon::StartBurst()
{
	float TimeSinceShot = GetWorld()->GetTimeSeconds() - LastFireTime;

	if (LastFireTime > 0.f && WeaponConfig.TimeBetweenShots > 0.f && TimeSinceShot < WeaponConfig.TimeBetweenShots)
	{
		GetWorldTimerManager().SetTimer(this, &ACrystallineWeapon::HandleFire, TimeSinceShot + WeaponConfig.TimeBetweenShots, false);
	}
	else
	{
		HandleFire();
	}
}

void ACrystallineWeapon::StopBurst()
{

	// This will stop FX on remote clients.
	BurstCounter = 0;

	GetWorldTimerManager().ClearTimer(this, &ACrystallineWeapon::HandleFire);

	// This will stop weapon effects locally.
	if (GetNetMode() != NM_DedicatedServer)
	{
		StopWeaponFireSimulation();
	}

	// Reset the spread to the baseline.
	HSpreadCurrent = WeaponConfig.HSpreadBase;
	VSpreadCurrent = WeaponConfig.VSpreadBase;
}

bool ACrystallineWeapon::CanFire()
{
	return true;
}

#pragma region Inventory_Related

void ACrystallineWeapon::OnEnterInventory(ACrystallinePlayer* NewOwner)
{
	SetOwningPawn(NewOwner);
}

void ACrystallineWeapon::OnExitInventory()
{
	// If this has authorative control, remove null the pawn.
	if (Role == ROLE_Authority)
	{
		SetOwningPawn(NULL);
	}
	//TODO Remove attachment.
	
}

void ACrystallineWeapon::OnEquip()
{
	// Timer and reload.
	AttachMeshToPawn();
	// TODO add switch animation id appropriate.
}

void ACrystallineWeapon::OnUnEquip()
{
	DetachMeshFromPawn();
}


void ACrystallineWeapon::SetOwningPawn(ACrystallinePlayer* Owner)
{
	if (OwningPawn != Owner)
	{
		OwningPawn = Owner;
		Instigator = Owner;

		// Apparently RPC calls require this.
		SetOwner(Owner);
	}
}

#pragma endregion

#pragma region ServerFunctions

/////////////////////////
// Fire Operations
void ACrystallineWeapon::ServerStartFire_Implementation()
{
	StartFire();
}

bool ACrystallineWeapon::ServerStartFire_Validate()
{
	return true;
}

void ACrystallineWeapon::ServerStopFire_Implementation()
{

}

bool ACrystallineWeapon::ServerStopFire_Validate()
{
	return true;
}



void ACrystallineWeapon::ServerHandleFire_Implementation()
{
	HandleFire();
// Ammo Update?

	BurstCounter++;
}

bool ACrystallineWeapon::ServerHandleFire_Validate()
{
	return true;
}



/////////////////////////
// Reload Operations
void ACrystallineWeapon::ServerStartReload_Implementation()
{

}
bool ACrystallineWeapon::ServerStartReload_Validate()
{
	return true;
}


void ACrystallineWeapon::ServerStopReload_Implementation()
{

}


bool ACrystallineWeapon::ServerStopReload_Validate()
{
	return true;
}

#pragma endregion

#pragma  region MeshOperations

void ACrystallineWeapon::AttachMeshToPawn()
{
	if (OwningPawn)
	{
		// Make sure the mesh isn't parented.
		DetachMeshFromPawn();

		FName ConnectionPoint = OwningPawn->GetWeaponAttachPoint();

		USkeletalMeshComponent * PawnMesh1P = OwningPawn->GetMesh1P();
		Mesh1P->SetHiddenInGame(false);

		// NOTE: Without the snapToTarget this has some transform issues.
		// XXX Why is this necessary?
		Mesh1P->AttachTo(PawnMesh1P, ConnectionPoint, EAttachLocation::SnapToTarget);
		//TODO something different for local and remote.
	}
}


void ACrystallineWeapon::DetachMeshFromPawn()
{
	// Remove mesh from the parent.
	Mesh1P->DetachFromParent();
	// Hide the mesh in the scene.
	Mesh1P->SetHiddenInGame(true);
}

#pragma endregion

#pragma region Replication

void ACrystallineWeapon::OnRep_OwningPawn()
{
	if (OwningPawn)
	{
		OnEnterInventory(OwningPawn);
	}
	else
	{
		OnExitInventory();
	}
}


void ACrystallineWeapon::OnRep_BurstCounter()
{
	if (BurstCounter > 0)
	{
		SimulateWeaponFire();
	}
	else
	{
		StopWeaponFireSimulation();
	}

}


void ACrystallineWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Owner doesn't need the replication.
	DOREPLIFETIME_CONDITION(ACrystallineWeapon, BurstCounter, COND_SkipOwner);

	// Replicated across all players.
	DOREPLIFETIME(ACrystallineWeapon, OwningPawn);

}
#pragma endregion

float ACrystallineWeapon::GetClipPercent() const
{
	return 0.0f;
}

FHitResult ACrystallineWeapon::WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const
{
	FCollisionQueryParams TraceParams = FCollisionQueryParams(WEAPON_TRACE_TAG, true, OwningPawn);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingle(Hit, TraceFrom, TraceTo, COLLISION_WEAPON, TraceParams);

	return Hit;
}

////////////////////////////////////////////////////////
// Weapon Transform Helpers

#pragma region TransformHelpers

FVector ACrystallineWeapon::GetCameraAim() const
{
	// Zero the aim  in case of failure.
	FVector Aim = FVector::ZeroVector;
	AController* Controller = Instigator ? Instigator->Controller : NULL;
	if (Controller)
	{
		FVector  TempOrigin;
		FRotator TempRotator;
		Controller->GetPlayerViewPoint(TempOrigin, TempRotator);

		Aim = TempRotator.Vector();
	}

	return Aim;
}

FVector ACrystallineWeapon::GetCameraLocation() const
{
	// Zero the origin in case of failure.
	FVector Origin = FVector::ZeroVector;
	AController* Controller = Instigator ? Instigator->Controller : NULL;
	if (Controller)
	{
		FRotator TempRotator;
		Controller->GetPlayerViewPoint(Origin, TempRotator);
	}

	return Origin;
}

FVector ACrystallineWeapon::GetMuzzleLocation() const
{
	// TODO Mesh for not locally controlled.

	return Mesh1P->GetSocketLocation(WeaponFXConfig.MuzzleSocket);
}

FVector ACrystallineWeapon::GetMuzzleRotation() const
{
	// TODO Mesh for not locally controlled.
	return Mesh1P->GetSocketRotation(WeaponFXConfig.MuzzleSocket).Vector();
}

#pragma endregion


////////////////////////////////////////////////////////
// Replication
