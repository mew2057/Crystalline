// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Crystalline.h"
#include "CrystallineHUD.h"
#include "Engine/Canvas.h"
#include "TextureResource.h"
#include "CanvasItem.h"

ACrystallineHUD::ACrystallineHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}


void ACrystallineHUD::DrawHUD()
{
	Super::DrawHUD();

	// Calculate the scale factor of the ui based on the clip space of the screen.
	// XXX We may be able to move this to an event response in the future (need to research if the clipspace change is bindable). -John
	ScaleUIY = Canvas->ClipY / TARGET_Y_RESOLUTION;
	//ScaleUIX = Canvas->ClipX / TARGET_X_RESOLUTION; XXX This may be needed in the future, we'll see how using only the Y for scaling works. -John


	// Find center of the Canvas. XXX Does this need to be a vector? -John
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	ACrystallinePlayer* Pawn = Cast<ACrystallinePlayer>(PlayerOwner->GetPawn());
	ACrystallineWeapon* Weapon = Pawn->GetCurrentWeapon();
	


	//////////////////////////
	// Crosshair	
	FCanvasIcon CrosshairIcon = Weapon->CrosshairIcon;
	// ScaleUI is 1 at 1080, .5 at 540 and 2 at 2160.
	// The UL and VL values indicate the width and length of the texture respectively.
	Canvas->DrawIcon(CrosshairIcon,
		(Center.X - (CrosshairIcon.UL * ScaleUIY * 0.5f)),
		(Center.Y - (CrosshairIcon.VL * ScaleUIY * 0.5f)), ScaleUIY); 
}


void ACrystallineHUD::DrawWeaponHUD(const ACrystallinePlayer& Pawn)
{
	// XXX Crystals should be present on the weapon sprites for the crystal gun.
	
	ACrystallineWeapon* CurrentWeapon = Pawn.GetCurrentWeapon();

	/** Get Primary weapon for the player. */
	if (CurrentWeapon)
	{
		/*
		FCanvasIcon CrosshairIcon = CurrentWeapon->CrosshairIcon;

		Canvas->DrawIcon(CrosshairIcon,
			(Center.X - (CrosshairIcon.UL * ScaleUIY * 0.5f) ),
			(Center.Y - (CrosshairIcon.VL * ScaleUIY * 0.5f) +  200), ScaleUIY);*/
	}


	/** Get the Secondary weapon for the player. */
	ACrystallineWeapon* SecondaryWeapon = Pawn.GetSecondaryWeapon();

	if (SecondaryWeapon)
	{
		// Output the icon.
		// NOTE: Don't include ammunition on this one.
		
	}
}

void ACrystallineHUD::DrawHealth(const ACrystallinePlayer& Pawn)
{

}

void ACrystallineHUD::DrawGameInfo(const ACrystallinePlayer& Pawn)
{

}
