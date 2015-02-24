// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGPlayerHUD.h"
#include "Engine/Canvas.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "GameModes/CGGameState.h"
#include "GameModes/CGPlayerState.h"
#include "GameModes/CGBaseGameMode.h"

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

	PixelsPerCent = FVector2D(Canvas->ClipX * .01f, Canvas->ClipY * 0.01f);
	ACGCharacter* Pawn = Cast<ACGCharacter>(GetOwningPawn());
	if (Pawn)
	{
		DrawShield();
		

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
			DrawPrompt();
			/*
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

			Canvas->DrawItem(TextItem, 50, 150);*/
		}
	}

	DrawGameInfo();
}


void ACGPlayerHUD::DrawWeaponHUD()
{
	// XXX Crystals should be present on the weapon sprites for the crystal gun.
	
	ACGCharacter* Pawn = Cast<ACGCharacter>(GetOwningPawn());
	ACGWeapon* CurrentWeapon = Pawn ? Pawn->GetCurrentWeapon() : NULL;
	ACGInventory* CurrentInventory = Pawn ? Pawn->Inventory : NULL;

	FCanvasIcon WeaponIcon;	
	FCanvasIcon AmmoIcon;

	// Get the Main Anchor for our weapon Element.
	const float X = PixelsPerCent.X * WeaponElement.Transform.PercentX;
	const float Y = PixelsPerCent.Y * WeaponElement.Transform.PercentY;
	const float PixelsPerWidth = PixelsPerCent.X * WeaponElement.Transform.WidthPercent * .01f;
	const float PixelsPerHeight = PixelsPerCent.Y * WeaponElement.Transform.HeightPercent * .01f;


	/** Get Primary weapon for the player. */
	if (CurrentWeapon )
	{
		FCGWeaponHUDData WeaponHUDConfig = CurrentWeapon->WeaponHUDConfig;
		// TODO team modifications.
		Canvas->SetDrawColor(FColor::White);
	
		AmmoIcon = WeaponHUDConfig.AmmoGuageBGIcon;

		Canvas->DrawTile(
			AmmoIcon.Texture,
			X + PixelsPerWidth * WeaponElement.GuageTransform.PercentX,
			Y + PixelsPerHeight * WeaponElement.GuageTransform.PercentY,
			PixelsPerWidth * WeaponElement.GuageTransform.WidthPercent,
			PixelsPerHeight * WeaponElement.GuageTransform.HeightPercent,
			AmmoIcon.U,  AmmoIcon.V,
			AmmoIcon.UL, AmmoIcon.VL,
			EBlendMode::BLEND_Translucent);


		AmmoIcon = WeaponHUDConfig.AmmoGuageFGIcon;		
	
		const float Percent = CurrentWeapon->GetClipPercent();
	
		// Determine the draw behavior.
		if (CurrentWeapon->WeaponConfig.bOverHeatWeapon)
		{
			Canvas->SetDrawColor(FMath::Lerp(WeaponHUDConfig.FullAmmoColor, WeaponHUDConfig.LowAmmoColor, Percent));

			Canvas->DrawTile(
				AmmoIcon.Texture,
				X + PixelsPerWidth * WeaponElement.GuageTransform.PercentX,
				Y + PixelsPerHeight * WeaponElement.GuageTransform.PercentY,
				PixelsPerWidth * WeaponElement.GuageTransform.WidthPercent * Percent,
				PixelsPerHeight * WeaponElement.GuageTransform.HeightPercent,
				AmmoIcon.U, AmmoIcon.V,
				AmmoIcon.UL * Percent, AmmoIcon.VL,
				EBlendMode::BLEND_Translucent);
		}
		else
		{
			Canvas->SetDrawColor(FMath::Lerp(WeaponHUDConfig.LowAmmoColor, WeaponHUDConfig.FullAmmoColor, Percent));

			Canvas->DrawTile(
				AmmoIcon.Texture,
				X + PixelsPerWidth * WeaponElement.GuageTransform.PercentX,
				Y + PixelsPerHeight * WeaponElement.GuageTransform.PercentY,
				PixelsPerWidth * WeaponElement.GuageTransform.WidthPercent * Percent,
				PixelsPerHeight * WeaponElement.GuageTransform.HeightPercent,
				AmmoIcon.U, AmmoIcon.V,
				AmmoIcon.UL * Percent, AmmoIcon.VL,
				EBlendMode::BLEND_Translucent);

			// Ammo Text only here.

			// TODO improve Text scaling.
			// Ammo in gun.
			float SizeX, SizeY;
			FString Text = FString::Printf(TEXT("%3d"), CurrentWeapon->GetAmmoInClip());
			FCanvasTextItem TextItem(FVector2D::ZeroVector, FText::GetEmpty(), BigFont, WeaponElement.AmmoTextColor);
			TextItem.Text = FText::FromString(Text);

			// Get the actual size, this is to scale the text to our "Box"
			Canvas->StrLen(BigFont, Text, SizeX, SizeY);
			TextItem.Scale.Set((PixelsPerWidth * WeaponElement.InClipAmmoTransform.WidthPercent) / SizeX, (PixelsPerHeight * WeaponElement.InClipAmmoTransform.HeightPercent) / SizeY);
			Canvas->DrawItem(TextItem, X + PixelsPerWidth * WeaponElement.InClipAmmoTransform.PercentX, Y + PixelsPerHeight * WeaponElement.InClipAmmoTransform.PercentY);
			
			// Ammo held.
			Text = FString::Printf(TEXT("%4d"), CurrentWeapon->GetAmmo());
			TextItem.Text = FText::FromString(Text);

			// Get the actual size, this is to scale the text to our "Box"
			Canvas->StrLen(BigFont, Text, SizeX, SizeY);
			TextItem.Scale.Set((PixelsPerWidth * WeaponElement.HeldAmmoTransform.WidthPercent) / SizeX, (PixelsPerHeight * WeaponElement.HeldAmmoTransform.HeightPercent) / SizeY);
			Canvas->DrawItem(TextItem, X + PixelsPerWidth * WeaponElement.HeldAmmoTransform.PercentX, Y + PixelsPerHeight * WeaponElement.HeldAmmoTransform.PercentY);

		}
		
		Canvas->SetDrawColor(FColor::White);	

		Canvas->DrawTile(
			WeaponHUDConfig.WeaponIcon.Texture,
			X + PixelsPerWidth * WeaponElement.MainIconTransform.PercentX,
			Y + PixelsPerHeight * WeaponElement.MainIconTransform.PercentY,
			PixelsPerWidth * WeaponElement.MainIconTransform.WidthPercent,
			PixelsPerHeight * WeaponElement.MainIconTransform.HeightPercent,
			WeaponHUDConfig.WeaponIcon.U,  WeaponHUDConfig.WeaponIcon.V,
			WeaponHUDConfig.WeaponIcon.UL, WeaponHUDConfig.WeaponIcon.VL,
			EBlendMode::BLEND_Translucent);

		// TODO Crystal Display
		// FIXME this crashes when both players are dead and respawning. Seems to happen on client, this ONLY occurs in release mode.
		/*
		if (Pawn && Pawn->Inventory)
		{
			/** Draw the crystals the player has*/
		/*	TextItem.Text = FText::FromString(TEXT("T1: " + FString::FromInt((int8)Pawn->Inventory->TierOneCrystal)));
			Canvas->DrawItem(TextItem, 5, 100);
	
			TextItem.Text = FText::FromString(TEXT("T2: " + FString::FromInt((int8)Pawn->Inventory->TierTwoCrystal)));
			Canvas->DrawItem(TextItem, 5, 150);
		}*/
	
	}
}

void ACGPlayerHUD::DrawShield()
{
	ACGCharacter* Pawn = Cast<ACGCharacter>(GetOwningPawn());
	
	Canvas->SetDrawColor(FColor::White);	
	const FCGHUDTransform Transform = Shield.Transform;

	Canvas->DrawTile(
		Shield.BGIcon.Texture,
		PixelsPerCent.X * Transform.PercentX,
		PixelsPerCent.Y * Transform.PercentY,
		PixelsPerCent.X * Transform.WidthPercent,
		PixelsPerCent.Y * Transform.HeightPercent,
		Shield.BGIcon.U, Shield.BGIcon.V,
		Shield.BGIcon.UL, Shield.BGIcon.VL,
		EBlendMode::BLEND_Translucent);


	if (Pawn)
	{
		float Percent = Pawn->GetShieldPercent();
		Canvas->SetDrawColor(FMath::Lerp(Shield.EmptyColor, Shield.FullColor, Percent));
		
		Canvas->DrawTile(
			Shield.FGIcon.Texture,
			PixelsPerCent.X * Transform.PercentX,
			PixelsPerCent.Y * Transform.PercentY,
			PixelsPerCent.X * Transform.WidthPercent * Percent,
			PixelsPerCent.Y * Transform.HeightPercent,
			Shield.FGIcon.U, Shield.FGIcon.V,
			Shield.FGIcon.UL * Percent, Shield.FGIcon.VL,
			EBlendMode::BLEND_Translucent);
	}
	
	// TODO Flashing when health low.
}

void ACGPlayerHUD::DrawGameInfo()
{
	ACGGameState* const CGGameState = Cast<ACGGameState>(GetWorld()->GameState);
	if (CGGameState)
	{
		// XXX Maybe cache this 
		// Get the relative space for this module.
		const float X = PixelsPerCent.X * RoundDataElement.Transform.PercentX;
		const float Y = PixelsPerCent.Y * RoundDataElement.Transform.PercentY;
		const float PixelsPerWidth  = PixelsPerCent.X * RoundDataElement.Transform.WidthPercent * .01f;
		const float PixelsPerHeight = PixelsPerCent.Y * RoundDataElement.Transform.HeightPercent * .01f;
		
		//////////////////////////////////////////////////////////////////////////////////////
		// Start Time Output
		//////////////////////////////////////////////////////////////////////////////////////
		float SizeX, SizeY;
		const int32 Minutes = CGGameState->RemainingTime / 60;
		const int32 Seconds = CGGameState->RemainingTime % 60;

		FString Text = FString::Printf(TEXT("%2d : %02d"), Minutes, Seconds);
		FCanvasTextItem TextItem(FVector2D::ZeroVector, FText::GetEmpty(), BigFont, RoundDataElement.TimeColor);
		TextItem.Text = FText::FromString(Text);

		// Get the actual size, this is to scale the text to our "Box"
		Canvas->StrLen(BigFont, Text, SizeX, SizeY);
		TextItem.Scale.Set((PixelsPerWidth * RoundDataElement.TimeTransform.WidthPercent) / SizeX, 	(PixelsPerHeight * RoundDataElement.TimeTransform.HeightPercent) / SizeY);

		// TODO this Jitters slightly.
		Canvas->DrawItem(TextItem, X + PixelsPerWidth * RoundDataElement.TimeTransform.PercentX, Y + PixelsPerHeight * RoundDataElement.TimeTransform.PercentY);	


		//////////////////////////////////////////////////////////////////////////////////////
		// Start Score Output.
		/////////////////////////////////////////////////////////////////////////////////////

		AController* Controller = GetOwningPlayerController();

		// Poll Who's winning.
		

		// Check Dirty bit for game state.
		//
		const int32 NumElements = RoundDataElement.GameDataElements.Num();
		FCGGameElement TempElement;

		for (int i = NumElements - CGGameState->PlayerArray.Num(); i < NumElements; ++i)
		{

			TempElement = RoundDataElement.GameDataElements[i];
			Canvas->SetDrawColor(FColor::White);

			const float ElemX = X + PixelsPerWidth * TempElement.Transform.PercentX;
			const float ElemY = Y + PixelsPerHeight * TempElement.Transform.PercentY;
			const float ElemW = PixelsPerWidth * TempElement.Transform.WidthPercent;
			const float ElemH = PixelsPerHeight * TempElement.Transform.HeightPercent;

			Canvas->DrawTile(
				RoundDataElement.BGIcon.Texture, 
				ElemX, ElemY,
				ElemW, ElemH,
				RoundDataElement.BGIcon.U, RoundDataElement.BGIcon.V,
				RoundDataElement.BGIcon.UL,RoundDataElement.BGIcon.VL,
				EBlendMode::BLEND_Translucent);

			Canvas->SetDrawColor(FColor::Blue);

			Canvas->DrawTile(
				RoundDataElement.FGIcon.Texture,
				ElemX, ElemY,
				ElemW * TempElement.PercentToGoal, ElemH,
				RoundDataElement.FGIcon.U, RoundDataElement.FGIcon.V,
				RoundDataElement.FGIcon.UL * TempElement.PercentToGoal, RoundDataElement.FGIcon.VL,
				EBlendMode::BLEND_Translucent);
			
			// Score goes here.
			Text = FString::Printf(TEXT("%02d"), TempElement.Score);
			TextItem.SetColor(RoundDataElement.ScoreColor);
			TextItem.Text = FText::FromString(Text);

			// Get the actual size, this is to scale the text to our "Box"
			Canvas->StrLen(BigFont, Text, SizeX, SizeY);

			// HUD Width.
			// TODO get scale working properly.
			TextItem.Scale.Set((PixelsPerWidth * RoundDataElement.ScoreTransform.WidthPercent) / SizeX, (PixelsPerHeight * RoundDataElement.ScoreTransform.HeightPercent) / SizeY);
			Canvas->DrawItem(TextItem, ElemX + ElemW + PixelsPerWidth * RoundDataElement.ScoreTransform.PercentX, ElemY + PixelsPerHeight * RoundDataElement.ScoreTransform.PercentY);

			
			if (TempElement.bIsOwner)
			{
				Canvas->SetDrawColor(FColor::White);

				Canvas->DrawTile(
					RoundDataElement.OwnerIcon.Texture,	
					X, ElemY,
					PixelsPerWidth * TempElement.Transform.PercentX,
					PixelsPerHeight * TempElement.Transform.HeightPercent,
					RoundDataElement.OwnerIcon.U, RoundDataElement.OwnerIcon.V,
					RoundDataElement.OwnerIcon.UL, RoundDataElement.OwnerIcon.VL,
					EBlendMode::BLEND_Translucent);
			}
		}
	}
}

void ACGPlayerHUD::DrawPrompt()
{
	// TODO Make this print out images and whatnot.
	float SizeX, SizeY;
	FCanvasTextItem TextItem(FVector2D::ZeroVector, FText::GetEmpty(), BigFont, FLinearColor::White);
	TextItem.EnableShadow(FLinearColor::Black);
	Canvas->StrLen(BigFont, PromptMessage, SizeX, SizeY);

	const float TopTextScale = 1.f; 

	TextItem.Text = FText::FromString(PromptMessage);
	TextItem.Scale = FVector2D(TopTextScale, TopTextScale);

	Canvas->SetDrawColor(FColor::Yellow);

	Canvas->DrawItem(TextItem, 50, 100);
}

void ACGPlayerHUD::SetPromptMessage(const FString& Message)
{
	PromptMessage = Message;
}
