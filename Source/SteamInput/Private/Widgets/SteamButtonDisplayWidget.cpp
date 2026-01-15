// Copyright 2026 Cynic. All Rights Reserved.
// Licensed under the Apache License, Version 2.0.
// See LICENSE file in the project root for full license information.


#include "Widgets/SteamButtonDisplayWidget.h"

#include "Widgets/SteamButtonDisplay.h"

void USteamButtonDisplayWidget::SetActionName(FName NewActionName)
{
	ActionName = NewActionName;
	if (ButtonDisplay.IsValid())
	{
		ButtonDisplay->SetActionName(ActionName);
	}
}

void USteamButtonDisplayWidget::SetUserId(FPlatformUserId NewUserId)
{
	UserId = NewUserId;
	if (ButtonDisplay.IsValid())
	{
		ButtonDisplay->SetPlatformUserId(UserId);
	}
}

void USteamButtonDisplayWidget::SetDisplayStrategy(USteamButtonDisplayStrategy* NewDisplayStrategy)
{
	DisplayStrategy = NewDisplayStrategy;
	if (ButtonDisplay.IsValid())
	{
		ButtonDisplay->SetStrategy(DisplayStrategy);
	}
}

void USteamButtonDisplayWidget::SetFallbackBrush(const FSlateBrush& NewFallbackBrush)
{
	FallbackBrush = NewFallbackBrush;
	if (ButtonDisplay.IsValid())
	{
		ButtonDisplay->SetFallbackBrush(FallbackBrush);
	}
}

TSharedRef<SWidget> USteamButtonDisplayWidget::RebuildWidget()
{
	if (!DisplayStrategy)
	{
		DisplayStrategy = NewObject<USteamButtonDisplayStrategy>(this);
	}

	if (!UserId.IsValid())
	{
		UserId = FPlatformMisc::GetPlatformUserForUserIndex(PlayerIndex);
	}
	
	ButtonDisplay = SNew(SSteamButtonDisplay)
		.ActionName(ActionName)
		.PlatformUserId(UserId)
		.Strategy(DisplayStrategy)
		.FallbackBrush(FallbackBrush);
	
	return ButtonDisplay.ToSharedRef();
}

void USteamButtonDisplayWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	
	if (!UserId.IsValid())
	{
		UserId = FPlatformMisc::GetPlatformUserForUserIndex(PlayerIndex);
	}
	
	if (ButtonDisplay.IsValid())
	{
		ButtonDisplay->SetActionName(ActionName);
		ButtonDisplay->SetPlatformUserId(UserId);
		ButtonDisplay->SetStrategy(DisplayStrategy);
	}
}

void USteamButtonDisplayWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	
	ButtonDisplay.Reset();
}
