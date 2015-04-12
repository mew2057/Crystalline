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
	static ConstructorHelpers::FObjectFinder<UFont> FontOb(TEXT("/Game/Textures/InGameHUD/Source_Reg_20"));
	Font = FontOb.Object;
	HitTakenColor = FLinearColor::Red;
	TimeSinceLastHitTaken = 0.f;
	TimeSinceLastHitConfirmed = 0.f;
	bScoreboardVisible = false;
}

void ACGPlayerHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// Initialize icons.
	ButtonIcons.Initialize();
	
	// TODO Remove hard coding.
	bool GamepadConnected = false;

	// Set Button Icons up.	
	// FIXME This crashes the editor!
	DetermineKeyCodeForAction("ActionButton", ACTION_BUTTON, GamepadConnected);
	DetermineKeyCodeForAction("PopCrystalButton", POP_BUTTON, GamepadConnected);

	// Initialize the damage indicators
	DamageIndicators.AddZeroed(MaxDamageIndicatorCount);
}

//XXX This is Janky code.
void ACGPlayerHUD::DetermineKeyCodeForAction(const FName& Action, int32 ButtonID, bool GamepadConnected)
{
	if (PlayerOwner == NULL)
	{
		return;
	}

	// Crashes PIE
	TArray<FInputActionKeyMapping> Keys = PlayerOwner->PlayerInput->GetKeysForAction(Action);
	int32 Num = Keys.Num();;
	int32 Key = -1;

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
			const float StrLen = KeyString.Len();

			if (StrLen == 1)
			{
				Key = (int32)KeyString[0] - ALPHA_BUTTON_ICON_OFFSET;
			}
			else
			{
				// I can't believe Unreal encoded it like this.
				if (KeyString == "Zero")
				{
					Key = NUM_BUTTON_OFFSET;
				}
				else if (KeyString == "One")
				{
					Key = NUM_BUTTON_OFFSET + 1;
				}
				else if (KeyString == "Two")
				{
					Key = NUM_BUTTON_OFFSET + 2;
				}
				else if (KeyString == "Three")
				{
					Key = NUM_BUTTON_OFFSET + 3;
				}
				else if (KeyString == "Four")
				{
					Key = NUM_BUTTON_OFFSET + 4;
				}
				else if (KeyString == "Five")
				{
					Key = NUM_BUTTON_OFFSET + 5;
				}
				else if (KeyString == "Six")
				{
					Key = NUM_BUTTON_OFFSET + 6;
				}
				else if (KeyString == "Seven")
				{
					Key = NUM_BUTTON_OFFSET + 7;
				}
				else if (KeyString == "Eight")
				{
					Key = NUM_BUTTON_OFFSET + 8;
				}
				else if (KeyString == "Nine")
				{
					Key = NUM_BUTTON_OFFSET + 9;
				}
			}

			// Kick out if the key was found.
			if (Key > 0)
			{
				ButtonIcons.SetKeyboardIcon(Key, ButtonID);
				break;
			}
		}
	}	
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
	
	PixelsPerCent = FVector2D(Canvas->ClipX * .01f, Canvas->ClipY * 0.01f);
	ACGCharacter* Pawn = Cast<ACGCharacter>(GetOwningPawn());
	if (Pawn)
	{
		DrawShield();		

		ACGWeapon* Weapon = Pawn->GetCurrentWeapon();
		if (Weapon)
		{
			// Weapon hud is always drawn.
			DrawWeaponHUD();


			//////////////////////////
			// Crosshair	
			if (!bScoreboardVisible)
			{
				DrawCrosshair();

				if (Prompt.bPrompt)
				{
					DrawPrompt();
				}
			}
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

	DrawDamageIndicators();

	DrawGameInfo();

	// XXX a hack, shows the scoreboard when the player is detached.
	if (bScoreboardVisible || Pawn == NULL)
	{
		DrawScoreboard();
	}

	if (EndGameMessage.bDisplay)
	{
		DrawEndGameMessage();
	}
}

void ACGPlayerHUD::DrawCrosshair()
{
	// Find center of the Canvas. XXX Does this need to be a vector? -John
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	ACGCharacter* Pawn = Cast<ACGCharacter>(GetOwningPawn());
	ACGWeapon* Weapon = Pawn->GetCurrentWeapon();
	
	const float ScaleUIY = Canvas->ClipY / TARGET_Y_RESOLUTION * Crosshair.YScale;

	bool bIsHitting = GetWorld()->GetTimeSeconds() - TimeSinceLastHitConfirmed < TimeToDisplayHitConfirmed;	
	bool bCanHit = false;
	bool bHeadShot = false;
	ACGCharacter* Target = Weapon->CheckCanHit(bCanHit, bHeadShot);
		
	Canvas->SetDrawColor(bIsHitting || bCanHit ? FColor::Red : FColor::White);

	// ScaleUI is 1 at 1080, .5 at 540 and 2 at 2160.
	// The UL and VL values indicate the width and length of the texture respectively.
	Canvas->DrawIcon(Weapon->WeaponHUDConfig.CrosshairIcon,
		(Center.X - (Weapon->WeaponHUDConfig.CrosshairIcon.UL * ScaleUIY * 0.5f)),
		(Center.Y - (Weapon->WeaponHUDConfig.CrosshairIcon.VL * ScaleUIY * 0.5f)), ScaleUIY);

	// Play hit confirmation
	if (bIsHitting)
	{
		Canvas->DrawIcon(Weapon->WeaponHUDConfig.HitConfirmedIcon,
			(Center.X - (Weapon->WeaponHUDConfig.HitConfirmedIcon.UL * ScaleUIY * 0.5f)),
			(Center.Y - (Weapon->WeaponHUDConfig.HitConfirmedIcon.VL * ScaleUIY * 0.5f)), ScaleUIY);
	}

	if (bHeadShot)
	{
		Canvas->DrawIcon(Crosshair.HeadShotIcon,
			(Center.X - (Crosshair.HeadShotIcon.UL * ScaleUIY * 0.5f)),
			(Center.Y - (Crosshair.HeadShotIcon.VL * ScaleUIY * 0.5f)), ScaleUIY);
	}

	// Draw the player name if present.
	if (Target && Target->PlayerState)
	{
		ACGPlayerState* State = Cast<ACGPlayerState>(Target->PlayerState);
		DrawScaledText(
			State->GetShortenedName(),
			Crosshair.EnemyNameColor,
			Center.X,
			Center.Y + Crosshair.EnemyNameOffset * ScaleUIY,
			Font,
			Crosshair.EnemyNameHeight *ScaleUIY,
			.5f);
	}
}

void ACGPlayerHUD::DrawWeaponHUD()
{
	// XXX Crystals should be present on the weapon sprites for the crystal gun.
	
	ACGCharacter* Pawn = Cast<ACGCharacter>(GetOwningPawn());
	ACGWeapon* CurrentWeapon = Pawn ? Pawn->GetCurrentWeapon() : NULL;
	ACGWeapon* OffHandWeapon = Pawn ? Pawn->GetOffHandWeapon() : NULL;
	ACGInventory* CurrentInventory = Pawn ? Pawn->Inventory : NULL;

	FCanvasIcon WeaponIcon;	
	FCanvasIcon AmmoIcon;

	// Get the Main Anchor for our weapon Element.
	const float X = PixelsPerCent.X * WeaponElement.Transform.PercentX;
	const float Y = PixelsPerCent.Y * WeaponElement.Transform.PercentY;
	const float PixelsPerWidth = PixelsPerCent.X * WeaponElement.Transform.WidthPercent * .01f;
	const float PixelsPerHeight = PixelsPerCent.Y * WeaponElement.Transform.HeightPercent * .01f;


	/** Get Primary weapon for the player. */
	if (CurrentWeapon)
	{
		// Current weapon box.
		const float EquippedX = X + PixelsPerWidth * WeaponElement.EquippedWeapon.Transform.PercentX;
		const float EquippedY = Y + PixelsPerHeight * WeaponElement.EquippedWeapon.Transform.PercentY;
		const float EquippedPixelsPerWidth  = PixelsPerWidth  * WeaponElement.EquippedWeapon.Transform.WidthPercent  * .01f;
		const float EquippedPixelsPerHeight = PixelsPerHeight * WeaponElement.EquippedWeapon.Transform.HeightPercent * .01f;
		float FlashFactor = 0.f;

		// Determines whether or not our background should be flashing.
		if (CurrentWeapon->ShouldDisplayAmmoWarning())
		{
			// At most flash at half speed.
			WeaponElement.EquippedWeapon.FlashTime += GetWorld()->GetDeltaSeconds() * WeaponElement.EquippedWeapon.FlashRate;

			// Use a Sine Wave for a smooth flash.
			FlashFactor = ((FMath::Sin(WeaponElement.EquippedWeapon.FlashTime) + 1) * .5f);
		}

		// Draw Background.
		DrawRect(
			FMath::Lerp(WeaponElement.EquippedWeapon.ElementBackgroundColor, WeaponElement.EquippedWeapon.FlashColor, FlashFactor),
			EquippedX, EquippedY,
			PixelsPerWidth  * WeaponElement.EquippedWeapon.Transform.WidthPercent, 
			PixelsPerHeight * WeaponElement.EquippedWeapon.Transform.HeightPercent
		);

		FCGWeaponHUDData WeaponHUDConfig = CurrentWeapon->WeaponHUDConfig;
		
		// Weapon Guage.
		const float ElemX = EquippedX + EquippedPixelsPerWidth  * WeaponElement.EquippedWeapon.GuageTransform.PercentX;
		const float ElemY = EquippedY + EquippedPixelsPerHeight * WeaponElement.EquippedWeapon.GuageTransform.PercentY;
		const float ElemW = EquippedPixelsPerWidth       * WeaponElement.EquippedWeapon.GuageTransform.WidthPercent;
		const float ElemH = EquippedPixelsPerHeight      * WeaponElement.EquippedWeapon.GuageTransform.HeightPercent;
		
		const float Percent = CurrentWeapon->GetClipPercent();

		DrawRect(
			WeaponElement.EquippedWeapon.GuageBackgroundColor,
			ElemX, ElemY,
			ElemW, ElemH 
		);

		// Determine the draw behavior.
		if (CurrentWeapon->WeaponConfig.bOverHeatWeapon)
		{			
			DrawRect(
				FMath::Lerp(WeaponHUDConfig.FullAmmoColor, WeaponHUDConfig.LowAmmoColor, Percent),
				ElemX, ElemY,
				ElemW * Percent, ElemH
			);

			const float ShotStart = ElemX + ElemW * Percent;
			const float ShotWidth = FMath::Min(ElemW - ElemW * Percent, ElemW * CurrentWeapon->GetPercentPerShot());

			// Draw ShotPercent
			DrawRect(
				WeaponElement.EquippedWeapon.GuageShotColor,
				ShotStart, ElemY,
				ShotWidth, ElemH
			);

			FString Text = FString::Printf(TEXT("----"));
			DrawScaledText(
				Text,
				WeaponElement.EquippedWeapon.InClip.Color,
				EquippedX + EquippedPixelsPerWidth * WeaponElement.EquippedWeapon.InClip.Transform.PercentX,
				EquippedY + EquippedPixelsPerHeight * WeaponElement.EquippedWeapon.InClip.Transform.PercentY,
				Font,
				EquippedPixelsPerHeight * WeaponElement.EquippedWeapon.InClip.Transform.HeightPercent,
				WeaponElement.EquippedWeapon.InClip.Anchor);

			Text = FString::Printf(TEXT("----"));
			DrawScaledText(
				Text,
				WeaponElement.EquippedWeapon.HeldAmmo.Color,
				EquippedX + EquippedPixelsPerWidth * WeaponElement.EquippedWeapon.HeldAmmo.Transform.PercentX,
				EquippedY + EquippedPixelsPerHeight * WeaponElement.EquippedWeapon.HeldAmmo.Transform.PercentY,
				Font,
				EquippedPixelsPerHeight * WeaponElement.EquippedWeapon.HeldAmmo.Transform.HeightPercent,
				WeaponElement.EquippedWeapon.HeldAmmo.Anchor);
		}
		else
		{
			// Draw Main bar.
			DrawRect(
				WeaponElement.EquippedWeapon.GuageEnergyColor,
				ElemX, ElemY,
				ElemW * Percent, ElemH
			);
			
			// Draw ShotPercent
			DrawRect(
				WeaponElement.EquippedWeapon.GuageShotColor,
				ElemX + ElemW * Percent, ElemY,
				-ElemW * CurrentWeapon->GetPercentPerShot() * (Percent > 0), ElemH
			);

			// Ammo in gun.
			FString Text = FString::Printf(TEXT("%4d"), CurrentWeapon->GetAmmoInClip());
			DrawScaledText(
				Text,
				WeaponElement.EquippedWeapon.InClip.Color,
				EquippedX + EquippedPixelsPerWidth * WeaponElement.EquippedWeapon.InClip.Transform.PercentX,
				EquippedY + EquippedPixelsPerHeight * WeaponElement.EquippedWeapon.InClip.Transform.PercentY,
				Font,
				EquippedPixelsPerHeight * WeaponElement.EquippedWeapon.InClip.Transform.HeightPercent,
				WeaponElement.EquippedWeapon.InClip.Anchor);

			
			// Ammo held.
			Text = FString::Printf(TEXT("%4d"), CurrentWeapon->GetAmmo());
			DrawScaledText(
				Text,
				WeaponElement.EquippedWeapon.HeldAmmo.Color,
				EquippedX + EquippedPixelsPerWidth * WeaponElement.EquippedWeapon.HeldAmmo.Transform.PercentX,
				EquippedY + EquippedPixelsPerHeight * WeaponElement.EquippedWeapon.HeldAmmo.Transform.PercentY,
				Font,
				EquippedPixelsPerHeight * WeaponElement.EquippedWeapon.HeldAmmo.Transform.HeightPercent,
				WeaponElement.EquippedWeapon.HeldAmmo.Anchor);

		}
		
		Canvas->SetDrawColor(FColor::White);	

		Canvas->DrawTile(
			WeaponHUDConfig.WeaponIcon.Texture,
			EquippedX + EquippedPixelsPerWidth  * WeaponElement.EquippedWeapon.MainIconTransform.PercentX,
			EquippedY + EquippedPixelsPerHeight * WeaponElement.EquippedWeapon.MainIconTransform.PercentY,
			EquippedPixelsPerWidth  * WeaponElement.EquippedWeapon.MainIconTransform.WidthPercent,
			EquippedPixelsPerHeight * WeaponElement.EquippedWeapon.MainIconTransform.HeightPercent,
			WeaponHUDConfig.WeaponIcon.U,  WeaponHUDConfig.WeaponIcon.V,
			WeaponHUDConfig.WeaponIcon.UL, WeaponHUDConfig.WeaponIcon.VL,
			EBlendMode::BLEND_Translucent);
	}

	if (OffHandWeapon)
	{
		// Offhand weapon box.
		const float OffHandX = X + PixelsPerWidth  * WeaponElement.OffHandWeapon.Transform.PercentX;
		const float OffHandY = Y + PixelsPerHeight * WeaponElement.OffHandWeapon.Transform.PercentY;
		const float OffHandPixelsPerWidth  = PixelsPerWidth  * WeaponElement.OffHandWeapon.Transform.WidthPercent  * .01f;
		const float OffHandPixelsPerHeight = PixelsPerHeight * WeaponElement.OffHandWeapon.Transform.HeightPercent * .01f;

		FCGWeaponHUDData WeaponHUDConfig = OffHandWeapon->WeaponHUDConfig;

		// Draw Background.
		DrawRect(
			WeaponElement.OffHandWeapon.ElementBackgroundColor,
			OffHandX, OffHandY,
			PixelsPerWidth  * WeaponElement.OffHandWeapon.Transform.WidthPercent, 
			PixelsPerHeight * WeaponElement.OffHandWeapon.Transform.HeightPercent
		);

		// TODO weapon color?
		Canvas->SetDrawColor(FColor::White);

		Canvas->DrawTile(
			WeaponHUDConfig.WeaponIcon.Texture,
			OffHandX + OffHandPixelsPerWidth  * WeaponElement.OffHandWeapon.IconTransform.PercentX,
			OffHandY + OffHandPixelsPerHeight * WeaponElement.OffHandWeapon.IconTransform.PercentY,
			OffHandPixelsPerWidth  * WeaponElement.OffHandWeapon.IconTransform.WidthPercent,
			OffHandPixelsPerHeight * WeaponElement.OffHandWeapon.IconTransform.HeightPercent,
			WeaponHUDConfig.WeaponIcon.U, WeaponHUDConfig.WeaponIcon.V,
			WeaponHUDConfig.WeaponIcon.UL, WeaponHUDConfig.WeaponIcon.VL,
			EBlendMode::BLEND_Translucent);

		if (OffHandWeapon->WeaponConfig.bOverHeatWeapon)
		{
			// DO something else
		}
		else
		{
			// Ammo held.
			FString Text = FString::Printf(TEXT("%4d"), OffHandWeapon->GetAmmoInClip() + OffHandWeapon->GetAmmo());
			DrawScaledText(
				Text,
				WeaponElement.OffHandWeapon.Ammo.Color,
				OffHandX + OffHandPixelsPerWidth  * WeaponElement.OffHandWeapon.Ammo.Transform.PercentX,
				OffHandY + OffHandPixelsPerHeight * WeaponElement.OffHandWeapon.Ammo.Transform.PercentY,
				Font,
				OffHandPixelsPerHeight * WeaponElement.OffHandWeapon.Ammo.Transform.HeightPercent,
				WeaponElement.OffHandWeapon.Ammo.Anchor);
		}
	}
}

void ACGPlayerHUD::DrawShield()
{
	ACGCharacter* Pawn = Cast<ACGCharacter>(GetOwningPawn());
	
	const FCGHUDTransform Transform = Shield.Transform;
	float Percent = 0.f;
	float FlashFactor = 0.f;

	if (Pawn)
	{
		Percent = Pawn->GetShieldPercent();

		if (Percent <= Pawn->GetWarningShieldPercent())
		{
			const float FlashAdjustment = (1 - Percent / Pawn->GetWarningShieldPercent());

			// At most flash at half speed.
			Shield.FlashTime += GetWorld()->GetDeltaSeconds() * Shield.FlashRate * FMath::Max(.5f, FlashAdjustment);

			// Use a Sine Wave for a smooth flash.
			FlashFactor = ((FMath::Sin(Shield.FlashTime) + 1) * .5f) * FlashAdjustment;
		}
	}

	// TODO Flashing when health low.
	DrawRect(
		FMath::Lerp(Shield.BackgroundColor, Shield.BackgroundFlashColor, FlashFactor),
		PixelsPerCent.X * Transform.PercentX,
		PixelsPerCent.Y * Transform.PercentY,
		PixelsPerCent.X * Transform.WidthPercent,
		PixelsPerCent.Y * Transform.HeightPercent
	);

	if (Pawn)
	{
		DrawRect(
			FMath::Lerp(Shield.ShieldColor, Shield.ForegroundFlashColor, FlashFactor),
			PixelsPerCent.X * Transform.PercentX,
			PixelsPerCent.Y * Transform.PercentY,
			PixelsPerCent.X * Transform.WidthPercent * Percent,
			PixelsPerCent.Y * Transform.HeightPercent );
	}


	//
	
}
// TODO make this code cleaner.
void ACGPlayerHUD::DrawDamageIndicators()
{
	const float ScaleY = Canvas->ClipY / TARGET_Y_RESOLUTION;
	const FVector2D Size(ScaleY * HTIndicatorSize.X, ScaleY * HTIndicatorSize.Y);
	const FVector2D Position(0.5f * (Canvas->ClipX - Size.X), .5f*(Canvas->ClipY - Size.Y));
	const float DeltaTime = GetWorld()->GetTimeSeconds() - TimeSinceLastHitTaken;

	for (int32 i = 0; i < DamageIndicators.Num(); ++i)
	{
		// If the indicator exists, draw it.
		if (DamageIndicators[i].FadeTime > 0.f)
		{
			DamageIndicators[i].FadeTime -= DeltaTime;

			FLinearColor Color = HTIndicatorColor;
			Color.A = DamageIndicators[i].FadeTime / HTIndicatorFadeTime;

			FCanvasTileItem TileItem(Position, HTIndicatorIcon->Resource, Size, Color);
			TileItem.PivotPoint = FVector2D(0.5, 0.5);
			TileItem.Rotation = FRotator(0, DamageIndicators[i].Rotation, 0);
			TileItem.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;
			Canvas->DrawItem(TileItem);
		}
	}

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

		// TODO write a new scaled text.
		DrawScaledText(
			Text,
			RoundDataElement.TimeText.Color,
			X + PixelsPerWidth * RoundDataElement.TimeText.Transform.PercentX,
			Y + PixelsPerHeight * RoundDataElement.TimeText.Transform.PercentY,
			Font,
			PixelsPerHeight * RoundDataElement.TimeText.Transform.HeightPercent,
			RoundDataElement.TimeText.Anchor);

		Text = FString::Printf(TEXT("%s"), *CGGameState->GameModeClass->GetName());

		// TODO write a new scaled text.
		DrawScaledText(
			Text,
			RoundDataElement.TimeText.Color,
			X,
			Y + PixelsPerHeight * RoundDataElement.TimeText.Transform.PercentY - PixelsPerHeight * RoundDataElement.TimeText.Transform.HeightPercent,
			Font,
			PixelsPerHeight * RoundDataElement.TimeText.Transform.HeightPercent,
			0.f);

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

		for (int32 i = 0; i < NumElements; ++i)
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

			//Canvas->SetDrawColor(RoundDataElement.DataElementColor);

			const float ElemX = X + PixelsPerWidth * TempElement.Transform.PercentX;
			const float ElemY = Y + PixelsPerHeight * TempElement.Transform.PercentY;
			const float ElemW = PixelsPerWidth * TempElement.Transform.WidthPercent;
			const float ElemH = PixelsPerHeight * TempElement.Transform.HeightPercent;

			DrawRect(
				RoundDataElement.DataBackgroundColor, 
				ElemX, ElemY,
				ElemW, ElemH );

			DrawRect(
				RoundDataElement.DataForegroundColor,
				ElemX, ElemY,
				ElemW * TempPlayerState->Score / CGGameState->GoalScore, ElemH );
			
			// TODO clean this up
			// Draw the score.
			const float ScoreBoxX = ElemX + ElemW + RoundDataElement.ScoreBox.PercentX * PixelsPerWidth;
			const float ScoreBoxWidth = RoundDataElement.ScoreBox.WidthPercent * PixelsPerWidth;
			const float ScoreBoxPixelsWidth = ScoreBoxWidth * .01f;

			DrawRect(
				RoundDataElement.DataBackgroundColor,
				ScoreBoxX, ElemY,
				ScoreBoxWidth, ElemH);

			Text = FString::Printf(TEXT("%.0f"), TempPlayerState->Score);
			DrawScaledText(
				Text, 
				RoundDataElement.ScoreText.Color, 
				ScoreBoxX + ScoreBoxPixelsWidth * RoundDataElement.ScoreText.Transform.PercentX,
				ElemY + PixelsPerHeight * RoundDataElement.ScoreText.Transform.PercentY,
				Font, PixelsPerHeight * RoundDataElement.ScoreText.Transform.HeightPercent,
				RoundDataElement.ScoreText.Anchor);
			
	


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
	// XXX Maybe make this a power of two?
	const float Scale = TextHeight / SizeY;
	TextItem.Scale.Set(Scale, Scale);

	// Anchor offset. TODO Fix DesiredWidth, it doesn't work quite right.
	ScreenX = ScreenX - (SizeX * Scale * Anchor);

	// TODO this Jitters slightly.
	Canvas->DrawItem(TextItem, ScreenX, ScreenY);

	return SizeX * Scale;
}

void ACGPlayerHUD::DrawPrompt()
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
		Canvas->StrLen(Font, Prompt.BasePrompt + Prompt.PromptMessage, SizeX, SizeY);
		const float Scale = Height / SizeY;

		SizeX = (SizeX * Scale) + PixelsPerCent.X * Prompt.PromptKeyOffset + bDisplayButton * (PixelsPerCent.X * Prompt.PromptKeyOffset + Height);
		X -= SizeX * Prompt.Anchor;
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	float CurrentX = DrawScaledText(
		Prompt.BasePrompt,
		Prompt.PromptTextColor,
		X, Y,
		Font,
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
		Font,
		Height);
	
}

void ACGPlayerHUD::DrawDialog()
{

}

void ACGPlayerHUD::SetPromptMessage(bool bSetPrompt, const FString& Message, int32 ButtonID)
{
	Prompt.PromptMessage = Message;
	Prompt.bPrompt = bSetPrompt;
	Prompt.CurrentButton = ButtonID;
}

void ACGPlayerHUD::AddDialogKillMessage(ACGPlayerState* Killer, ACGPlayerState* KilledPlayer, const UDamageType* DamageType)
{
	FCGKillMessage NewKillMessage;
	if (Killer)
		NewKillMessage.KillerName = Killer->GetShortenedName();
	if (KilledPlayer)
		NewKillMessage.VictimName = KilledPlayer->GetShortenedName();


	DialogQueue.Add(NewKillMessage);

	UE_LOG(LogTemp, Warning, TEXT("%s Killed %s"), *NewKillMessage.KillerName, *NewKillMessage.VictimName);

}

void ACGPlayerHUD::AddDialogGameScoreMessage(int32 MessageIndex)
{
	ACGGameState* const CGGameState = GetWorld()->GetGameState<ACGGameState>();

	if (CGGameState)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, 
			CGGameState->GetScoreMessageText(MessageIndex));
	}
}

void ACGPlayerHUD::NotifyHitTaken(APawn* DamageCauser)
{
	float TempTime = GetWorld()->GetTimeSeconds();
	TimeSinceLastHitTaken = TempTime - TimeSinceLastHitTaken > TimeToDisplayHitTaken ? TempTime : TimeSinceLastHitTaken;

	AController* const Controller = GetOwningPlayerController();

	// If the source exists add an indicator.
	if (Controller && DamageCauser && DamageIndicators.Num() > 0)
	{
		//  Get the player location and look direction.
		FVector  ControllerOrigin;
		FRotator ControllerLookRotation;
		Controller->GetPlayerViewPoint(ControllerOrigin, ControllerLookRotation);

		// Find the source of the hit and normalize it.
		FVector SourceDir = (DamageCauser->GetActorLocation() - ControllerOrigin).GetSafeNormal2D();

		// The direction the player is looking.
		FVector LookDir = ControllerLookRotation.Vector().GetSafeNormal2D();
		
		// Copute the right vector for the LookDir
		FVector Right = LookDir ^ FVector::UpVector;

		// Perform the dot product.
		float Rotation = FMath::Acos(SourceDir | LookDir);

		// Determine if left or right.
		Rotation *= ((SourceDir | Right) > 0 ? -1 : 1);
		
		// Prep the indicator.
		float ShortestRemainingTime = DamageIndicators[0].FadeTime;
		int32 BestIndicator = 0;

		// Iterate over the indicators.
		for (int32 i = 0; i < DamageIndicators.Num(); ++i)
		{
			if ( DamageIndicators[i].FadeTime <= 0)
			{
				BestIndicator = i;
				break;
			}
			else if (DamageIndicators[i].FadeTime < ShortestRemainingTime)
			{
				ShortestRemainingTime = DamageIndicators[i].FadeTime;
				BestIndicator = i;
			}			
		}

		// Set Up the indicator for drawing.
		DamageIndicators[BestIndicator].FadeTime = HTIndicatorFadeTime;
		DamageIndicators[BestIndicator].Rotation = Rotation * (180 / PI);
	}
}

void ACGPlayerHUD::NotifyHitConfirmed()
{
	TimeSinceLastHitConfirmed = GetWorld()->GetTimeSeconds();
}

void ACGPlayerHUD::DrawScoreboard()
{
	ACGGameState* const CGGameState = GetWorld()->GetGameState<ACGGameState>();
	if (CGGameState)
	{
		const float Width =  PixelsPerCent.X * Scoreboard.Transform.WidthPercent;
		const float X = (PixelsPerCent.X * Scoreboard.Transform.PercentX) - Scoreboard.Alignment * Width;
		const float Y = PixelsPerCent.Y * Scoreboard.Transform.PercentY;

		const float PercentWidth = Width * .01f;
		const float PercentHeight = PixelsPerCent.Y * Scoreboard.Transform.HeightPercent * .01f;

		// Compute the row height from the supplied transform.
		const float RowHeight = PercentHeight * Scoreboard.RowHeightPercent;
		const float RowSpacing = PercentHeight * Scoreboard.RowOffsetPercent + RowHeight;

		const float RankWidth = Scoreboard.DesiredRankWidth  * PercentWidth;
		const float NameWidth = Scoreboard.DesiredNameWidth  * PercentWidth;
		const float ScoreWidth = Scoreboard.DesiredScoreWidth * PercentWidth;
		const float ColOffset = Scoreboard.ColumnOffsetPercent  * PercentWidth;


		// Tracks where the 
		float CurrentX = X;
		float CurrentY = Y;

		// Draw the background of the header.
		DrawRect( Scoreboard.HeaderBackgroundColor,
			CurrentX, CurrentY,
			Width, RowHeight);

		CurrentX += ColOffset;
		DrawScaledText(
			"Rank",
			Scoreboard.TextColor,
			CurrentX + RankWidth*Scoreboard.RankAlignment, Y, Font,
			RowHeight,
			Scoreboard.RankAlignment);
		CurrentX += RankWidth + ColOffset;

		// TODO add elipses to name.
		DrawScaledText(
			"Name",
			Scoreboard.TextColor,
			CurrentX + NameWidth * Scoreboard.NameAlignment, Y, Font,
			RowHeight,
			Scoreboard.NameAlignment);
		CurrentX += NameWidth + ColOffset;

		DrawScaledText(
			"Score",
			Scoreboard.TextColor,
			CurrentX + ScoreWidth * Scoreboard.ScoreAlignment, Y, Font,
			RowHeight,
			Scoreboard.ScoreAlignment);

		const int32 NumPlayers = CGGameState->PlayerArray.Num();
		ACGPlayerState* TempPlayerState;

		AController* const Controller = GetOwningPlayerController();
		APlayerState* const PlayerState = Controller ? Controller->PlayerState : NULL;

		CurrentY += RowSpacing;

		// Rank of the current player
		int32 Rank = 1;

		// Last player score.
		float PrevScore = FLT_MAX;

		for (int32 i = 0; i < NumPlayers; ++i)
		{
			TempPlayerState = Cast<ACGPlayerState>(CGGameState->PlayerArray[i]);
			CurrentX = X;

			DrawRect(
				TempPlayerState != PlayerState ? Scoreboard.RowBackgroundColor : Scoreboard.OwnerBackgroundColor,
				CurrentX, CurrentY,
				Width, RowHeight);

			// Fudge factor to make sure the column is not reight on the edge.
			CurrentX += ColOffset;

			// If our rank is less than the previous score, set the rank to the current index + 1 
			if (TempPlayerState->Score  < PrevScore)
			{
				Rank = i + 1;
			}

			// Cache the score.
			PrevScore = TempPlayerState->Score;

			DrawScaledText(
				FString::Printf(TEXT("%d"), Rank),
				Scoreboard.TextColor,
				CurrentX + RankWidth*Scoreboard.RankAlignment, CurrentY, Font,
				RowHeight,
				Scoreboard.RankAlignment);
			CurrentX += RankWidth + ColOffset;

			DrawScaledText(
				TempPlayerState->GetShortenedName(),
				Scoreboard.TextColor,
				CurrentX + NameWidth * Scoreboard.NameAlignment, CurrentY, Font,
				RowHeight,
				Scoreboard.NameAlignment);
			CurrentX += NameWidth + ColOffset;

			DrawScaledText(
				FString::Printf(TEXT("%.0f"), TempPlayerState->Score),
				Scoreboard.TextColor,
				CurrentX + ScoreWidth * Scoreboard.ScoreAlignment, CurrentY, Font,
				RowHeight,
				Scoreboard.ScoreAlignment);

			CurrentY += RowSpacing;
		}
	}
}

void ACGPlayerHUD::SetEndGameMessage(bool bSetMessage, const FString& Message)
{
	EndGameMessage.bDisplay = bSetMessage;
	EndGameMessage.Message = Message;
}

/** Draws the prompt message.*/
void ACGPlayerHUD::DrawEndGameMessage()
{
	const float X = PixelsPerCent.X * EndGameMessage.Transform.PercentX;
	const float Y = PixelsPerCent.Y * EndGameMessage.Transform.PercentY;
	const float Height = PixelsPerCent.Y * EndGameMessage.Transform.HeightPercent;

	DrawScaledText(
		EndGameMessage.Message,
		EndGameMessage.TextColor,
		X, Y, EndGameMessage.Font,
		Height,
		EndGameMessage.Alignment);
}
