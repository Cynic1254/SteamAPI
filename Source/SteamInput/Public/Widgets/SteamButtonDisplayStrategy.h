// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SteamInputTypes.h"
#include "UObject/Object.h"
#include "SteamButtonDisplayStrategy.generated.h"

/**
 * 
 */
UCLASS()
class STEAMINPUT_API USteamButtonDisplayStrategy : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Steam|Input")
	FSlateBrush CreatePromptBrush(const TArray<FSteamInputActionOrigin>& ActionOrigins, const FSlateBrush& FallbackBrush);
};
