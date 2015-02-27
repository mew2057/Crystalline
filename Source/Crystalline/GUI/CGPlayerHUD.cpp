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
	HitTakenColor = FLinearColor::Red;
	TimeSinceLastHitTaken = 0.f;
	TimeSinceLastHitConfirmed = 0.f;
}

void ACGPlayerHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// Initialize icons.
	ButtonIcons.Initialize();

	int32 StrLen;
	int32 Key;
	int32 Num;
	TArray<FInputActionKeyMapping> Keys;
	bool GamepadConnected = false;

	// Set Action Button Icon.

	Keys = PlayerOwner->PlayerInput->GetKeysForAction("ActionButton");
	Num = Keys.Num();

	for (int32 i = 0; i < Num; ++i)
	{
		if (Keys[i].Key.IsGamepadKey() && GamepadConnected)
		{
		//	Keys[i].Key
			break;
		}
		else if (!GamepadConnected)
		{
			FString KeyString = Keys[i].Key.ToString();
			StrLen = KeyString.Len();
			if (StrLen == 1 )
			{
				Key = (int32)KeyString[0];// -BUTTON_ICON_OFFSET;

				Key -= Key >= ALPHA_BUTTON_ICON_OFFSET ? ALPHA_BUTTON_ICON_OFFSET : NUM_ALPHA_BUTTON_OFFSET;

				UE_LOG(LogTemp, Log, TEXT("KeyId : %d"), Key);
				ButtonIcons.SetKeyboardActionIcon(Key);
				break;
			}
		}
	}


	// Prioritize
	
}

// TODO add a dirty bit for player Scoring.
void ACGPlayerHUD::PostRender()
{
	// FIXME This is a point for optimization.
	ACGGameState* GS = GetWorld()->GetGameState<ACGGameState>();
	if (GS != NULL)
	{
		GS->SortPlayers();
	}

	Super::PostRender();
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
			Canvas->SetDrawColor(Weapon->CheckCanHit() ? FColor::Red : FColor::White);

			//////////////////////////
			// Crosshair	
			FCanvasIcon CrosshairIcon = Weapon->WeaponHUDConfig.CrosshairIcon;
			// ScaleUI is 1 at 1080, .5 at 540 and 2 at 2160.
			// The UL and VL values indicate the width and length of the texture respectively.
			Canvas->DrawIcon(CrosshairIcon,
				(Center.X - (CrosshairIcon.UL * ScaleUIY * 0.5f)),
				(Center.Y - (CrosshairIcon.VL * ScaleUIY * 0.5f)), ScaleUIY);

			// Play hit confirmation
			if (GetWorld()->GetTimeSeconds() - TimeSinceLastHitConfirmed < TimeToDisplayHitConfirmed)
			{
				//Canvas->SetDrawColor(FLinearColor(1.f, 1.f, 1.f, 1 - (GetWorld()->GetTimeSeconds() - TimeSinceLastHitConfirmed) / TimeToDisplayHitConfirmed));
				Canvas->DrawIcon(HitConfirmedIcon,
					(Center.X - (CrosshairIcon.UL * ScaleUIY * 0.5f)),
					(Center.Y - (CrosshairIcon.VL * ScaleUIY * 0.5f)), ScaleUIY);
			}

			DrawWeaponHUD();
			DrawPrompt();
		}

		
	}

	// Draw the HitTaken if we've been hit recently.
	if (GetWorld()->GetTimeSeconds() - TimeSinceLastHitTaken < TimeToDisplayHitTaken)
	{
		Canvas->PopSafeZoneTransform();
		HitTakenColor.A = 1.f - ((GetWorld()->GetTimeSeconds() - TimeSinceLastHitTaken) / TimeToDisplayHitTaken);
		FCanvasTileItem TileItem(FVector2D(0, 0), HitTakenOverlay->Resource, FVector2D(Canvas->ClipX, Canvas->ClipY), HitTakenColor);
		TileItem.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(TileItem);
		Canvas->ApplySafeZoneTransform();
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

			// Ammo in gun.
			FString Text = FString::Printf(TEXT("%3d"), CurrentWeapon->GetAmmoInClip());
			
			DrawScaledText(
				Text,
				WeaponElement.AmmoTextColor,
				X + PixelsPerWidth * WeaponElement.InClipAmmoTransform.PercentX,
				Y + PixelsPerHeight * WeaponElement.InClipAmmoTransform.PercentY,
				BigFont,
				PixelsPerHeight * WeaponElement.InClipAmmoTransform.HeightPercent,
				WeaponElement.InClipAnchor);

			
			// Ammo held.
			Text = FString::Printf(TEXT("%4d"), CurrentWeapon->GetAmmo());
			DrawScaledText(
				Text,
				WeaponElement.AmmoTextColor,
				X + PixelsPerWidth * WeaponElement.HeldAmmoTransform.PercentX,
				Y + PixelsPerHeight * WeaponElement.HeldAmmoTransform.PercentY,
				BigFont,
				PixelsPerHeight * WeaponElement.HeldAmmoTransform.HeightPercent,
				WeaponElement.HeldAmmoAnchor);

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
	ACGGameState* const CGGameState = GetWorld()->GetGameState<ACGGameState>();
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
		const int32 Minutes = CGGameState->RemainingTime / 60;
		const int32 Seconds = CGGameState->RemainingTime % 60;

		FString Text = FString::Printf(TEXT("%2d : %02d"), Minutes, Seconds);

		DrawScaledText(
			Text,
			RoundDataElement.TimeColor,
			X + PixelsPerWidth * RoundDataElement.TimeTransform.PercentX,
			Y + PixelsPerHeight * RoundDataElement.TimeTransform.PercentY,
			BigFont,
			PixelsPerHeight * RoundDataElement.TimeTransform.HeightPercent,
			RoundDataElement.TimeAnchor);

		//////////////////////////////////////////////////////////////////////////////////////
		// Start Score Output.
		/////////////////////////////////////////////////////////////////////////////////////

		AController* const Controller = GetOwningPlayerController();
		APlayerState* const PlayerState = Controller ? Controller->PlayerState : NULL;
		// TODO get Score to win.

		const int32 NumElements = FMath::Min(CGGameState->PlayerArray.Num(), RoundDataElement.GameDataElements.Num());
		FCGGameElement TempElement;
		APlayerState* TempPlayerState;
		bool bPlayerFound = false;

		for (int i = 0; i < NumElements; ++i)
		{
			TempElement = RoundDataElement.GameDataElements[i];

			// On the last iteration this player is forcibly applied.
			// XXX is there a more elegant solution?
			if (i < (NumElements - 1) || bPlayerFound || PlayerState == NULL)
			{
				TempPlayerState = CGGameState->PlayerArray[i];
			}
			else
			{
				TempPlayerState = PlayerState ;
			}

			Canvas->SetDrawColor(RoundDataElement.DataElementColor);

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

			// TODO verify Goal Score.
			Canvas->DrawTile(
				RoundDataElement.FGIcon.Texture,
				ElemX, ElemY,
				ElemW * TempPlayerState->Score / CGGameState->GoalScore, ElemH,
				RoundDataElement.FGIcon.U, RoundDataElement.FGIcon.V,
				RoundDataElement.FGIcon.UL * TempPlayerState->Score / CGGameState->GoalScore, RoundDataElement.FGIcon.VL,
				EBlendMode::BLEND_Translucent);
			
			// Score goes here.
			// TODO this needs some kind of anchoring.
			Text = FString::Printf(TEXT("%.0f"), TempPlayerState->Score);
			DrawScaledText(
				Text, 
				RoundDataElement.ScoreColor, 
				ElemX + ElemW + PixelsPerWidth * RoundDataElement.ScoreTransform.PercentX, 
				ElemY + PixelsPerHeight * RoundDataElement.ScoreTransform.PercentY, 
				BigFont, PixelsPerHeight * RoundDataElement.ScoreTransform.HeightPercent,
				RoundDataElement.ScoreAnchor);


			if (TempPlayerState == PlayerState)
			{
				bPlayerFound = true;
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

float ACGPlayerHUD::DrawScaledText(const FString & Text, FLinearColor TextColor, float ScreenX, float ScreenY, UFont * Font, float TextHeight, float Anchor)
{

	FCanvasTextItem TextItem(FVector2D::ZeroVector, FText::GetEmpty(), Font, TextColor);
	TextItem.Text = FText::FromString(Text);

	// Get the size of the text.
	float SizeX, SizeY;
	Canvas->StrLen(Font, Text, SizeX, SizeY);

	// Compute the scale for the final text.
	const float Scale = TextHeight / SizeY;
	TextItem.Scale.Set(Scale, Scale);

	// Anchor offset.
	ScreenX -= SizeX * Scale * Anchor;

	// TODO this Jitters slightly.
	Canvas->DrawItem(TextItem, ScreenX, ScreenY);

	return SizeX * Scale;
}

void ACGPlayerHUD::DrawPrompt()
{
	if (Prompt.bPrompt)
	{
		// Get the Main Anchor for our weapon Element.
		float X = PixelsPerCent.X * Prompt.Transform.PercentX;
		const float Y = PixelsPerCent.Y * Prompt.Transform.PercentY;
		const float Height = PixelsPerCent.Y * Prompt.Transform.HeightPercent;
		const bool bDisplayButton = ButtonIcons.ButtonIcons.Num() > Prompt.CurrentButton;

		////////////////////////////////////////////////////////////////////////////////////////////
		if (Prompt.Anchor > 0.f)
		{
			// Compute the total size of the message for justification.
			float SizeX, SizeY;
			Canvas->StrLen(BigFont, Prompt.BasePrompt + Prompt.PromptMessage, SizeX, SizeY);
			const float Scale = Height / SizeY;

			SizeX = (SizeX * Scale) + PixelsPerCent.X * Prompt.PromptKeyOffset + bDisplayButton * (PixelsPerCent.X * Prompt.PromptKeyOffset + Height);
			X -= SizeX * Prompt.Anchor;
		}

		////////////////////////////////////////////////////////////////////////////////////////////
		float CurrentX = DrawScaledText(
			Prompt.BasePrompt,
			Prompt.PromptTextColor,
			X, Y,
			BigFont,
			Height);

		if (bDisplayButton)
		{
			Canvas->SetDrawColor(FColor::White);

			FVector2D ButtonUV = ButtonIcons.ButtonIcons[Prompt.CurrentButton];

			CurrentX = X + CurrentX + PixelsPerCent.X * Prompt.PromptKeyOffset;

			Canvas->DrawTile(
				ButtonIcons.ButtonIconTexture,
				CurrentX, Y,
				Height,	Height,
				ButtonUV.X, ButtonUV.Y,
				ButtonIcons.IconWidth, ButtonIcons.IconHeight,
				EBlendMode::BLEND_Translucent);

			CurrentX += Height;
		}

		CurrentX += PixelsPerCent.X * Prompt.PromptKeyOffset;

		DrawScaledText(
			Prompt.PromptMessage,
			Prompt.PromptTextColor,
			CurrentX, Y,
			BigFont,
			Height);
	}


	// TODO Make this print out images and whatnot.
	/*
	float SizeX, SizeY;
	FCanvasTextItem TextItem(FVector2D::ZeroVector, FText::GetEmpty(), BigFont, FLinearColor::White);
	TextItem.EnableShadow(FLinearColor::Black);
	Canvas->StrLen(BigFont, PromptMessage, SizeX, SizeY);

	const float TopTextScale = 1.f; 

	TextItem.Text = FText::FromString(PromptMessage);
	TextItem.Scale = FVector2D(TopTextScale, TopTextScale);

	Canvas->SetDrawColor(FColor::Yellow);

	Canvas->DrawItem(TextItem, 50, 100);
*/
}

void ACGPlayerHUD::SetPromptMessage(bool bSetPrompt, const FString& Message, int32 ButtonID)
{
	Prompt.PromptMessage = Message;
	Prompt.bPrompt = bSetPrompt;
	Prompt.CurrentButton = ButtonID;

}

void ACGPlayerHUD::NotifyHitTaken()
{
	float TempTime = GetWorld()->GetTimeSeconds();
	TimeSinceLastHitTaken = TempTime - TimeSinceLastHitTaken > TimeToDisplayHitTaken ? TempTime : TimeSinceLastHitTaken;
}

void ACGPlayerHUD::NotifyHitConfirmed()
{
	TimeSinceLastHitConfirmed = GetWorld()->GetTimeSeconds();
}
