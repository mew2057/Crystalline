// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CrystallinePistol.h"

void ACrystallinePistol::FireWeapon()
{

	// TODO clean this and compartmentalize it to functions.
	// Not sure if this will work.
	ACrystallinePlayer* Pawn = Cast<ACrystallinePlayer>(OwningPawn);

	FVector StartTrace;
	FRotator CamRot;
	Pawn->Controller->GetPlayerViewPoint(StartTrace, CamRot);


	// TODO remove magic number.
	FVector EndTrace = StartTrace + CamRot.Vector() * 10000.0f;

	// Trace Parameters, these should be relatively static, so maybe declare once per weapon? -John

	const FName TraceTag("MyTraceTag");

	FCollisionQueryParams TraceParams = FCollisionQueryParams(TraceTag, true, this);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);

	// ECC_GameTraceChannel1 needs to be replaced with something that fits.
	GetWorld()->LineTraceSingle(Hit, StartTrace, EndTrace, ECC_GameTraceChannel1, TraceParams);

	// XXX this is blocking out how the projectile code will work.
#pragma region Projectile

	// This needs to be more robust for multi mesh solutions.
	FVector Origin = Mesh1P->GetSocketLocation(MuzzleSocket);
	FVector Direction = CamRot.Vector();

	if (Hit.bBlockingHit)
	{
		Direction = (Hit.ImpactPoint - Origin).SafeNormal();

	} 
	

	// Determine the spawn point and create a bullet to fire.
	FTransform BulletSpawn(Direction.Rotation(), Origin);

	ACrystallineProjectile* Bullet = Cast<ACrystallineProjectile>(
		UGameplayStatics::BeginSpawningActorFromClass(this, ProjectileConfig.ProjectileClass, BulletSpawn));
		
	if (Bullet)
	{
		Bullet->SetVelocity(Direction);
		Bullet->Instigator = Instigator;

		UGameplayStatics::FinishSpawningActor(Bullet, BulletSpawn);
	}

#pragma endregion








}