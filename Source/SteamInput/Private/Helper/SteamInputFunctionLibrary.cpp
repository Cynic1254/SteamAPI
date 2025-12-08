// Fill out your copyright notice in the Description page of Project Settings.


#include "Helper/SteamInputFunctionLibrary.h"

#include "SteamInputCache.h"
#include "Controller/FSteamInputController.h"
#include "Settings/SteamInputSettings.h"

TMap<FName, InputActionSetHandle_t> USteamInputFunctionLibrary::CachedHandles = {};
TMap<FInputDeviceId, InputActionSetHandle_t> USteamInputFunctionLibrary::ActiveActionSet = {};
TMap<FInputDeviceId, TArray<InputActionSetHandle_t>> USteamInputFunctionLibrary::ActionSetLayers = {};

TInputDeviceMap<uint64> USteamInputFunctionLibrary::DeviceMappings = {};

FInputHandle USteamInputFunctionLibrary::GetHandleFromID(const FInputDeviceId ControllerHandle)
{
	if (auto InputHandle = DeviceMappings.FindDeviceKey(ControllerHandle))
	{
		return *InputHandle;
	}
	return 0;
}

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

TArray<InputActionSetHandle_t>* USteamInputFunctionLibrary::GetActionLayersForController(const FInputDeviceId ControllerHandle)
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

bool USteamInputFunctionLibrary::IsSteamController(const FInputDeviceId DeviceId)
{
	return DeviceMappings.FindDeviceKey(DeviceId) != nullptr;
}

TArray<FSteamInputActionOrigin> USteamInputFunctionLibrary::GetInputActionOriginForCurrentActionSet(
	const FInputDeviceId ControllerHandle, const FControllerActionHandle ActionHandle)
{
	TArray<FSteamInputActionOrigin> Out = GetInputActionOrigin(ControllerHandle, GetActionSetForController(ControllerHandle), ActionHandle);

	if (const auto* Layers = GetActionLayersForController(ControllerHandle))
	{
		for (const auto ActionSet : *Layers)
		{
			for (FSteamInputActionOrigin ActionOrigin : GetInputActionOrigin(ControllerHandle, ActionSet, ActionHandle))
			{
				Out.AddUnique(ActionOrigin);
			}
		}
	}
	
	return Out;
}

TArray<FSteamInputActionOrigin> USteamInputFunctionLibrary::GetInputActionOrigin(const FInputDeviceId Controller,
                                                                                 const FInputActionSetHandle ActionSetHandle, const FControllerActionHandle ActionHandle)
{
	const InputHandle_t Handle = GetHandleFromID(Controller);
	if (Handle == 0 || !SteamInput())
	{
		return {};
	}
	
	EInputActionOrigin Origins[STEAM_INPUT_MAX_ORIGINS]{};
	int Count;

	switch (ActionHandle.GetType())
	{
	case ActionType::EAnalog:
		Count = SteamInput()->GetAnalogActionOrigins(Handle, ActionSetHandle, ActionHandle.GetAnalogActionHandle(), Origins);
		break;
	case ActionType::EDigital:
	default:
		Count = SteamInput()->GetDigitalActionOrigins(Handle, ActionSetHandle, ActionHandle.GetDigitalActionHandle(), Origins);
		break;
	}
	
	TArray<FSteamInputActionOrigin> Out;
	Out.Reserve(Count);
	for (int i = 0; i < Count; ++i)
	{
		Out.Emplace(static_cast<ESteamInputActionOrigin>(Origins[i]));
	}
	
	return Out;
}

UTexture2D* USteamInputFunctionLibrary::GetTextureFromActionOrigin(FSteamInputActionOrigin ActionOrigin)
{
	if (const auto TextureOverwrite = GetDefault<USteamInputSettings>()->ButtonTextureMapping.Find(ActionOrigin))
	{
		return TextureOverwrite->LoadSynchronous();
	}
	
	return USteamInputCache::Get()->GetGlyphTexture(SteamInput()->GetGlyphPNGForActionOrigin(static_cast<EInputActionOrigin>(ActionOrigin.ActionOrigin), k_ESteamInputGlyphSize_Large, 0));
}
