// Copyright 2026 Cynic. All Rights Reserved.
// Licensed under the Apache License, Version 2.0.
// See LICENSE file in the project root for full license information.


#include "Widgets/SteamButtonDisplay.h"

#include "SlateOptMacros.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "Helper/SteamInputFunctionLibrary.h"
#include "Widgets/Images/SImage.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SSteamButtonDisplay::Construct(const FArguments& InArgs)
{
	ActionName = InArgs._ActionName;
	PlatformUserId = InArgs._PlatformUserId;
	Strategy = InArgs._Strategy;
	FallbackBrush = InArgs._FallbackBrush;

	// Create default strategy if none provided
	if (!Strategy.IsValid())
	{
		Strategy = NewObject<USteamButtonDisplayStrategy>();
	}

	CachedDeviceId = GetCurrentDeviceId();
	
	ChildSlot
	[
		SAssignNew(ImageWidget, SImage)
		.Image(this, &SSteamButtonDisplay::GetPromptBrush)
	];

	RefreshPrompt();
}

void SSteamButtonDisplay::SetActionName(FName InActionName)
{
	ActionName = InActionName;
	RefreshPrompt();
}

void SSteamButtonDisplay::SetPlatformUserId(FPlatformUserId InUserId)
{
	PlatformUserId = InUserId;
	CachedDeviceId = GetCurrentDeviceId();
	RefreshPrompt();
}

void SSteamButtonDisplay::SetStrategy(USteamButtonDisplayStrategy* InStrategy)
{
	Strategy = InStrategy;
	RefreshPrompt();
}

void SSteamButtonDisplay::SetFallbackBrush(const FSlateBrush& SlateBrush)
{
	FallbackBrush = SlateBrush;
	RefreshPrompt();
}

void SSteamButtonDisplay::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	// Check if the device assigned to this user changed
	const FInputDeviceId CurrentDeviceId = GetCurrentDeviceId();
	if (CurrentDeviceId != CachedDeviceId)
	{
		CachedDeviceId = CurrentDeviceId;
		LastOriginHash = 0; // Force refresh
		RefreshPrompt();
		return;
	}
	
	// Check if origins changed (button remapping, controller change, etc.)
	const TArray<FSteamInputActionOrigin> CurrentOrigins = GetOriginsForAction();
	const uint32 CurrentHash = ComputeOriginHash(CurrentOrigins);

	if (CurrentHash != LastOriginHash)
	{
		LastOriginHash = CurrentHash;
		CachedOrigins = CurrentOrigins;
		RefreshPrompt();
	}
}

void SSteamButtonDisplay::RefreshPrompt()
{
	if (!Strategy.IsValid())
	{
		CurrentBrush = FallbackBrush.Get();
		return;
	}

	const TArray<FSteamInputActionOrigin> Origins = GetOriginsForAction();
	CurrentBrush = Strategy->CreatePromptBrush(Origins, FallbackBrush.Get());
}

FInputDeviceId SSteamButtonDisplay::GetCurrentDeviceId() const
{
	if (!PlatformUserId.IsValid())
	{
		return INPUTDEVICEID_NONE;
	}

	// Get the primary input device for this platform user
	const FInputDeviceId DeviceId = UInputDeviceSubsystem::Get()->GetMostRecentlyUsedInputDeviceId(PlatformUserId);
	
	// Only return it if it's a Steam controller
	if (DeviceId.IsValid() && USteamInputFunctionLibrary::IsSteamController(DeviceId))
	{
		return DeviceId;
	}

	return INPUTDEVICEID_NONE;
}

TArray<FSteamInputActionOrigin> SSteamButtonDisplay::GetOriginsForAction() const
{
	const FInputDeviceId DeviceId = GetCurrentDeviceId();
	
	if (!DeviceId.IsValid() || ActionName.IsNone())
	{
		return {};
	}

	const FControllerActionHandle ActionHandle = USteamInputFunctionLibrary::GetActionHandle(ActionName);
	if (ActionHandle.GetHandle() == 0)
	{
		return {};
	}

	return USteamInputFunctionLibrary::GetInputActionOriginForCurrentActionSet(DeviceId, ActionHandle);
}

uint32 SSteamButtonDisplay::ComputeOriginHash(const TArray<FSteamInputActionOrigin>& Origins)
{
	uint32 Hash = 0;
	for (const FSteamInputActionOrigin& Origin : Origins)
	{
		Hash = HashCombine(Hash, GetTypeHash(Origin.ActionOrigin));
	}
	return Hash;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
