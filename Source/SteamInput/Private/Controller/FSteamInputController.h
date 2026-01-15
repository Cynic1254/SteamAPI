// Copyright 2026 Cynic. All Rights Reserved.
// Licensed under the Apache License, Version 2.0.
// See LICENSE file in the project root for full license information.

#pragma once

#include "IInputDevice.h"
#include "SteamInputTypes.h"
#include "GenericPlatform/IInputInterface.h"
#include "steam/isteamcontroller.h"

struct FSteamInputAction;

class FSteamInputController : public IInputDevice
{
public:
	FSteamInputController(const TSharedRef< FGenericApplicationMessageHandler>& InMessageHandler);
	virtual ~FSteamInputController() override;
	virtual void SendControllerEvents() override;
	virtual void Tick(float DeltaTime) override {}
	virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override;
	virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues& Values) override;
	virtual void SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;
	virtual bool IsGamepadAttached() const override;
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;

	void SetVibration(int32 ControllerId, const FForceFeedbackValues& Values) const;
private:
	struct FControllerState
	{
		/** Analog status for all actions from previous frame, on a -1.0 to 1.0 range */
		TMap<FName, ControllerAnalogActionData_t> AnalogStatusMap{};

		/** Button status for all actions from previous frame (pressed down or not) */
		TMap<FName, bool> DigitalStatusMap{};

		/** List of times that if a button is still pressed counts as a "repeated press" */
		TMap<FName, double> DigitalRepeatTimeMap{};

		/** Values for force feedback on this controller.  We only consider the LEFT_LARGE channel for SteamControllers */
		FForceFeedbackValues VibeValues{};

		enum EConnectionState
		{
			Disconnected,
			Connected,
			Reconnect,
		} ConnectionState = Disconnected;

		FControllerState() = default;
	};
	
	TMap<FInputHandle, FControllerState> ControllerStates;

	bool bControllerInitialized = false;
	TSharedRef<FGenericApplicationMessageHandler> MessageHandler;
	double InitialButtonRepeatDelay = 0.2;
	double ButtonRepeatDelay = 0.1;

	void ProcessControllerInput(const FInputHandle& ControllerHandle, FControllerState& State);
	void ProcessDigitalAction(FPlatformUserId UserID, FInputDeviceId DeviceId, const FInputHandle& ControllerHandle, const FName& ActionName, const FSteamInputAction& ActionData, FControllerState& State) const;
	void ProcessAnalogAction(FPlatformUserId UserID, FInputDeviceId DeviceId, const FInputHandle& ControllerHandle, const FName& ActionName, const FSteamInputAction& ActionData, FControllerState& State) const;

	void UpdateControllerState(const InputHandle_t* ConnectedControllers, int32 Count);
	void GetPlatformUserAndDevice(FInputHandle InputHandle, FPlatformUserId& OutUserID, FInputDeviceId& OutDeviceId);
	bool ShouldProcessKeyRepeat(const FName& ActionName, FControllerState& State, double CurrentTime) const;
	void UpdateKeyRepeatTiming(const FName& ActionName, FControllerState& State, double CurrentTime) const;
};
