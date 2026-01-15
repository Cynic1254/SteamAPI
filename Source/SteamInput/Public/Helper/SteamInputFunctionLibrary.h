// Copyright 2026 Cynic. All Rights Reserved.
// Licensed under the Apache License, Version 2.0.
// See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "SteamInputTypes.h"
#include "GenericPlatform/GenericInputDeviceMap.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SteamInputFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STEAMINPUT_API USteamInputFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/// Push an action set layer by name to the provided controller. This will put the layer at the top of the list even if it was already in the list
	/// @param ControllerHandle Device ID of the steam controller you want to push the action set layer to
	/// @param Name Name of the action set layer to push
	UFUNCTION(BlueprintCallable, Category = "Steam|Input|Action|Layers")
	static void PushActionLayerByName(FInputDeviceId ControllerHandle, FName Name);
	/// Push an action set layer by ID to the provided controller. This will put the layer at the top of the list even if it was already in the list
	/// @param ControllerHandle Device ID of the steam controller you want to push the action set layer to
	/// @param Handle Action set handle of the layer to push
	UFUNCTION(BlueprintCallable, Category = "Steam|Input|Action|Layers")
	static void PushActionLayer(FInputDeviceId ControllerHandle, FInputActionSetHandle Handle);

	/// Remove an action set layer by name from the provided controller.
	/// @param ControllerHandle Device ID of the steam controller you want to remove the layer fromm
	/// @param Name name of the action set layer to remove
	UFUNCTION(BlueprintCallable, Category = "Steam|Input|Action|Layers")
	static void RemoveActionLayerByName(FInputDeviceId ControllerHandle, FName Name);
	/// Remove an action set layer by ID from the provided controller.
	/// @param ControllerHandle Device ID of the steam controller you want to remove the layer fromm
	/// @param Handle Action set handle of the action set layer to remove
	UFUNCTION(BlueprintCallable, Category = "Steam|Input|Action|Layers")
	static void RemoveActionLayer(FInputDeviceId ControllerHandle, FInputActionSetHandle Handle);

	/// Get a list of the active layers for the controller
	/// @param ControllerHandle Device ID of the steam controller you want to get the layers from
	/// @return Array containing all the layers that are active on the controller
	static TArray<InputActionSetHandle_t>* GetActionLayersForController(FInputDeviceId ControllerHandle);

	/// Get the active action set for the controller
	/// @param ControllerHandle Device ID of the steam controller to get the active action set from
	/// @return The active action set for the controller
	UFUNCTION(BlueprintCallable, Category = "Steam|Input|Action|Set")
	static FInputActionSetHandle GetActionSetForController(FInputDeviceId ControllerHandle);

	/// Activate an action set by name for the controller
	/// @param ControllerHandle The controller to activate the action set for
	/// @param HandleName Name of the action set to activate
	UFUNCTION(BlueprintCallable, DisplayName = "Activate Action Set (By Name)", Category = "Steam|Input|Action|Set")
	static void ActivateActionSetByName(FInputDeviceId ControllerHandle, FName HandleName);
	/// Activate an action set by ID for the controller
	/// @param ControllerHandle The controller to activate the action set for
	/// @param Handle The handle of the action set to activate
	UFUNCTION(BlueprintCallable, DisplayName = "Activate Action Set (By Handle)", Category = "Steam|Input|Action|Set")
	static void ActivateActionSet(FInputDeviceId ControllerHandle, FInputActionSetHandle Handle);

	/// Translate the action set name into the ID, this caches the information for easy lookup later
	/// @param Name The name of the action set
	/// @return The Handle for the action set
	UFUNCTION(BlueprintCallable, Category = "Steam|Input|Action|Set")
	static FInputActionSetHandle GetActionSetHandle(FName Name);
	/// Translate the action set ID into it's name, for this to work GetActionHandle would need to have cached the information before
	/// @param Handle The handle of the action set
	/// @return The name of the action set if it's known, returns the ID in Hex otherwise
	UFUNCTION(BlueprintCallable, Category = "Steam|Input|Action|Set")
	static FName GetActionSetName(FInputActionSetHandle Handle);

	/// Translate the Steam Controller ID into the Unreal Engine Controller ID
	/// @param InputHandle The Hardware ID of the controller
	/// @return The Unreal Engine Device ID the controller is bound to
	UFUNCTION(BlueprintCallable, Category = "Steam|Input")
	static FInputDeviceId GetDeviceIDFromSteamID(FInputHandle InputHandle);

	/// Test if the provided controller is managed by the FSteamInputController
	/// @param ControllerHandle The Controller to test
	/// @return true if the controller is managed by steam, false otherwise
	UFUNCTION(BlueprintCallable, Category = "Steam|Input")
	static bool IsSteamController(FInputDeviceId ControllerHandle);

	/// Get the Input Origins for the input action using the currently active action set and action set layers
	/// @param ControllerHandle The Controller to get the origins off
	/// @param ActionHandle The action to get the origins off
	/// @return An array containing the origins
	UFUNCTION(BlueprintCallable, Category = "Steam|Input|Action")
	static TArray<FSteamInputActionOrigin> GetInputActionOriginForCurrentActionSet(FInputDeviceId ControllerHandle, FControllerActionHandle ActionHandle);
	/// Get the Input Origins for the input action using the provided action set
	/// @param ControllerHandle The Controller to get the origins off
	/// @param ActionSetHandle The action set to get the origins off
	/// @param ActionHandle The action to get the origins off
	/// @return An array containing the origins
	UFUNCTION(BlueprintCallable, Category = "Steam|Input|Action|Origin")
	static TArray<FSteamInputActionOrigin> GetInputActionOrigin(FInputDeviceId ControllerHandle, FInputActionSetHandle ActionSetHandle, FControllerActionHandle ActionHandle);
	/// Get the texture that is used for the action origin
	/// @param ActionOrigin Action origin to get the texture from
	/// @return The texture for the origin
	UFUNCTION(BlueprintCallable, Category = "Steam|Input|Action|Origin")
	static UTexture2D* GetTextureFromActionOrigin(FSteamInputActionOrigin ActionOrigin);
	/// Get an action handle from its name
	/// @param ActionName Name of the action to get the handle for
	/// @return The handle for the action, if the action doesn't exist will return an empty handle
	UFUNCTION(BlueprintCallable, Category = "Steam|Input|Action")
	static FControllerActionHandle GetActionHandle(const FName& ActionName);
private:
	static TMap<FName, InputActionSetHandle_t> CachedHandles;

	static TMap<FInputDeviceId, InputActionSetHandle_t> ActiveActionSet;
	static TMap<FInputDeviceId, TArray<InputActionSetHandle_t>> ActionSetLayers;
	
	static TInputDeviceMap<uint64> DeviceMappings;
	
	static FInputHandle GetHandleFromID(FInputDeviceId ControllerHandle);
	
	friend class FSteamInputController;
	friend class SInputMonitor;
};
