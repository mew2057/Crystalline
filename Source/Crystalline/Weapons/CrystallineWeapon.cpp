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

	
	/** The default name of the Muzzle socket. */
	MuzzleSocket = TEXT("MuzzleFlashSocket");

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bReplicateInstigator = true;
	bNetUseOwnerRelevancy = true;
}

void ACrystallineWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ACrystallineWeapon::StartFire()
{
	
	if (Role < ROLE_Authority)
	{
		ServerStartFire();
	}


	// Firing is set to be true.
	bWantsToFire = true;
	HandleFire();

}

void ACrystallineWeapon::HandleFire()
{
	if (!CanFire())
	{
		return;
	}

	// The sound effect.
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAttached(FireSound, GetRootComponent());
	}
	
	// Muzzle Flash
	if (MuzzleFlash)
	{
		const FVector FlashPoint = Mesh1P->GetSocketLocation(MuzzleSocket);

		MuzzleFlashComp = UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh1P, MuzzleSocket);
		//// TODO CONFIGURE THIS!
		//
		//MuzzleFlashComp->bOwnerNoSee = false;
		//MuzzleFlashComp->bOnlyOwnerSee = true;
		//
	}

	FireWeapon();
	UseAmmo();
}

bool ACrystallineWeapon::CanFire()
{
	return true;
}


void ACrystallineWeapon::StopFire()
{
	bWantsToFire = false;
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
	OwningPawn = Owner;
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
		Mesh1P->AttachTo(PawnMesh1P, ConnectionPoint);

		//TODO something different for local and remote.

		UE_LOG(LogTemp, Log, TEXT("attach happened."));

	}

	
	//Mesh1P->AttachTo(PawnMesh1p, AttachPoint);
}

void ACrystallineWeapon::DetachMeshFromPawn()
{
	// Remove mesh from the parent.
	Mesh1P->DetachFromParent();
	// Hide the mesh in the scene.
	Mesh1P->SetHiddenInGame(false);

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


