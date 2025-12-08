#include "Controller/FSteamInputController.h"

#include "Globals.h"
#include "Helper/SteamInputFunctionLibrary.h"
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
	
	UpdateControllerState(Controllers, ControllerCount);

	for (auto& ControllerState : ControllerStates)
	{
		ProcessControllerInput(ControllerState.Key, ControllerState.Value);
	}
}

void FSteamInputController::SetChannelValue(const int32 ControllerId, const FForceFeedbackChannelType ChannelType, const float Value)
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

void FSteamInputController::SetChannelValues(const int32 ControllerId, const FForceFeedbackValues& Values)
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

void FSteamInputController::SetVibration(const int32 ControllerId, const FForceFeedbackValues& Values) const
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

void FSteamInputController::ProcessControllerInput(const FInputHandle& ControllerHandle,
                                                   FControllerState& State)
{
	static FName SystemName(TEXT("SteamController"));
	static FString ControllerName(TEXT("SteamController"));
	FInputDeviceScope InputScope{this, SystemName, static_cast<int32>(GetTypeHash(ControllerHandle.ControllerID)), ControllerName};
	
	FPlatformUserId UserId;
	FInputDeviceId DeviceId;
	GetPlatformUserAndDevice(ControllerHandle, UserId, DeviceId);
	
	SteamInput()->ActivateActionSet(ControllerHandle, USteamInputFunctionLibrary::GetActionSetForController(DeviceId));

	SteamInput()->DeactivateAllActionSetLayers(ControllerHandle);
	if (const auto ActionLayers = USteamInputFunctionLibrary::GetActionLayersForController(DeviceId))
		for (const auto ActionLayer : *ActionLayers)
		{
			SteamInput()->ActivateActionSetLayer(ControllerHandle, ActionLayer);
		}
	
	for (auto& Key : GetDefault<USteamInputSettings>()->Keys)
	{
		const FName& ActionName = Key.ActionName;

		if (!Key.bHandleValid)
		{
			continue;
		}

		switch (Key.KeyType)
		{
		case EKeyType::Button:
			ProcessDigitalAction(UserId, DeviceId, ControllerHandle, ActionName, Key, State);
			break;
		default:
			ProcessAnalogAction(UserId, DeviceId, ControllerHandle, ActionName, Key, State);
			break;
		}
	}
}

void FSteamInputController::ProcessDigitalAction(const FPlatformUserId UserID, const FInputDeviceId DeviceId, const FInputHandle& ControllerHandle,
	const FName& ActionName, const FSteamInputAction& ActionData, FControllerState& State) const
{
	if (UserID == PLATFORMUSERID_NONE || DeviceId == INPUTDEVICEID_NONE)
	{
		return;
	}
	
	const auto [bState, bActive] = SteamInput()->GetDigitalActionData(ControllerHandle, ActionData.CachedHandle);

	const bool bPreviousState = State.DigitalStatusMap.FindRef(ActionName);

	const double CurrentTime = FPlatformTime::Seconds();

	if (!bPreviousState && bState)
	{
		MessageHandler->OnControllerButtonPressed(ActionName, UserID, DeviceId, false);
		UpdateKeyRepeatTiming(ActionName, State, CurrentTime);
	}
	else if (bPreviousState && !bState)
	{
		MessageHandler->OnControllerButtonReleased(ActionName, UserID, DeviceId, false);
		State.DigitalRepeatTimeMap.Remove(ActionName);
	}
	else if (bPreviousState && bState && ShouldProcessKeyRepeat(ActionName, State, CurrentTime))
	{
		MessageHandler->OnControllerButtonPressed(ActionName, UserID, DeviceId, true);
		UpdateKeyRepeatTiming(ActionName, State, CurrentTime);
	}

	State.DigitalStatusMap.Add(ActionName, bState);
}

void FSteamInputController::ProcessAnalogAction(const FPlatformUserId UserID, const FInputDeviceId DeviceId, const FInputHandle& ControllerHandle,
	const FName& ActionName, const FSteamInputAction& ActionData, FControllerState& State) const
{
	const InputAnalogActionData_t ActionState = SteamInput()->GetAnalogActionData(ControllerHandle, ActionData.CachedHandle);

	const ControllerAnalogActionData_t PreviousState = State.AnalogStatusMap.FindRef(ActionName);

	if (UserID == PLATFORMUSERID_NONE || DeviceId == INPUTDEVICEID_NONE)
	{
		return;
	}

	switch (ActionData.KeyType)
	{
	case EKeyType::Analog:
		{
			if (PreviousState.x != ActionState.x)
			{
				MessageHandler->OnControllerAnalog(ActionName, UserID, DeviceId, ActionState.x);
			}
		}
		break;
	case EKeyType::MouseInput:
	case EKeyType::Joystick:
		if (PreviousState.x != ActionState.x)
		{
			const FName XAxisName = USteamInputSettings::GetXAxisName(ActionName);
			MessageHandler->OnControllerAnalog(XAxisName, UserID, DeviceId, ActionState.x);
		}
			
		if (PreviousState.y != ActionState.y)
		{
			const FName YAxisName = USteamInputSettings::GetYAxisName(ActionName);
			MessageHandler->OnControllerAnalog(YAxisName, UserID, DeviceId, ActionState.y);
		}
		break;
	default:
		break;
	}

	State.AnalogStatusMap.Add(ActionName, ActionState);
}

void FSteamInputController::UpdateControllerState(const InputHandle_t* ConnectedControllers, const int32 Count)
{
	//remove controllers that have been disconnected for more than 1 frame, and mark connected controllers as disconnected
	for (auto It = ControllerStates.CreateIterator(); It; ++It)
	{
		switch (It.Value().ConnectionState)
		{
		case FControllerState::Reconnect:
		case FControllerState::Connected:
			{
				It.Value().ConnectionState = FControllerState::Disconnected;
				break;
			}
		case FControllerState::Disconnected:
			{
				It.RemoveCurrent();
				continue;
			}
		}
	}

	//mark currently connected controllers as connected.
	for (int i = 0; i < Count; ++i)
	{
		//if the controller is already in the list, just mark it as connected, otherwise mark it as a new (re)connection
		if (const auto ControllerState = ControllerStates.Find(ConnectedControllers[i]))
		{
			ControllerState->ConnectionState = FControllerState::Connected;
		}
		else
		{
			ControllerStates.Add(ConnectedControllers[i]).ConnectionState = FControllerState::Reconnect;
		}
	}
}

void FSteamInputController::GetPlatformUserAndDevice(const FInputHandle InputHandle, FPlatformUserId& OutUserID,
                                                     FInputDeviceId& OutDeviceId)
{
	OutDeviceId = USteamInputFunctionLibrary::DeviceMappings.GetOrCreateDeviceId(InputHandle);

	IPlatformInputDeviceMapper& DeviceMapper = IPlatformInputDeviceMapper::Get();

	if (auto ControllerState = ControllerStates.Find(InputHandle))
	{
		switch (ControllerState->ConnectionState)
		{
		case FControllerState::Reconnect:
			{
				OutUserID = DeviceMapper.GetPlatformUserForNewlyConnectedDevice();
				DeviceMapper.Internal_MapInputDeviceToUser(OutDeviceId, OutUserID, EInputDeviceConnectionState::Connected);
				break;
			}
		case FControllerState::Disconnected:
			{
				OutUserID = DeviceMapper.GetUserForUnpairedInputDevices();
				break;
			}
		default:
			{
				OutUserID = DeviceMapper.GetUserForInputDevice(OutDeviceId);
			}
		}
	}
}

bool FSteamInputController::ShouldProcessKeyRepeat(const FName& ActionName, FControllerState& State,
                                                   const double CurrentTime) const
{
	const double* NextRepeatTime = State.DigitalRepeatTimeMap.Find(ActionName);
	return NextRepeatTime && CurrentTime >= *NextRepeatTime;
}

void FSteamInputController::UpdateKeyRepeatTiming(const FName& ActionName, FControllerState& State,
	const double CurrentTime) const
{
	const bool bIsFirstRepeat = !State.DigitalRepeatTimeMap.Contains(ActionName);
	const double DelayToUse = bIsFirstRepeat ? InitialButtonRepeatDelay : ButtonRepeatDelay;

	State.DigitalRepeatTimeMap.Add(ActionName, CurrentTime + DelayToUse);
}
