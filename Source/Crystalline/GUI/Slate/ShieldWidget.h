// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GUI/CGPlayerHUD.h"
#include "Slate.h"


/**
 * 
 */
class CRYSTALLINE_API SShieldWidget : public SCompoundWidget
{
	// Bind Args
	SLATE_BEGIN_ARGS(SShieldWidget)
	{}
	SLATE_ARGUMENT(TWeakObjectPtr<class ACGPlayerHUD>, OwnerHUD)
	SLATE_END_ARGS()

public:

	// Constructs the Widget.
	void Construct(const FArguments& InArgs);

private:
	// Weak Pointer to the owner of this widget.
	TWeakObjectPtr<class ACGPlayerHUD> OwnerHUD;


};
