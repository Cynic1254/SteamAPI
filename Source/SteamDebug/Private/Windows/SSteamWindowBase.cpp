// Copyright 2026 Cynic. All Rights Reserved.

#include "Windows/SSteamWindowBase.h"

#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SSeparator.h"
#include "Styling/CoreStyle.h"

void SSteamWindowBase::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(this, &SSteamWindowBase::GetWindowTitle)
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSeparator)
		]

		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			ConstructContent()
		]
	];
}
