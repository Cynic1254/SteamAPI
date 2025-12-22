// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/USteamDebugSubsystem.h"
#include "Engine/Engine.h"
#include "Framework/Application/SlateApplication.h"
#include "Input/Events.h"
#include "InputCoreTypes.h"

UUSteamDebugSubsystem* UUSteamDebugSubsystem::Get()
{
	return GEngine->GetEngineSubsystem<UUSteamDebugSubsystem>();
}

void UUSteamDebugSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor, EInputPreProcessorType::Overlay);
}

void UUSteamDebugSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);
}

bool UUSteamDebugSubsystem::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	PressedKeys.Add(InKeyEvent.GetKey().GetFName().ToString() + FString::FromInt(InKeyEvent.GetInputDeviceId().GetId()));
	return false;
}

bool UUSteamDebugSubsystem::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	PressedKeys.Remove(InKeyEvent.GetKey().GetFName().ToString() + FString::FromInt(InKeyEvent.GetInputDeviceId().GetId()));
	return false;
}

bool UUSteamDebugSubsystem::HandleAnalogInputEvent(FSlateApplication& SlateApp,
	const FAnalogInputEvent& InAnalogInputEvent)
{
	AnalogKeyState.FindOrAdd(InAnalogInputEvent.GetKey().GetFName().ToString() + FString::FromInt(InAnalogInputEvent.GetInputDeviceId().GetId())) = InAnalogInputEvent.GetAnalogValue();
	return false;
}

bool UUSteamDebugSubsystem::HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	FKey MouseButton = MouseEvent.GetEffectingButton();
	return false;
}

bool UUSteamDebugSubsystem::IsKeyPressed(const FKey& Key, FInputDeviceId ControllerHandle) const
{
	return PressedKeys.Contains(Key.ToString() + FString::FromInt(ControllerHandle.GetId()));
}

float UUSteamDebugSubsystem::GetKeyValue(const FKey& Key, FInputDeviceId ControllerHandle) const
{
	return AnalogKeyState.Contains(Key.ToString() + FString::FromInt(ControllerHandle.GetId())) ?
	AnalogKeyState[Key.ToString() + FString::FromInt(ControllerHandle.GetId())] :
	0.0f;
}