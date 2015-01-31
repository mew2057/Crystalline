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

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);


	ACrystallinePlayer* Pawn = Cast<ACrystallinePlayer>(PlayerOwner->GetPawn());
	ACrystallineWeapon* Weapon = Pawn->GetCurrentWeapon();
	FCanvasIcon Crosshair = Weapon->Crosshair;

	// draw the crosshair
	Canvas->DrawIcon(Crosshair,
		(Center.X - (Crosshair.UL * 0.5)),
		(Center.Y - (Crosshair.VL * 0.5f)), 2);
}

