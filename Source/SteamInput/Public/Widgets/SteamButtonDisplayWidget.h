// Copyright 2026 Cynic. All Rights Reserved.
// Licensed under the Apache License, Version 2.0.
// See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "SteamButtonDisplayWidget.generated.h"

class SSteamButtonDisplay;
class USteamButtonDisplayStrategy;
/**
 * 
 */
UCLASS(Category = "Steam|Input")
class STEAMINPUT_API USteamButtonDisplayWidget : public UWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Steam|Input")
	void SetActionName(FName NewActionName);
	UFUNCTION(BlueprintCallable, Category = "Steam|Input")
	void SetUserId(FPlatformUserId NewUserId);
	UFUNCTION(BlueprintCallable, Category = "Steam|Input")
	void SetDisplayStrategy(USteamButtonDisplayStrategy* NewDisplayStrategy);
	UFUNCTION(BlueprintCallable, Category = "Steam|Input")
	void SetFallbackBrush(const FSlateBrush& NewFallbackBrush);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter = SetActionName, Category = "Steam|Input", meta = (GetOptions = "SteamInput.SteamInputSettings.GetKeyList"))
	FName ActionName;
	UPROPERTY(EditAnywhere, Category = "Steam|Input", meta = (ClampMin = "0", ClampMax = "7"))
	int32 PlayerIndex = 0;
	UPROPERTY(BlueprintReadWrite, BlueprintSetter = SetUserId, Category = "Steam|Input")
	FPlatformUserId UserId;
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, BlueprintSetter = SetDisplayStrategy, Category = "Steam|Input")
	USteamButtonDisplayStrategy* DisplayStrategy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter = SetFallbackBrush, Category = "Steam|Input")
	FSlateBrush FallbackBrush;
	
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
private:
	TSharedPtr<SSteamButtonDisplay> ButtonDisplay;
};
