// Copyright 2026 Cynic. All Rights Reserved.
// Licensed under the Apache License, Version 2.0.
// See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SteamApi.generated.h"

/**
 * 
 */
UCLASS()
class STEAMCORE_API USteamApi : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Steam")
	static bool RestartAppIfNecessary(int AppID);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Steam")
	static int GetAppID();
private:
	static void RequestGameExit();
};
