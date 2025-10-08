// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SteamInputTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ActionSets.generated.h"

/**
 * 
 */
UCLASS()
class STEAMINPUT_API UActionSets : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static void PushActionSetLayer(int32 ControllerID, FName Handle);

	UFUNCTION(BlueprintCallable)
	static void RemoveActionSetLayer(int32 ControllerID, FName Handle);
	
	static TArray<FName>* GetActionLayersForController(int32 ControllerID);

	UFUNCTION(BlueprintCallable)
	static FInputActionSetHandle GetActionSetForController(int32 ControllerID);
	
	UFUNCTION(BlueprintCallable, DisplayName = "Activate Action Set (By Name)")
	static void ActivateActionSetByName(int32 ControllerID, FName HandleName);
	
	UFUNCTION(BlueprintCallable, DisplayName = "Activate Action Set (By Handle)")
	static void ActivateActionSet(int32 ControllerID, FInputActionSetHandle Handle);
	
	UFUNCTION(BlueprintCallable)
	static FInputActionSetHandle GetActionHandle(FName Name);
private:
	static TMap<FName, InputActionSetHandle_t> CachedHandles;

	static InputActionSetHandle_t ActiveActionSet[16 /*STEAM_INPUT_MAX_COUNT*/];
	static TArray<FName> ActionSetLayers[16 /*STEAM_INPUT_MAX_COUNT*/];
};
