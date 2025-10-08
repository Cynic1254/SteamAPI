// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSets/ActionSets.h"

#include "Controller/FSteamInputController.h"

TMap<FName, InputActionSetHandle_t> UActionSets::CachedHandles = {};
InputActionSetHandle_t UActionSets::ActiveActionSet[16 /*STEAM_INPUT_MAX_COUNT*/] = {};
TArray<FName> UActionSets::ActionSetLayers[16 /*STEAM_INPUT_MAX_COUNT*/] = {};

void UActionSets::PushActionSetLayer(const int32 ControllerID, const FName Handle)
{
	if (ControllerID > (sizeof ActionSetLayers / sizeof(TArray<FName>)))
	{
		return;
	}
	
	ActionSetLayers[ControllerID].Remove(Handle);
	ActionSetLayers[ControllerID].Add(Handle);
}

void UActionSets::RemoveActionSetLayer(const int32 ControllerID, const FName Handle)
{
	if (ControllerID > (sizeof ActionSetLayers / sizeof(TArray<FName>)))
	{
		return;
	}
	
	ActionSetLayers[ControllerID].Remove(Handle);
}

TArray<FName>* UActionSets::GetActionLayersForController(const int32 ControllerID)
{
	if (ControllerID > (sizeof ActionSetLayers / sizeof(TArray<FName>)))
	{
		return nullptr;
	}
	
	return &ActionSetLayers[ControllerID];
}

FInputActionSetHandle UActionSets::GetActionSetForController(int32 ControllerID)
{
	if (ControllerID > (sizeof ActionSetLayers / sizeof(TArray<FName>)))
	{
		return {};
	}
	
	return ActiveActionSet[ControllerID];
}

void UActionSets::ActivateActionSetByName(const int32 ControllerID, const FName HandleName)
{
	ActivateActionSet(ControllerID, GetActionHandle(HandleName));
}

void UActionSets::ActivateActionSet(const int32 ControllerID, const FInputActionSetHandle Handle)
{
	if (ControllerID > (sizeof ActionSetLayers / sizeof(TArray<FName>)))
	{
		return;
	}
	
	ActiveActionSet[ControllerID] = Handle;
}

FInputActionSetHandle UActionSets::GetActionHandle(const FName Name)
{
	if (const InputActionSetHandle_t* Handle = CachedHandles.Find(Name))
	{
		return *Handle;
	}

	return CachedHandles.Add(Name, SteamInput()->GetActionSetHandle(TCHAR_TO_UTF8(*Name.ToString())));
}
