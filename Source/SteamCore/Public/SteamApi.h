// Fill out your copyright notice in the Description page of Project Settings.

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
