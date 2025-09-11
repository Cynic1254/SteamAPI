#include "FSteamInputController.h"

#include "Globals.h"
#include "Settings/SteamInputSettings.h"

FSteamInputController::FSteamInputController(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) : MessageHandler(InMessageHandler)
{
	GConfig->GetDouble(TEXT("/Script/Engine.InputSettings"), TEXT("InitialButtonRepeatDelay"), InitialButtonRepeatDelay, GInputIni);
	GConfig->GetDouble(TEXT("/Script/Engine.InputSettings"), TEXT("ButtonRepeatDelay"), ButtonRepeatDelay, GInputIni);

	if (SteamInput())
	{
		bControllerInitialized = true;

		UE_LOG(SteamInputLog, Log, TEXT("Steam Input Controller initialized successfully"));
	}
	else
	{
		UE_LOG(SteamInputLog, Warning, TEXT("Steam Input not available - controller disabled"));
	}
}

FSteamInputController::~FSteamInputController()
{
	bControllerInitialized = false;
}

void FSteamInputController::SendControllerEvents()
{
	if (!bControllerInitialized || !SteamInput())
	{
		return;
	}

	InputHandle_t Controllers[STEAM_INPUT_MAX_COUNT];
	const int32 ControllerCount = SteamInput()->GetConnectedControllers(Controllers);

	for (int i = 0; i < ControllerCount; ++i)
	{
		ProcessControllerInput(i, Controllers[i], ControllerStates[i]);
	}
}

void FSteamInputController::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
{
	if (ChannelType != FForceFeedbackChannelType::LEFT_LARGE && ChannelType != FForceFeedbackChannelType::RIGHT_LARGE)
	{
		return;
	}

	if (ControllerId >= 0 && ControllerId < STEAM_INPUT_MAX_COUNT)
	{
		FForceFeedbackValues Values;
		if (ChannelType == FForceFeedbackChannelType::LEFT_LARGE)
		{
			Values.LeftLarge = Value;
		}
		else
		{
			Values.RightLarge = Value;
		}

		SetVibration(ControllerId, Values);
	}
}

void FSteamInputController::SetChannelValues(int32 ControllerId, const FForceFeedbackValues& Values)
{
	if (ControllerId >= 0 && ControllerId < STEAM_INPUT_MAX_COUNT)
	{
		SetVibration(ControllerId, Values);
	}
}

void FSteamInputController::SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
	MessageHandler = InMessageHandler;
}

bool FSteamInputController::IsGamepadAttached() const
{
	return bControllerInitialized && SteamInput();
}

bool FSteamInputController::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	return false;
}

void FSteamInputController::SetVibration(int32 ControllerId, const FForceFeedbackValues& Values)
{
	const InputHandle_t ControllerHandle = SteamInput()->GetControllerForGamepadIndex(ControllerId);
	if (!ControllerHandle || !IsGamepadAttached())
	{
		return;
	}

	//TODO: Don't use legacy functions
	if (Values.LeftLarge > 0.0f)
	{
		SteamInput()->Legacy_TriggerHapticPulse(ControllerHandle, k_ESteamControllerPad_Left, static_cast<unsigned short>(Values.LeftLarge * 4000.0f));
	}

	if (Values.RightLarge > 0.0f)
	{
		SteamInput()->Legacy_TriggerHapticPulse(ControllerHandle, k_ESteamControllerPad_Right, static_cast<unsigned short>(Values.RightLarge * 4000.0f));
	}
}

void FSteamInputController::ProcessControllerInput(int32 ControllerIndex, InputHandle_t ControllerHandle,
                                                   FControllerState& State)
{
	//TODO: Handle Action Set

	static FName SystemName(TEXT("SteamController"));
	static FString ControllerName(TEXT("SteamController"));
	FInputDeviceScope InputScope{this, SystemName, ControllerIndex, ControllerName};

	for (auto& KeyPair : GetDefault<USteamInputSettings>()->Keys)
	{
		const FName& ActionName = KeyPair.Key;
		const FSteamInputAction& ActionData = KeyPair.Value;

		if (!ActionData.bHandleValid)
		{
			continue;
		}

		switch (ActionData.KeyType)
		{
		case EKeyType::Button:
			ProcessDigitalAction(ControllerIndex, ControllerHandle, ActionName, ActionData, State);
			break;
		default:
			ProcessAnalogAction(ControllerIndex, ControllerHandle, ActionName, ActionData, State);
			break;
		}
	}
}

void FSteamInputController::ProcessDigitalAction(int32 ControllerIndex, InputHandle_t ControllerHandle,
	const FName& ActionName, const FSteamInputAction& ActionData, FControllerState& State)
{
	const InputDigitalActionData_t ActionState = SteamInput()->GetDigitalActionData(ControllerHandle, ActionData.CachedHandle);

	const bool bPreviousState = State.DigitalStatusMap.FindRef(ActionName);
	const bool bCurrentState = ActionState.bState;

	const FPlatformUserId UserId = GetPlatformUserId(ControllerIndex);
	const FInputDeviceId DeviceId = GetInputDeviceId(ControllerIndex);

	if (UserId == PLATFORMUSERID_NONE || DeviceId == INPUTDEVICEID_NONE)
	{
		return;
	}

	const double CurrentTime = FPlatformTime::Seconds();

	if (!bPreviousState && bCurrentState)
	{
		MessageHandler->OnControllerButtonPressed(ActionName, UserId, DeviceId, false);
		UpdateKeyRepeatTiming(ActionName, State, CurrentTime);

		//TODO: Process Slate
	}
	else if (bPreviousState && !bCurrentState)
	{
		MessageHandler->OnControllerButtonReleased(ActionName, UserId, DeviceId, false);
		State.DigitalRepeatTimeMap.Remove(ActionName);

		//TODO: Process Slate
	}
	else if (bPreviousState && bCurrentState && ShouldProcessKeyRepeat(ActionName, State, CurrentTime))
	{
		MessageHandler->OnControllerButtonPressed(ActionName, UserId, DeviceId, true);
		UpdateKeyRepeatTiming(ActionName, State, CurrentTime);
	}

	State.DigitalStatusMap.Add(ActionName, bCurrentState);
}

void FSteamInputController::ProcessAnalogAction(int32 ControllerIndex, InputHandle_t ControllerHandle,
	const FName& ActionName, const FSteamInputAction& ActionData, FControllerState& State)
{
	const InputAnalogActionData_t ActionState = SteamInput()->GetAnalogActionData(ControllerHandle, ActionData.CachedHandle);

	const ControllerAnalogActionData_t PreviousState = State.AnalogStatusMap.FindRef(ActionName);

	const FPlatformUserId UserId = GetPlatformUserId(ControllerIndex);
	const FInputDeviceId DeviceId = GetInputDeviceId(ControllerIndex);

	if (UserId == PLATFORMUSERID_NONE || DeviceId == INPUTDEVICEID_NONE)
	{
		return;
	}

	switch (ActionData.KeyType)
	{
	case EKeyType::MouseInput:
		{
			if (ActionState.x != 0.0f || ActionState.y != 0)
			{
				MessageHandler->OnRawMouseMove(ActionState.x, ActionState.y);
			}
		}
		break;
	case EKeyType::Analog:
		{
			if (PreviousState.x != ActionState.x)
			{
				MessageHandler->OnControllerAnalog(ActionName, UserId, DeviceId, ActionState.x);
			}
		}
		break;
	case EKeyType::Joystick:
		if (PreviousState.x != ActionState.x)
		{
			const FName XAxisName = USteamInputSettings::GetXAxisName(ActionName);
			MessageHandler->OnControllerAnalog(XAxisName, UserId, DeviceId, ActionState.x);
		}
			
		if (PreviousState.y != ActionState.y)
		{
			const FName YAxisName = USteamInputSettings::GetYAxisName(ActionName);
			MessageHandler->OnControllerAnalog(YAxisName, UserId, DeviceId, ActionState.y);
		}
		break;
	}

	State.AnalogStatusMap.Add(ActionName, ActionState);
}

bool FSteamInputController::ShouldProcessKeyRepeat(const FName& ActionName, FControllerState& State,
	const double CurrentTime) const
{
	const double* NextRepeatTime = State.DigitalRepeatTimeMap.Find(ActionName);
	return NextRepeatTime && CurrentTime >= *NextRepeatTime;
}

void FSteamInputController::UpdateKeyRepeatTiming(const FName& ActionName, FControllerState& State,
	double CurrentTime) const
{
	const bool bIsFirstRepeat = !State.DigitalRepeatTimeMap.Contains(ActionName);
	const double DelayToUse = bIsFirstRepeat ? InitialButtonRepeatDelay : ButtonRepeatDelay;

	State.DigitalRepeatTimeMap.Add(ActionName, CurrentTime + DelayToUse);
}

FInputDeviceId FSteamInputController::GetInputDeviceId(int32 ControllerIndex) const
{
	FPlatformUserId UserId;
	FInputDeviceId DeviceId;
	IPlatformInputDeviceMapper::Get().RemapControllerIdToPlatformUserAndDevice(ControllerIndex, UserId, DeviceId);
	return DeviceId;
}

FPlatformUserId FSteamInputController::GetPlatformUserId(int32 ControllerIndex) const
{
	return FPlatformMisc::GetPlatformUserForUserIndex(ControllerIndex);
}
