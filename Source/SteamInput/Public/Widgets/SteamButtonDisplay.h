// Copyright 2026 Cynic. All Rights Reserved.
// Licensed under the Apache License, Version 2.0.
// See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "SteamButtonDisplayStrategy.h"
#include "Widgets/SCompoundWidget.h"

class SImage;

/**
 * 
 */
class STEAMINPUT_API SSteamButtonDisplay : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSteamButtonDisplay) :
		_ActionName(NAME_None)
		, _PlatformUserId(PLATFORMUSERID_NONE)
		, _Strategy(nullptr)
	{}
	SLATE_ARGUMENT(FName, ActionName)
	SLATE_ARGUMENT(FPlatformUserId, PlatformUserId)
	SLATE_ARGUMENT(USteamButtonDisplayStrategy*, Strategy)
	SLATE_ATTRIBUTE(FSlateBrush, FallbackBrush)
		
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	
	void SetActionName(FName InActionName);
	void SetPlatformUserId(FPlatformUserId InUserId);
	void SetStrategy(USteamButtonDisplayStrategy* InStrategy);
	void SetFallbackBrush(const FSlateBrush& SlateBrush);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
	FName ActionName;
	FPlatformUserId PlatformUserId;
	TWeakObjectPtr<USteamButtonDisplayStrategy> Strategy;
	TAttribute<FSlateBrush> FallbackBrush;
	
	// Cached data to detect changes
	FInputDeviceId CachedDeviceId;
	TArray<FSteamInputActionOrigin> CachedOrigins;
	uint32 LastOriginHash = 0;
	
	FSlateBrush CurrentBrush;
	TSharedPtr<SImage> ImageWidget;

	void RefreshPrompt();
	FInputDeviceId GetCurrentDeviceId() const;
	TArray<FSteamInputActionOrigin> GetOriginsForAction() const;
	static uint32 ComputeOriginHash(const TArray<FSteamInputActionOrigin>& Origins);
	const FSlateBrush* GetPromptBrush() const {return &CurrentBrush;}
};
