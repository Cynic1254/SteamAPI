#pragma once

#include "IInputDevice.h"
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

	void SetVibration(int32 ControllerId, const FForceFeedbackValues& Values);
private:
	struct FControllerState
	{
		/** Analog status for all actions from previous frame, on a -1.0 to 1.0 range */
		TMap<FName, ControllerAnalogActionData_t> AnalogStatusMap;

		/** Button status for all actions from previous frame (pressed down or not) */
		TMap<FName, bool> DigitalStatusMap;

		/** List of times that if a button is still pressed counts as a "repeated press" */
		TMap<FName, double> DigitalRepeatTimeMap;

		/** Values for force feedback on this controller.  We only consider the LEFT_LARGE channel for SteamControllers */
		FForceFeedbackValues VibeValues;

		FControllerState() = default;
	};
	
	FControllerState ControllerStates[STEAM_INPUT_MAX_COUNT];

	bool bControllerInitialized = false;
	TSharedRef<FGenericApplicationMessageHandler> MessageHandler;
	double InitialButtonRepeatDelay = 0.2;
	double ButtonRepeatDelay = 0.1;

	void ProcessControllerInput(int32 ControllerIndex, InputHandle_t ControllerHandle, FControllerState& State);
	void ProcessDigitalAction(int32 ControllerIndex, InputHandle_t ControllerHandle, const FName& ActionName, const FSteamInputAction& ActionData, FControllerState& State);
	void ProcessAnalogAction(int32 ControllerIndex, InputHandle_t ControllerHandle, const FName& ActionName, const FSteamInputAction& ActionData, FControllerState& State);

	bool ShouldProcessKeyRepeat(const FName& ActionName, FControllerState& State, double CurrentTime) const;
	void UpdateKeyRepeatTiming(const FName& ActionName, FControllerState& State, double CurrentTime) const;
	FInputDeviceId GetInputDeviceId(int32 ControllerIndex) const;
	FPlatformUserId GetPlatformUserId(int32 ControllerIndex) const;
};
