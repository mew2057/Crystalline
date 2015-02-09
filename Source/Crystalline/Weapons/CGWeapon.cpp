// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "Weapons/States/CGWeaponState.h"
#include "Weapons/States/CGWeaponActiveState.h"
#include "Weapons/States/CGWeaponInactiveState.h"
#include "Weapons/States/CGWeaponEquippingState.h"
#include "Weapons/States/CGWeaponUnequippingState.h"
#include "Weapons/States/CGWeaponReloadingState.h"
#include "Weapons/States/CGWeaponFiringState.h"
#include "Projectiles/CGProjectile.h"


#include "CGWeapon.h"

ACGWeapon::ACGWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Initializes the weapon mesh.
	Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh1P"));
	Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh1P->bChartDistanceFactor = false;                        // prevents the mesh from being added to the gloabal chart
	Mesh1P->bReceivesDecals = false;                             // Prevents decals from spawning on the gun.
	Mesh1P->CastShadow = false;                                  // Hides the shadow.
	Mesh1P->bOnlyOwnerSee = false;
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

	BurstCount    = 0;
	LastFireTime  = 0.f;

	HitScanConfig.MaxSpread     = FMath::DegreesToRadians(HitScanConfig.MaxSpread * 0.5f);
	HitScanConfig.BaseSpread    = FMath::DegreesToRadians(HitScanConfig.BaseSpread * 0.5f);
	CurrentSpread				= HitScanConfig.BaseSpread;
	HitScanConfig.SpreadPerShot = FMath::DegreesToRadians(HitScanConfig.SpreadPerShot * 0.5f);
}


void ACGWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CurrentState == NULL)
		GotoState(InactiveState);
	
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
		CGOwner    = NewOwner;
		Instigator = NewOwner;

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
	CurrentState->StartEquip();


}

void ACGWeapon::OnUnequip()
{
	// XXX this catches the starting issue wherein the weapon owners haven't been replicated yet.
	// XXX If anims don't play on start this is likely the source!
	CurrentState->StartUnequip();
}



void ACGWeapon::StartFire()
{
	// Verify we can fire.

	// Tell the server to start firing.
	if (Role < ROLE_Authority)
	{
		ServerStartFire();
	}

	UE_LOG(LogTemp, Log, TEXT("CurrentState: %s"), *CurrentState->GetName());

	// Begin firing locally.
	CurrentState->StartFire();
}

bool ACGWeapon::ServerStartFire_Validate()
{
	return true;
}

void ACGWeapon::ServerStartFire_Implementation()
{
	CurrentState->StartFire();
}

void ACGWeapon::StopFire()
{
	if (Role < ROLE_Authority)
	{
		ServerStopFire();
	}

	// TODO make a function?
	CurrentState->StopFire();

}

bool ACGWeapon::ServerStopFire_Validate()
{
	return true;
}

void ACGWeapon::ServerStopFire_Implementation()
{
	CurrentState->StopFire();
}


void ACGWeapon::StartFiring()
{
	// XXX Must be able to fire.

	// Don't fire if the netmode is dedicated server.
	if (GetNetMode() != NM_DedicatedServer)
	{
		StartWeaponFireSimulation();
	}

	if (CGOwner && CGOwner->IsLocallyControlled())
	{
		if (WeaponConfig.bUsesProjectile)
		{
			FireProjectile();
		}
		else
		{
			FireHitScan();
		}
		UseAmmo();
	}

	// Triggers the OnRep
	BurstCount++;

	LastFireTime = GetWorld()->GetTimeSeconds();
}

void ACGWeapon::StopFiring()
{	
	// Clean up
	StopWeaponFireSimulation();
}

void ACGWeapon::StartWeaponFireSimulation()
{  
	// The sound effect.
	if (WeaponFXConfig.FireSound)
	{
		FireAudioComponent = UGameplayStatics::PlaySoundAttached(WeaponFXConfig.FireSound, GetRootComponent());
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

	APlayerController* CGController = Cast<APlayerController>(CGOwner->GetController());

	if (CGController != NULL && CGController->IsLocalController())
	{
		if (WeaponFXConfig.CameraShake != NULL)
		{
			CGController->ClientPlayCameraShake(WeaponFXConfig.CameraShake, 1);
		}
	}
	
}

void ACGWeapon::StopWeaponFireSimulation()
{ 
	BurstCount = 0;
	/*
	// Clean up the components.
	if (MuzzleFlashComp != NULL)
	{
		MuzzleFlashComp->DeactivateSystem();
		MuzzleFlashComp = NULL;
	}*/

	// TODO Configure this more when we actually have audio.
	/*
	if (FireAudioComponent)
	{
		FireAudioComponent->FadeOut(0.1f, 0.0f);
		FireAudioComponent = NULL;
	}*/
}

#pragma region Projectile Fire

void ACGWeapon::FireProjectile()
{
	const FVector StartTrace = GetCameraLocation();
	const FVector AimDir     = GetCameraAim();
	const FVector EndTrace   = StartTrace + AimDir * WeaponConfig.WeaponRange;
	FHitResult Impact        = WeaponTrace(StartTrace, EndTrace);

	// This needs to be more robust for multi mesh solutions.
	FVector Origin = GetMuzzleLocation();
	FVector Direction = AimDir;

	if (Impact.bBlockingHit)
	{
		FVector MuzzleDir = (Impact.ImpactPoint - Origin).SafeNormal();

		bool bIntersecting = false;
		float GunDot = FVector::DotProduct(MuzzleDir, Direction);

		// If it's less than zero we're penetrating.
		if (GunDot < 0.0f)
		{
			bIntersecting = true;
		}
		else if (GunDot < 0.5f) // If there's an angle there's a chance of penetration
		{
			// TOOD can shoot through walls in doorways.
			// XXX the 150 may change.
			FVector WeaponTraceStart = Origin - GetMuzzleRotation() * 200.f;

			bIntersecting = WeaponTrace(WeaponTraceStart, Origin).bBlockingHit;

			// Basically the check finds the gun intersection.
			//				____________	
			//				|/
			//			    / (Gun)
			//	(Gun Inter)*|
			//				|
			//				|
		}

		if (bIntersecting)
		{
			Origin = Impact.ImpactPoint;
		}
		else
		{
			Direction = MuzzleDir;
		}
	}
	
	// Server should only spawn the projectile.
	ServerFireProjectile(Origin, Direction);
}

bool ACGWeapon::ServerFireProjectile_Validate(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
	return true;
}

void ACGWeapon::ServerFireProjectile_Implementation(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
	// Determine the spawn point and create a bullet to fire.
	FTransform BulletSpawn(ShootDir.Rotation(), Origin);
	ACGProjectile* Bullet = Cast<ACGProjectile>(
		UGameplayStatics::BeginSpawningActorFromClass(this, ProjectileConfig.ProjectileClass, BulletSpawn));

	if (Bullet)
	{
		Bullet->Instigator = Instigator;
		Bullet->SetOwner(this);
		Bullet->SetVelocity(ShootDir); // This ensures the behavior matches it's intended use case.
		Bullet->ImpactDamage = WeaponConfig.BaseDamage;
		UGameplayStatics::FinishSpawningActor(Bullet, BulletSpawn);
	}
}

#pragma endregion

#pragma region HitScan
void ACGWeapon::FireHitScan()
{
	// My old nemesis
	int32 FireSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(FireSeed);

	// Perform a raycast from the crosshair in to the world space.
	// Get the starting location and rotation for the player.
	const FVector StartTrace = GetCameraLocation();
	const FVector AimDir = GetCameraAim();

	// Adds variation to the bullet.
	FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, CurrentSpread);

	// Specify the end point for the weapon's fire.
	FVector EndTrace = StartTrace + AimDir * WeaponConfig.WeaponRange;

	// Get the Impact for the weapon trace then confirm whether or not it hit a player.
	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	SpawnTrailEffect(Impact.ImpactPoint);
	SpawnHitEffect(Impact);

	// Call Server

	CurrentSpread = FMath::Min(HitScanConfig.MaxSpread, CurrentSpread + HitScanConfig.SpreadPerShot);

}


void ACGWeapon::SpawnTrailEffect(const FVector& EndPoint)
{
	if (WeaponFXConfig.WeaponTrail)
	{
		UParticleSystemComponent* TrailPSC = UGameplayStatics::SpawnEmitterAtLocation(
			this,
			WeaponFXConfig.WeaponTrail,
			GetMuzzleLocation());

		if (TrailPSC)
		{
			// Set the vector for the particle.
			TrailPSC->SetVectorParameter(WeaponFXConfig.TrailTargetParam, EndPoint);
		}
	}
}

// TODO
void ACGWeapon::SpawnHitEffect(const FHitResult& Impact)
{
}

#pragma endregion

void ACGWeapon::UseAmmo()
{

}


void ACGWeapon::OnRep_BurstCount()
{
	if (BurstCount > 0)
	{
		StartWeaponFireSimulation();
	}
	else
	{
		StopWeaponFireSimulation();
	}
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
	// TODO Checks, if ammo < AmmoPerShot, enter reload state.
	GotoState(FiringState);
}


void ACGWeapon::GotoEquippingState()
{
	// TODO checks.
	GotoState(EquippingState);
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

#pragma region TransformHelpers

FVector ACGWeapon::GetCameraAim() const
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

FVector ACGWeapon::GetCameraLocation() const
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

FVector ACGWeapon::GetMuzzleLocation() const
{
	// TODO Mesh for not locally controlled.

	return Mesh1P->GetSocketLocation(WeaponFXConfig.MuzzleSocket);
}

FVector ACGWeapon::GetMuzzleRotation() const
{
	// TODO Mesh for not locally controlled.
	return Mesh1P->GetSocketRotation(WeaponFXConfig.MuzzleSocket).Vector();
}


FHitResult ACGWeapon::WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const
{
	FCollisionQueryParams TraceParams = FCollisionQueryParams(WEAPON_TRACE_TAG, true, CGOwner);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingle(Hit, TraceFrom, TraceTo, COLLISION_WEAPON, TraceParams);

	return Hit;
}
#pragma endregion

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region Replication

void ACGWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ACGWeapon, BurstCount, COND_SkipOwner);

	DOREPLIFETIME(ACGWeapon, CGOwner);
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