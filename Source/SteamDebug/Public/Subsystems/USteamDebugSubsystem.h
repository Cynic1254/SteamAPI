// Copyright 2026 Cynic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/IInputProcessor.h"
#include "Subsystems/EngineSubsystem.h"
#include "USteamDebugSubsystem.generated.h"

class FSteamInputProcessor;
struct FKey;

/**
 * Subsystem for hooking and managing various debug information that can be used by the debug tools
 */
UCLASS()
class STEAMDEBUG_API UUSteamDebugSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
public:
	static UUSteamDebugSubsystem* Get();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent);
	bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent);
	bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent);
	bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent);

	bool IsKeyPressed(const FKey& Key, FInputDeviceId ControllerHandle) const;
	float GetKeyValue(const FKey& Key, FInputDeviceId ControllerHandle) const;
private:
	TSharedPtr<FSteamInputProcessor> InputProcessor = MakeShared<FSteamInputProcessor>();
	TSet<FString> PressedKeys;
	TMap<FString, float> AnalogKeyState;
};

class FSteamInputProcessor : public IInputProcessor
{
public:
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
	{
		return UUSteamDebugSubsystem::Get()->HandleKeyDownEvent(SlateApp, InKeyEvent);
	}
	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
	{
		return UUSteamDebugSubsystem::Get()->HandleKeyUpEvent(SlateApp, InKeyEvent);
	}
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override
	{
		return UUSteamDebugSubsystem::Get()->HandleAnalogInputEvent(SlateApp, InAnalogInputEvent);
	}
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override {};
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		return UUSteamDebugSubsystem::Get()->HandleMouseMoveEvent(SlateApp, MouseEvent);
	}
};