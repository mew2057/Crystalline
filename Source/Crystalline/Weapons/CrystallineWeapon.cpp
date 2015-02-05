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
	RootComponent = Mesh1P;										 // Makes the first player mesh the root component.
	LastFireTime = 0.0f;

	/** The default name of the Muzzle socket. */
	MuzzleSocket = TEXT("MuzzleFlashSocket");

	// Allows weapon to have a tick update (Necessary for some mechanics).
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bReplicateInstigator = true;
	bNetUseOwnerRelevancy = true;

	// Default colors for the ammo readout.
	LowAmmoColor = FLinearColor(1, 0, 0, 1);
	FullAmmoColor = FLinearColor(0, 1, 0, 1);
	WeaponRange = 10000.0f;
}


void ACrystallineWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// Remove the pawn when everything is set up (hides the mesh);
	DetachMeshFromPawn();

	// If the ammo guage width is not set, then set it to the width of the gauge icon.
	if (AmmoGuageWidth == 0.f)
	{
		AmmoGuageWidth = AmmoGuageFGIcon.UL;
	}
	//////////////////////////////////
	// Initialize the spread factor.

	// Halve the spread and convert to radians for the random cone.
	HSpreadMax       = FMath::DegreesToRadians(HSpreadMax * 0.5f);
	HSpreadBase      = FMath::DegreesToRadians(HSpreadBase * 0.5f);
	HSpreadCurrent   = HSpreadBase;
	HSpreadIncrement = FMath::DegreesToRadians(HSpreadIncrement * 0.5f);

	VSpreadMax       = FMath::DegreesToRadians(VSpreadMax * 0.5f);
	VSpreadBase      = FMath::DegreesToRadians(VSpreadBase * 0.5f);
	VSpreadCurrent   = VSpreadBase;
	VSpreadIncrement = FMath::DegreesToRadians(VSpreadIncrement * 0.5f);
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

	// IFF we can fire.
	SimulateWeaponFire();


	// TODO AmmoCheck
	// Fire if there's ammo
	if (OwningPawn && OwningPawn->IsLocallyControlled())
	{
		FireWeapon();
		UseAmmo();
	}


	if (OwningPawn && OwningPawn->IsLocallyControlled())
	{
		if (Role < ROLE_Authority)
		{
			ServerHandleFire();
		}
		// Trigger a reload if needed.

		// If this is an automatic weapon fire continue firing in TimeBetweenShots seconds.
		if (bAutomaticFire)
		{
			GetWorldTimerManager().SetTimer(this, &ACrystallineWeapon::HandleFire, TimeBetweenShots, false);
		}

		
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

void ACrystallineWeapon::SimulateWeaponFire()
{
	// EARLY RETURN: The Server shouldn't play these effects.
	if (Role == ROLE_Authority )
	{
		return;
	}


	// The sound effect.
	if (FireSound )
	{
		//FireAudioComponent=
		UGameplayStatics::PlaySoundAttached(FireSound, GetRootComponent());
	}

	// Muzzle Flash
	if (MuzzleFlash)
	{
		const FVector FlashPoint = Mesh1P->GetSocketLocation(MuzzleSocket);

		MuzzleFlashComp = UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh1P, MuzzleSocket);
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

	if (FireAudioComponent)
	{
		FireAudioComponent->FadeOut(0.1f, 0.0f);
		FireAudioComponent = NULL;
	}
}



void ACrystallineWeapon::StartBurst()
{
	float TimeSinceShot = GetWorld()->GetTimeSeconds() - LastFireTime;

	if (LastFireTime > 0.f && TimeBetweenShots > 0.f && TimeSinceShot < TimeBetweenShots)
	{
		GetWorldTimerManager().SetTimer(this, &ACrystallineWeapon::HandleFire, TimeSinceShot + TimeBetweenShots, false);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Burst"));

		HandleFire();
	}
}

void ACrystallineWeapon::StopBurst()
{
	GetWorldTimerManager().ClearTimer(this, &ACrystallineWeapon::HandleFire);
	// Cancel weapon simulations.

	// Since the server already doesn't simluate weapon visuals they don't need to Stop them.
	// XXX is the ROLE check appropriate here? -John
	// FIXME this is still not quite ready.
	//if (GetNetMode() != NM_DedicatedServer)
	//{
	//	StopWeaponFireSimulation();
	//}

	// Reset the spread to the baseline.
	HSpreadCurrent = HSpreadBase;
	VSpreadCurrent = VSpreadBase;
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

void ACrystallineWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated across all players.
	DOREPLIFETIME(ACrystallineWeapon, OwningPawn);

}

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

