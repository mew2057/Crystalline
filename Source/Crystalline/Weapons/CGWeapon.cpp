// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "Weapons/States/CGWeaponState.h"
#include "Weapons/States/CGWeaponActiveState.h"
#include "Weapons/States/CGWeaponInactiveState.h"
#include "Weapons/States/CGWeaponEquippingState.h"
#include "Weapons/States/CGWeaponUnequippingState.h"
#include "Weapons/States/CGWeaponReloadingState.h"
#include "Weapons/States/CGWeaponFiringState.h"

#include "CGWeapon.h"

ACGWeapon::ACGWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Initializes the weapon mesh.
	Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh1P"));
	Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh1P->bChartDistanceFactor = false;                        // prevents the mesh from being added to the gloabal chart
	Mesh1P->bReceivesDecals = false;                             // Prevents decals from spawning on the gun.
	Mesh1P->CastShadow = false;                                  // Hides the shadow.
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->SetCollisionObjectType(ECC_WorldDynamic);			 //Sets the Collision channel of the gun.
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Ignores collisions.
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);       // No Collision response.
	Mesh1P->SetHiddenInGame(true);
	RootComponent = Mesh1P;										 // Makes the first player mesh the root component.

	// Initializes the weapon mesh.
	/*Mesh3P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh1P"));
	Mesh3P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh3P->bChartDistanceFactor = false;                        // prevents the mesh from being added to the gloabal chart
	Mesh3P->bReceivesDecals = false;                             // Prevents decals from spawning on the gun.
	Mesh3P->CastShadow = false;                                  // Hides the shadow.
	Mesh1P->bOnlyOwnerSee = false;
	Mesh1P->bOwnerNoSee = true;
	Mesh3P->SetCollisionObjectType(ECC_WorldDynamic);			 //Sets the Collision channel of the gun.
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Ignores collisions.
	Mesh3P->SetCollisionResponseToAllChannels(ECR_Ignore);       // No Collision response.
	Mesh3P->AttachParent = Mesh1P;
	*/


	ActiveState      = ObjectInitializer.CreateDefaultSubobject<UCGWeaponActiveState>(this, TEXT("StateActive"));
	InactiveState    = ObjectInitializer.CreateDefaultSubobject<UCGWeaponInactiveState>(this, TEXT("StateInactive"));
	EquippingState   = ObjectInitializer.CreateDefaultSubobject<UCGWeaponEquippingState>(this, TEXT("StateEquipping"));
	UnequippingState = ObjectInitializer.CreateDefaultSubobject<UCGWeaponUnequippingState>(this, TEXT("StateUnequipping"));
	ReloadingState   = ObjectInitializer.CreateDefaultSubobject<UCGWeaponReloadingState>(this, TEXT("StateReloading"));
	FiringState      = ObjectInitializer.CreateDefaultSubobject<UCGWeaponFiringState>(this, TEXT("StateFiring"));

	// NO! THIS IS BAD, LEFT IN SO YOU KNOW HOW TO KILL A SATURDAY!
	//	CurrentState = InactiveState;

	// Allows weapon to have a tick update (Necessary for some mechanics).
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bReplicateInstigator = true;
	bNetUseOwnerRelevancy = true;
}


void ACGWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	
}

void ACGWeapon::BeginPlay()
{
	Super::BeginPlay();

	// Ensure we're in a state.
	if (CurrentState == NULL)
	{
		GotoState(InactiveState);
	}
}


#pragma region Set Functions

void ACGWeapon::SetCGOwner(ACGCharacter* NewOwner)
{
	if (CGOwner != NewOwner)
	{
		CGOwner = NewOwner;
		SetOwner(CGOwner);
	}
}


#pragma endregion


////////////////////////////////////////////////////////////////////////////////////////

#pragma region State Functions

void ACGWeapon::OnEnterInventory(ACGCharacter* NewOwner)
{
	SetCGOwner(NewOwner);

	if (CurrentState == NULL)
	{
		GotoState(InactiveState);
	}
}

void ACGWeapon::OnExitInventory()
{
	// Make it inactive.
	GotoState(InactiveState);

	// If this has authorative control, remove null the pawn.
	if (Role == ROLE_Authority)
	{
		SetCGOwner(NULL);
	}
}

void ACGWeapon::OnEquip()
{
	// Timer and reload.
	AttachMeshToPawn();	

	// XXX this catches the starting issue wherein the weapon owners haven't been replicated yet.
	// XXX If anims don't play on start this is likely the source!
	if (CurrentState)
		CurrentState->StartEquip();

}

void ACGWeapon::OnUnequip()
{
	// XXX this catches the starting issue wherein the weapon owners haven't been replicated yet.
	// XXX If anims don't play on start this is likely the source!
	if (CurrentState)
		CurrentState->StartUnequip();
}



void ACGWeapon::StartFire()
{
	// Verify we can fire.


	if (Role < ROLE_Authority)
	{
		ServerStartFire();
	}

	StartFiring();
}

void ACGWeapon::StartFiring()
{
	CurrentState->StartFire();
}

bool ACGWeapon::ServerStartFire_Validate()
{
	return true;
}

void ACGWeapon::ServerStartFire_Implementation()
{
	StartFiring();
}

void ACGWeapon::StopFire()
{
	CurrentState->StopFire();
}

bool ACGWeapon::ServerStopFire_Validate()
{
	return true;
}

void ACGWeapon::ServerStopFire_Implementation()
{
	//
}

#pragma endregion


#pragma region State Management

void ACGWeapon::GotoState(UCGWeaponState* NewState)
{
	// Don't transition back into the same state.
	if (NewState != NULL && NewState->IsIn(this) && NewState != CurrentState)
	{
		UCGWeaponState* PrevState = CurrentState;
		if (CurrentState != NULL)
		{
			CurrentState->EndState();
		}

		// Ensure the states are the same.
		if (PrevState == CurrentState)
		{
			CurrentState = NewState;
			CurrentState->EnterState();
		}
	}
}

void ACGWeapon::GotoFiringState()
{
	// TODO Checks.
	GotoState(FiringState);
}


void ACGWeapon::GotoEquippingState()
{
	// TODO checks.
	GotoState(EquippingState);
}

void ACGWeapon::GotoUnequippingState()
{
	GotoState(UnequippingState);
}

#pragma endregion




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region Visuals

void ACGWeapon::AttachMeshToPawn()
{
	if (CGOwner)
	{
		DetachMeshFromPawn();

		const FName ConnectionPoint = CGOwner->GetWeaponAttachPoint();

		// This lets the flags for the mesh handle everything.
		USkeletalMeshComponent * PawnMesh1P = CGOwner->GetMesh1P();
		Mesh1P->SetHiddenInGame(false);
		Mesh1P->AttachTo(PawnMesh1P, ConnectionPoint, EAttachLocation::SnapToTarget);

		// TODO attach the 3rd person weapon.
		// FIXME At Some point get the locally controlled gate workng.
	}
}

void ACGWeapon::DetachMeshFromPawn()
{
	Mesh1P->DetachFromParent();
	Mesh1P->SetHiddenInGame(true);
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region Replication

void ACGWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACGWeapon, CGOwner);
	//DOREPLIFETIME(ACGWeapon, CurrentState);
}

void ACGWeapon::OnRep_CGOwner()
{
	if (CGOwner)
	{
		OnEnterInventory(CGOwner);
	}
	else
	{
		OnExitInventory();
	}
}



#pragma endregion