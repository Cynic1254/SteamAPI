// Fill out your copyright notice in the Description page of Project Settings.


#include "Helper/SteamInputFunctionLibrary.h"

#include "Controller/FSteamInputController.h"

TMap<FName, InputActionSetHandle_t> USteamInputFunctionLibrary::CachedHandles = {};
TMap<FInputDeviceId, InputActionSetHandle_t> USteamInputFunctionLibrary::ActiveActionSet = {};
TMap<FInputDeviceId, TArray<InputActionSetHandle_t>> USteamInputFunctionLibrary::ActionSetLayers = {};

TInputDeviceMap<uint64> USteamInputFunctionLibrary::DeviceMappings = {};

void USteamInputFunctionLibrary::PushActionSetLayerByName(const FInputDeviceId ControllerHandle, const FName Name)
{
	PushActionSetLayer(ControllerHandle, GetActionHandle(Name));
}

void USteamInputFunctionLibrary::RemoveActionSetLayerByName(const FInputDeviceId ControllerHandle, const FName Name)
{
	RemoveActionSetLayer(ControllerHandle, GetActionHandle(Name));
}

void USteamInputFunctionLibrary::PushActionSetLayer(const FInputDeviceId ControllerHandle, const FInputActionSetHandle Handle)
{
	if (Handle == 0)
	{
		return;
	}
	
	auto& ActionLayers = ActionSetLayers.FindOrAdd(ControllerHandle);
	
	ActionLayers.Remove(Handle);
	ActionLayers.Add(Handle);
}

void USteamInputFunctionLibrary::RemoveActionSetLayer(const FInputDeviceId ControllerHandle, const FInputActionSetHandle Handle)
{
	if (const auto ActionLayers = ActionSetLayers.Find(ControllerHandle))
	{
		ActionLayers->Remove(Handle);
	}
}

TArray<InputActionSetHandle_t>* USteamInputFunctionLibrary::GetActionLayersForController(const FInputDeviceId& ControllerHandle)
{
	return ActionSetLayers.Find(ControllerHandle);
}

FInputActionSetHandle USteamInputFunctionLibrary::GetActionSetForController(const FInputDeviceId ControllerHandle)
{
	return ActiveActionSet.FindRef(ControllerHandle);
}

void USteamInputFunctionLibrary::ActivateActionSetByName(const FInputDeviceId ControllerHandle, const FName HandleName)
{
	ActivateActionSet(ControllerHandle, GetActionHandle(HandleName));
}

void USteamInputFunctionLibrary::ActivateActionSet(const FInputDeviceId ControllerHandle, const FInputActionSetHandle Handle)
{
	if (Handle == 0)
	{
		return;
	}
	
	ActiveActionSet.FindOrAdd(ControllerHandle) = Handle;
}

FInputActionSetHandle USteamInputFunctionLibrary::GetActionHandle(const FName Name)
{
	if (const InputActionSetHandle_t* Handle = CachedHandles.Find(Name))
	{
		return *Handle;
	}

	if (InputActionSetHandle_t Handle = SteamInput()->GetActionSetHandle(TCHAR_TO_UTF8(*Name.ToString())))
	{
		return CachedHandles.Add(Name, Handle);
	}

	return 0;
}

FName USteamInputFunctionLibrary::GetActionName(const FInputActionSetHandle Handle)
{
	if (const FName* Name = CachedHandles.FindKey(Handle))
	{
		return *Name;
	}
	
	return FName(*FString::Printf(TEXT("0x%016llX"), static_cast<InputActionSetHandle_t>(Handle)));
}

FInputDeviceId USteamInputFunctionLibrary::GetDeviceIDFromSteamID(const FInputHandle InputHandle)
{
	return DeviceMappings.FindDeviceId(InputHandle);
}
