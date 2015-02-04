// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CrystallinePistol.h"

ACrystallinePistol::ACrystallinePistol(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsOverheated = false;
	bIsCoolingDown = false;
	WeaponHeat    = 0.f;
}

void ACrystallinePistol::FireWeapon()
{
	// XXX THIS CODE IS DIIIIRTY!
	// TODO clean this and compartmentalize it to functions.
	// Not sure if this will work.
	ACrystallinePlayer* Pawn = Cast<ACrystallinePlayer>(OwningPawn);

	FVector StartTrace;
	FRotator CamRot;
	Pawn->Controller->GetPlayerViewPoint(StartTrace, CamRot);

	//FVector StartTrace2 = StartTrace + CamRot.Vector() * ((OwningPawn->GetActorLocation() - StartTrace) | CamRot.Vector());
	// TODO remove magic number.
	FVector EndTrace = StartTrace + CamRot.Vector() * 10000.0f;

	// Trace Parameters, these should be relatively static, so maybe declare once per weapon? -John

	const FName TraceTag("MyTraceTag");

	FCollisionQueryParams TraceParams = FCollisionQueryParams(TraceTag, true, OwningPawn);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);

	// ECC_GameTraceChannel1 needs to be replaced with something that fits.
	GetWorld()->LineTraceSingle(Hit, StartTrace, EndTrace, ECC_GameTraceChannel1, TraceParams);

	// This needs to be more robust for multi mesh solutions.
	FVector Origin = Mesh1P->GetSocketLocation(MuzzleSocket);
	FVector Direction = CamRot.Vector();

	if (Hit.bBlockingHit)
	{
		Direction = (Hit.ImpactPoint - Origin).SafeNormal();
	}

	ServerFireProjectile(Origin, Direction);
}

void ACrystallinePistol::UseAmmo()
{
	WeaponHeat = FMath::Min(WeaponHeat + ProjectileConfig.HeatPerShot, ProjectileConfig.MaxHeat);

	// May not need the overheated thing, but this is me trying to be sure we don't get too many timers running.
	if (WeaponHeat == ProjectileConfig.MaxHeat && !bIsOverheated)
	{

		bIsOverheated  = true;
		bIsCoolingDown = false;


		GetWorldTimerManager().SetTimer(this, 
			&ACrystallinePistol::HandleOverheatCooldown, 
			ProjectileConfig.OverheatTime, false);
	}
}

void ACrystallinePistol::HandleOverheatCooldown()
{
	bIsOverheated = false; 
	bIsCoolingDown = true;

	GetWorldTimerManager().SetTimer(this,
		&ACrystallinePistol::FinishCooldown,
		ProjectileConfig.MaxHeat/ProjectileConfig.CooldownPerSecond, false);
}


void ACrystallinePistol::FinishCooldown()
{
	bIsCoolingDown = false;
}


void ACrystallinePistol::Tick(float DeltaSeconds)
{
	if (!bIsOverheated)
		WeaponHeat = FMath::Max(0.f,WeaponHeat - (ProjectileConfig.CooldownPerSecond * DeltaSeconds));
}


bool ACrystallinePistol::CanFire()
{
	return !(bIsOverheated || bIsCoolingDown);
}

bool ACrystallinePistol::ServerFireProjectile_Validate(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
	return true;
}


void ACrystallinePistol::ServerFireProjectile_Implementation(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
	// Determine the spawn point and create a bullet to fire.
	FTransform BulletSpawn(ShootDir.Rotation(), Origin);
	ACrystallineProjectile* Bullet = Cast<ACrystallineProjectile>(
		UGameplayStatics::BeginSpawningActorFromClass(this, ProjectileConfig.ProjectileClass, BulletSpawn));

	if (Bullet)
	{
		Bullet->Instigator = Instigator;		
		Bullet->SetOwner(this);
		Bullet->SetVelocity(ShootDir); // This ensures the behavior matches it's intended use case.
		UGameplayStatics::FinishSpawningActor(Bullet, BulletSpawn);
	}
}



