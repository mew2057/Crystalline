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

	// Draw very simple crosshair

	// find center of the Canvas. XXX Does this need to be a vector? -John
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);


	ACrystallinePlayer* Pawn = Cast<ACrystallinePlayer>(PlayerOwner->GetPawn());
	ACrystallineWeapon* Weapon = Pawn->GetCurrentWeapon();
	

	//////////////////////////
	// Crosshair
	UTexture2D* Crosshair = Weapon->Crosshair;
	const FVector2D CrosshairDrawPosition(
		(Center.X - (Crosshair->GetSurfaceWidth() * 0.5f)),
		(Center.Y - (Crosshair->GetSurfaceHeight() * 0.5f)));

	FCanvasTileItem CrosshairTile(CrosshairDrawPosition, Crosshair->Resource, FLinearColor::White);
	CrosshairTile.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(CrosshairTile);

	/** The Icon UVs don't transfer to the C++ properly? FIXME
	// ScaleUI is 1 at 1080, .5 at 540 and 2 at 2160.
	Canvas->DrawIcon(Crosshair, 
		(Center.X - (Crosshair.Texture->GetSurfaceHeight() * ScaleUIY * 0.5f)),
		(Center.Y - (Crosshair.Texture->GetSurfaceWidth() * ScaleUIY * 0.5f)), 1); // Not entirely sure what UL and VL are, but the 1st person shooter example included them. -John
	//UE_LOG(LogTemp, Log, TEXT("%d"), Crosshair.UL));
	*/

}
