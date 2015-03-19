// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "ShieldWidget.h"

void SShieldWidget::Construct(const FArguments& InArgs)
{
	OwnerHUD = InArgs._OwnerHUD;

	// TODO SAMPLE CODE!
	ChildSlot
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.ShadowColorAndOpacity(FLinearColor::Black)
				.ColorAndOpacity(FLinearColor::White)
				.ShadowOffset(FIntPoint(-1, 1))
				.Font(FSlateFontInfo("Veranda", 16))
				.Text(FText::FromString("Hello, Slate!"))
			]
		];
}
