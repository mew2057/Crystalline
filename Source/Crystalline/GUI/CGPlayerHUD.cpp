// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGPlayerHUD.h"
#include "Engine/Canvas.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "Weapons/States/CGWeaponState.h"

ACGPlayerHUD::ACGPlayerHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// TODO REPLACE THIS FONT!
	static ConstructorHelpers::FObjectFinder<UFont> BigFontOb(TEXT("/Game/Textures/MenuFont"));
	BigFont = BigFontOb.Object;
}


void ACGPlayerHUD::DrawHUD()
{
	Super::DrawHUD();

	// Calculate the scale factor of the ui based on the clip space of the screen.
	// XXX We may be able to move this to an event response in the future (need to research if the clipspace change is bindable). -John
	ScaleUIY = Canvas->ClipY / TARGET_Y_RESOLUTION;
	//ScaleUIX = Canvas->ClipX / TARGET_X_RESOLUTION; XXX This may be needed in the future, we'll see how using only the Y for scaling works. -John

	// Find center of the Canvas. XXX Does this need to be a vector? -John
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	ACGCharacter* Pawn = Cast<ACGCharacter>(GetOwningPawn());
	if (Pawn)
	{
		DrawHealth();

		ACGWeapon* Weapon = Pawn->GetCurrentWeapon();
		if (Weapon)
		{
			Canvas->SetDrawColor(FColor::White);

			//////////////////////////
			// Crosshair	
			FCanvasIcon CrosshairIcon = Weapon->WeaponHUDConfig.CrosshairIcon;
			// ScaleUI is 1 at 1080, .5 at 540 and 2 at 2160.
			// The UL and VL values indicate the width and length of the texture respectively.
			Canvas->DrawIcon(CrosshairIcon,
				(Center.X - (CrosshairIcon.UL * ScaleUIY * 0.5f)),
				(Center.Y - (CrosshairIcon.VL * ScaleUIY * 0.5f)), ScaleUIY);

			DrawWeaponHUD();

			// State Print out
			float SizeX, SizeY;
			FString Text = Weapon->CurrentState->GetName();

			FCanvasTextItem TextItem(FVector2D::ZeroVector, FText::GetEmpty(), BigFont, FLinearColor::White);
			TextItem.EnableShadow(FLinearColor::Black);
			Canvas->StrLen(BigFont, Text, SizeX, SizeY);

			const float TopTextScale = 0.73f; // of 51pt font

			TextItem.Text = FText::FromString(Text);
			TextItem.Scale = FVector2D(TopTextScale * ScaleUIY, TopTextScale * ScaleUIY);
			//TextItem.FontRenderInfo = ShadowedFont;

			Canvas->DrawItem(TextItem, 100, 100);
		}
	}

	
}


void ACGPlayerHUD::DrawWeaponHUD()
{
	// XXX Crystals should be present on the weapon sprites for the crystal gun.
	
	ACGCharacter* Pawn = Cast<ACGCharacter>(GetOwningPawn());
	ACGWeapon* CurrentWeapon = Pawn->GetCurrentWeapon();
	FCanvasIcon WeaponIcon;
	
	FCanvasIcon AmmoIcon;
	
	/** Get Primary weapon for the player. */
	if (CurrentWeapon)
	{
		FCGWeaponHUDData WeaponHUDConfig = CurrentWeapon->WeaponHUDConfig;
		// TODO team modifications.
		Canvas->SetDrawColor(FColor::White);
	
		AmmoIcon = WeaponHUDConfig.AmmoGuageBGIcon;
		Canvas->DrawIcon(AmmoIcon, 20, 20, ScaleUIY);
		
		AmmoIcon = WeaponHUDConfig.AmmoGuageFGIcon;
	
		const float Percent = CurrentWeapon->GetClipPercent();

		// Determine the appropriate color for the guage.

		if (CurrentWeapon->WeaponConfig.OverHeatWeapon)
		{
			Canvas->SetDrawColor(FMath::Lerp(WeaponHUDConfig.FullAmmoColor, WeaponHUDConfig.LowAmmoColor, Percent));
			AmmoIcon.UL = WeaponHUDConfig.AmmoGuageWidth * Percent + ICON_FUDGE;
		}
		else
		{
			Canvas->SetDrawColor(FMath::Lerp(WeaponHUDConfig.LowAmmoColor, WeaponHUDConfig.FullAmmoColor, Percent));
			AmmoIcon.UL = WeaponHUDConfig.AmmoGuageWidth * Percent + ICON_FUDGE;
		}

		Canvas->DrawIcon(AmmoIcon, 20, 20, ScaleUIY);

		Canvas->SetDrawColor(FColor::White);	
		Canvas->DrawIcon(WeaponHUDConfig.WeaponIcon, 40, 20, ScaleUIY);

		// Debug Ammo Text
		////////////////////////////////////////////////////
		float SizeX, SizeY;
		FString Text = FString::SanitizeFloat(CurrentWeapon->GetAmmoInClip());

		FCanvasTextItem TextItem(FVector2D::ZeroVector, FText::GetEmpty(), BigFont, FLinearColor::White);
		TextItem.EnableShadow(FLinearColor::Black);
		Canvas->StrLen(BigFont, Text, SizeX, SizeY);

		const float TopTextScale = 0.73f; // of 51pt font

		TextItem.Text = FText::FromString(Text);
		TextItem.Scale = FVector2D(TopTextScale * ScaleUIY, TopTextScale * ScaleUIY);
		//TextItem.FontRenderInfo = ShadowedFont;

		Canvas->DrawItem(TextItem, 20, 10);

		Text = FString::SanitizeFloat(CurrentWeapon->GetAmmo());
		TextItem.Text = FText::FromString(Text);

		Canvas->DrawItem(TextItem, 50, 10);
		////////////////////////////////////////////////////
	}
	
	
	/** Get the Secondary weapon for the player. */
	/*
	ACGWeapon* SecondaryWeapon = Pawn->GetSecondaryWeapon();
	
	if (SecondaryWeapon)
	{
	
		WeaponIcon = SecondaryWeapon->WeaponHUDConfig.WeaponIcon;
		Canvas->DrawIcon(WeaponIcon, 40, 60, ScaleUIY);
	
		// Output the icon.
		// NOTE: Don't include ammunition on this one.
	
	}
	*/
}

void ACGPlayerHUD::DrawHealth()
{
	ACGCharacter* Pawn = Cast<ACGCharacter>(GetOwningPawn());


	// TODO replace this!
	// Write a text function.
	
	float SizeX, SizeY;
	FString Text = FString::SanitizeFloat(Pawn->GeCurrentShield());

	FCanvasTextItem TextItem(FVector2D::ZeroVector, FText::GetEmpty(), BigFont, FLinearColor::White);
	TextItem.EnableShadow(FLinearColor::Black);
	Canvas->StrLen(BigFont, Text, SizeX, SizeY);

	const float TopTextScale = 0.73f; // of 51pt font

	TextItem.Text = FText::FromString(Text);
	TextItem.Scale = FVector2D(TopTextScale * ScaleUIY, TopTextScale * ScaleUIY);
	//TextItem.FontRenderInfo = ShadowedFont;

	Canvas->DrawItem(TextItem, 100, 300);

	Text = FString::SanitizeFloat(Pawn->GetCurrentHealth());
	TextItem.Text = FText::FromString(Text);

	Canvas->DrawItem(TextItem, 100, 350);


}

void ACGPlayerHUD::DrawGameInfo()
{

}
