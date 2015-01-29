// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CrystallineWeapon.h"

ACrystallineWeapon::ACrystallineWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Initializes the weapon mesh.
	Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh1P"));
	Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh1P->bChartDistanceFactor = false; // prevents the mesh from being added to the gloabal chart
	Mesh1P->bReceivesDecals = false;      // Prevents decals from spawning on the gun.
	Mesh1P->CastShadow = false;           // Hides the shadow.
	Mesh1P->SetCollisionObjectType(ECC_WorldDynamic);			 //Sets the Collision channel of the gun.
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Ignores collisions.
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);       // No Collision response.
	RootComponent = Mesh1P;										 // Makes the first player mesh the root component.

	
	/** The default name of the Muzzle socket. */
	MuzzleSocket = TEXT("MuzzleFlashSocket");
}

void ACrystallineWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ACrystallineWeapon::StartFire()
{


}

void ACrystallineWeapon::StopFire()
{

}


