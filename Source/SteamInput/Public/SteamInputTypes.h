// ReSharper disable CppNonExplicitConvertingConstructor
// ReSharper disable CppNonExplicitConversionOperator
// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "SteamInputTypes.generated.h"

typedef uint64 InputHandle_t;
typedef uint64 InputActionSetHandle_t;
typedef uint64 ControllerDigitalActionHandle_t;
typedef uint64 ControllerAnalogActionHandle_t;
typedef uint64 ControllerActionHandle_t;

UENUM()
enum class ESteamInputActionOrigin : uint16
{
	// Steam Controller
	ENone,
	ESteamController_A,
	ESteamController_B,
	ESteamController_X,
	ESteamController_Y,
	ESteamController_LeftBumper,
	ESteamController_RightBumper,
	ESteamController_LeftGrip,
	ESteamController_RightGrip,
	ESteamController_Start,
	ESteamController_Back,
	ESteamController_LeftPad_Touch,
	ESteamController_LeftPad_Swipe,
	ESteamController_LeftPad_Click,
	ESteamController_LeftPad_DPadNorth,
	ESteamController_LeftPad_DPadSouth,
	ESteamController_LeftPad_DPadWest,
	ESteamController_LeftPad_DPadEast,
	ESteamController_RightPad_Touch,
	ESteamController_RightPad_Swipe,
	ESteamController_RightPad_Click,
	ESteamController_RightPad_DPadNorth,
	ESteamController_RightPad_DPadSouth,
	ESteamController_RightPad_DPadWest,
	ESteamController_RightPad_DPadEast,
	ESteamController_LeftTrigger_Pull,
	ESteamController_LeftTrigger_Click,
	ESteamController_RightTrigger_Pull,
	ESteamController_RightTrigger_Click,
	ESteamController_LeftStick_Move,
	ESteamController_LeftStick_Click,
	ESteamController_LeftStick_DPadNorth,
	ESteamController_LeftStick_DPadSouth,
	ESteamController_LeftStick_DPadWest,
	ESteamController_LeftStick_DPadEast,
	ESteamController_Gyro_Move,
	ESteamController_Gyro_Pitch,
	ESteamController_Gyro_Yaw,
	ESteamController_Gyro_Roll,
	ESteamController_Reserved0,
	ESteamController_Reserved1,
	ESteamController_Reserved2,
	ESteamController_Reserved3,
	ESteamController_Reserved4,
	ESteamController_Reserved5,
	ESteamController_Reserved6,
	ESteamController_Reserved7,
	ESteamController_Reserved8,
	ESteamController_Reserved9,
	ESteamController_Reserved10,
	
	// PS4 Dual Shock
	EPS4_X,
	EPS4_Circle,
	EPS4_Triangle,
	EPS4_Square,
	EPS4_LeftBumper,
	EPS4_RightBumper,
	EPS4_Options,	//Start
	EPS4_Share,		//Back
	EPS4_LeftPad_Touch,
	EPS4_LeftPad_Swipe,
	EPS4_LeftPad_Click,
	EPS4_LeftPad_DPadNorth,
	EPS4_LeftPad_DPadSouth,
	EPS4_LeftPad_DPadWest,
	EPS4_LeftPad_DPadEast,
	EPS4_RightPad_Touch,
	EPS4_RightPad_Swipe,
	EPS4_RightPad_Click,
	EPS4_RightPad_DPadNorth,
	EPS4_RightPad_DPadSouth,
	EPS4_RightPad_DPadWest,
	EPS4_RightPad_DPadEast,
	EPS4_CenterPad_Touch,
	EPS4_CenterPad_Swipe,
	EPS4_CenterPad_Click,
	EPS4_CenterPad_DPadNorth,
	EPS4_CenterPad_DPadSouth,
	EPS4_CenterPad_DPadWest,
	EPS4_CenterPad_DPadEast,
	EPS4_LeftTrigger_Pull,
	EPS4_LeftTrigger_Click,
	EPS4_RightTrigger_Pull,
	EPS4_RightTrigger_Click,
	EPS4_LeftStick_Move,
	EPS4_LeftStick_Click,
	EPS4_LeftStick_DPadNorth,
	EPS4_LeftStick_DPadSouth,
	EPS4_LeftStick_DPadWest,
	EPS4_LeftStick_DPadEast,
	EPS4_RightStick_Move,
	EPS4_RightStick_Click,
	EPS4_RightStick_DPadNorth,
	EPS4_RightStick_DPadSouth,
	EPS4_RightStick_DPadWest,
	EPS4_RightStick_DPadEast,
	EPS4_DPad_North,
	EPS4_DPad_South,
	EPS4_DPad_West,
	EPS4_DPad_East,
	EPS4_Gyro_Move,
	EPS4_Gyro_Pitch,
	EPS4_Gyro_Yaw,
	EPS4_Gyro_Roll,
	EPS4_DPad_Move,
	EPS4_Reserved1,
	EPS4_Reserved2,
	EPS4_Reserved3,
	EPS4_Reserved4,
	EPS4_Reserved5,
	EPS4_Reserved6,
	EPS4_Reserved7,
	EPS4_Reserved8,
	EPS4_Reserved9,
	EPS4_Reserved10,

	// XBox One
	EXBoxOne_A,
	EXBoxOne_B,
	EXBoxOne_X,
	EXBoxOne_Y,
	EXBoxOne_LeftBumper,
	EXBoxOne_RightBumper,
	EXBoxOne_Menu,  //Start
	EXBoxOne_View,  //Back
	EXBoxOne_LeftTrigger_Pull,
	EXBoxOne_LeftTrigger_Click,
	EXBoxOne_RightTrigger_Pull,
	EXBoxOne_RightTrigger_Click,
	EXBoxOne_LeftStick_Move,
	EXBoxOne_LeftStick_Click,
	EXBoxOne_LeftStick_DPadNorth,
	EXBoxOne_LeftStick_DPadSouth,
	EXBoxOne_LeftStick_DPadWest,
	EXBoxOne_LeftStick_DPadEast,
	EXBoxOne_RightStick_Move,
	EXBoxOne_RightStick_Click,
	EXBoxOne_RightStick_DPadNorth,
	EXBoxOne_RightStick_DPadSouth,
	EXBoxOne_RightStick_DPadWest,
	EXBoxOne_RightStick_DPadEast,
	EXBoxOne_DPad_North,
	EXBoxOne_DPad_South,
	EXBoxOne_DPad_West,
	EXBoxOne_DPad_East,
	EXBoxOne_DPad_Move,
	EXBoxOne_LeftGrip_Lower,
	EXBoxOne_LeftGrip_Upper,
	EXBoxOne_RightGrip_Lower,
	EXBoxOne_RightGrip_Upper,
	EXBoxOne_Share, // Xbox Series X controllers only
	EXBoxOne_Reserved6,
	EXBoxOne_Reserved7,
	EXBoxOne_Reserved8,
	EXBoxOne_Reserved9,
	EXBoxOne_Reserved10,

	// XBox 360
	EXBox360_A,
	EXBox360_B,
	EXBox360_X,
	EXBox360_Y,
	EXBox360_LeftBumper,
	EXBox360_RightBumper,
	EXBox360_Start,		//Start
	EXBox360_Back,		//Back
	EXBox360_LeftTrigger_Pull,
	EXBox360_LeftTrigger_Click,
	EXBox360_RightTrigger_Pull,
	EXBox360_RightTrigger_Click,
	EXBox360_LeftStick_Move,
	EXBox360_LeftStick_Click,
	EXBox360_LeftStick_DPadNorth,
	EXBox360_LeftStick_DPadSouth,
	EXBox360_LeftStick_DPadWest,
	EXBox360_LeftStick_DPadEast,
	EXBox360_RightStick_Move,
	EXBox360_RightStick_Click,
	EXBox360_RightStick_DPadNorth,
	EXBox360_RightStick_DPadSouth,
	EXBox360_RightStick_DPadWest,
	EXBox360_RightStick_DPadEast,
	EXBox360_DPad_North,
	EXBox360_DPad_South,
	EXBox360_DPad_West,
	EXBox360_DPad_East,	
	EXBox360_DPad_Move,
	EXBox360_Reserved1,
	EXBox360_Reserved2,
	EXBox360_Reserved3,
	EXBox360_Reserved4,
	EXBox360_Reserved5,
	EXBox360_Reserved6,
	EXBox360_Reserved7,
	EXBox360_Reserved8,
	EXBox360_Reserved9,
	EXBox360_Reserved10,


	// Switch - Pro or Joycons used as a single input device.
	// This does not apply to a single joycon
	ESwitch_A,
	ESwitch_B,
	ESwitch_X,
	ESwitch_Y,
	ESwitch_LeftBumper,
	ESwitch_RightBumper,
	ESwitch_Plus,	//Start
	ESwitch_Minus,	//Back
	ESwitch_Capture,
	ESwitch_LeftTrigger_Pull,
	ESwitch_LeftTrigger_Click,
	ESwitch_RightTrigger_Pull,
	ESwitch_RightTrigger_Click,
	ESwitch_LeftStick_Move,
	ESwitch_LeftStick_Click,
	ESwitch_LeftStick_DPadNorth,
	ESwitch_LeftStick_DPadSouth,
	ESwitch_LeftStick_DPadWest,
	ESwitch_LeftStick_DPadEast,
	ESwitch_RightStick_Move,
	ESwitch_RightStick_Click,
	ESwitch_RightStick_DPadNorth,
	ESwitch_RightStick_DPadSouth,
	ESwitch_RightStick_DPadWest,
	ESwitch_RightStick_DPadEast,
	ESwitch_DPad_North,
	ESwitch_DPad_South,
	ESwitch_DPad_West,
	ESwitch_DPad_East,
	ESwitch_ProGyro_Move,  // Primary Gyro in Pro Controller, or Right JoyCon
	ESwitch_ProGyro_Pitch,  // Primary Gyro in Pro Controller, or Right JoyCon
	ESwitch_ProGyro_Yaw,  // Primary Gyro in Pro Controller, or Right JoyCon
	ESwitch_ProGyro_Roll,  // Primary Gyro in Pro Controller, or Right JoyCon
	ESwitch_DPad_Move,
	ESwitch_Reserved1,
	ESwitch_Reserved2,
	ESwitch_Reserved3,
	ESwitch_Reserved4,
	ESwitch_Reserved5,
	ESwitch_Reserved6,
	ESwitch_Reserved7,
	ESwitch_Reserved8,
	ESwitch_Reserved9,
	ESwitch_Reserved10,

	// Switch JoyCon Specific
	ESwitch_RightGyro_Move,  // Right JoyCon Gyro generally should correspond to Pro's single gyro
	ESwitch_RightGyro_Pitch,  // Right JoyCon Gyro generally should correspond to Pro's single gyro
	ESwitch_RightGyro_Yaw,  // Right JoyCon Gyro generally should correspond to Pro's single gyro
	ESwitch_RightGyro_Roll,  // Right JoyCon Gyro generally should correspond to Pro's single gyro
	ESwitch_LeftGyro_Move,
	ESwitch_LeftGyro_Pitch,
	ESwitch_LeftGyro_Yaw,
	ESwitch_LeftGyro_Roll,
	ESwitch_LeftGrip_Lower, // Left JoyCon SR Button
	ESwitch_LeftGrip_Upper, // Left JoyCon SL Button
	ESwitch_RightGrip_Lower,  // Right JoyCon SL Button
	ESwitch_RightGrip_Upper,  // Right JoyCon SR Button
	ESwitch_JoyConButton_N, // With a Horizontal JoyCon this will be Y or what would be Dpad Right when vertical
	ESwitch_JoyConButton_E, // X
	ESwitch_JoyConButton_S, // A
	ESwitch_JoyConButton_W, // B
	ESwitch_Reserved15,
	ESwitch_Reserved16,
	ESwitch_Reserved17,
	ESwitch_Reserved18,
	ESwitch_Reserved19,
	ESwitch_Reserved20,
	
	// Added in SDK 1.51
	EPS5_X,
	EPS5_Circle,
	EPS5_Triangle,
	EPS5_Square,
	EPS5_LeftBumper,
	EPS5_RightBumper,
	EPS5_Option,	//Start
	EPS5_Create,		//Back
	EPS5_Mute,
	EPS5_LeftPad_Touch,
	EPS5_LeftPad_Swipe,
	EPS5_LeftPad_Click,
	EPS5_LeftPad_DPadNorth,
	EPS5_LeftPad_DPadSouth,
	EPS5_LeftPad_DPadWest,
	EPS5_LeftPad_DPadEast,
	EPS5_RightPad_Touch,
	EPS5_RightPad_Swipe,
	EPS5_RightPad_Click,
	EPS5_RightPad_DPadNorth,
	EPS5_RightPad_DPadSouth,
	EPS5_RightPad_DPadWest,
	EPS5_RightPad_DPadEast,
	EPS5_CenterPad_Touch,
	EPS5_CenterPad_Swipe,
	EPS5_CenterPad_Click,
	EPS5_CenterPad_DPadNorth,
	EPS5_CenterPad_DPadSouth,
	EPS5_CenterPad_DPadWest,
	EPS5_CenterPad_DPadEast,
	EPS5_LeftTrigger_Pull,
	EPS5_LeftTrigger_Click,
	EPS5_RightTrigger_Pull,
	EPS5_RightTrigger_Click,
	EPS5_LeftStick_Move,
	EPS5_LeftStick_Click,
	EPS5_LeftStick_DPadNorth,
	EPS5_LeftStick_DPadSouth,
	EPS5_LeftStick_DPadWest,
	EPS5_LeftStick_DPadEast,
	EPS5_RightStick_Move,
	EPS5_RightStick_Click,
	EPS5_RightStick_DPadNorth,
	EPS5_RightStick_DPadSouth,
	EPS5_RightStick_DPadWest,
	EPS5_RightStick_DPadEast,
	EPS5_DPad_North,
	EPS5_DPad_South,
	EPS5_DPad_West,
	EPS5_DPad_East,
	EPS5_Gyro_Move,
	EPS5_Gyro_Pitch,
	EPS5_Gyro_Yaw,
	EPS5_Gyro_Roll,
	EPS5_DPad_Move,
	EPS5_LeftGrip,
	EPS5_RightGrip,
	EPS5_LeftFn,
	EPS5_RightFn,
	EPS5_Reserved5,
	EPS5_Reserved6,
	EPS5_Reserved7,
	EPS5_Reserved8,
	EPS5_Reserved9,
	EPS5_Reserved10,
	EPS5_Reserved11,
	EPS5_Reserved12,
	EPS5_Reserved13,
	EPS5_Reserved14,
	EPS5_Reserved15,
	EPS5_Reserved16,
	EPS5_Reserved17,
	EPS5_Reserved18,
	EPS5_Reserved19,
	EPS5_Reserved20,

	// Added in SDK 1.53
	ESteamDeck_A,
	ESteamDeck_B,
	ESteamDeck_X,
	ESteamDeck_Y,
	ESteamDeck_L1,
	ESteamDeck_R1,
	ESteamDeck_Menu,
	ESteamDeck_View,
	ESteamDeck_LeftPad_Touch,
	ESteamDeck_LeftPad_Swipe,
	ESteamDeck_LeftPad_Click,
	ESteamDeck_LeftPad_DPadNorth,
	ESteamDeck_LeftPad_DPadSouth,
	ESteamDeck_LeftPad_DPadWest,
	ESteamDeck_LeftPad_DPadEast,
	ESteamDeck_RightPad_Touch,
	ESteamDeck_RightPad_Swipe,
	ESteamDeck_RightPad_Click,
	ESteamDeck_RightPad_DPadNorth,
	ESteamDeck_RightPad_DPadSouth,
	ESteamDeck_RightPad_DPadWest,
	ESteamDeck_RightPad_DPadEast,
	ESteamDeck_L2_SoftPull,
	ESteamDeck_L2,
	ESteamDeck_R2_SoftPull,
	ESteamDeck_R2,
	ESteamDeck_LeftStick_Move,
	ESteamDeck_L3,
	ESteamDeck_LeftStick_DPadNorth,
	ESteamDeck_LeftStick_DPadSouth,
	ESteamDeck_LeftStick_DPadWest,
	ESteamDeck_LeftStick_DPadEast,
	ESteamDeck_LeftStick_Touch,
	ESteamDeck_RightStick_Move,
	ESteamDeck_R3,
	ESteamDeck_RightStick_DPadNorth,
	ESteamDeck_RightStick_DPadSouth,
	ESteamDeck_RightStick_DPadWest,
	ESteamDeck_RightStick_DPadEast,
	ESteamDeck_RightStick_Touch,
	ESteamDeck_L4,
	ESteamDeck_R4,
	ESteamDeck_L5,
	ESteamDeck_R5,
	ESteamDeck_DPad_Move,
	ESteamDeck_DPad_North,
	ESteamDeck_DPad_South,
	ESteamDeck_DPad_West,
	ESteamDeck_DPad_East,
	ESteamDeck_Gyro_Move,
	ESteamDeck_Gyro_Pitch,
	ESteamDeck_Gyro_Yaw,
	ESteamDeck_Gyro_Roll,
	ESteamDeck_Reserved1,
	ESteamDeck_Reserved2,
	ESteamDeck_Reserved3,
	ESteamDeck_Reserved4,
	ESteamDeck_Reserved5,
	ESteamDeck_Reserved6,
	ESteamDeck_Reserved7,
	ESteamDeck_Reserved8,
	ESteamDeck_Reserved9,
	ESteamDeck_Reserved10,
	ESteamDeck_Reserved11,
	ESteamDeck_Reserved12,
	ESteamDeck_Reserved13,
	ESteamDeck_Reserved14,
	ESteamDeck_Reserved15,
	ESteamDeck_Reserved16,
	ESteamDeck_Reserved17,
	ESteamDeck_Reserved18,
	ESteamDeck_Reserved19,
	ESteamDeck_Reserved20,

	ECount UMETA(Hidden), // If Steam has added support for new controllers origins will go here.
	EMaximumPossibleValue = 32767 UMETA(Hidden), // Origins are currently a maximum of 16 bits.
};

USTRUCT(BlueprintType)
struct FSteamInputActionOrigin
{
	GENERATED_BODY()
	ESteamInputActionOrigin ActionOrigin;
	
	FSteamInputActionOrigin() : ActionOrigin(ESteamInputActionOrigin::ENone) {}
	FSteamInputActionOrigin(ESteamInputActionOrigin InOrigin) : ActionOrigin(InOrigin) {}
    
	// Implicit conversion to enum for C++ usage
	operator ESteamInputActionOrigin() const { return ActionOrigin; }
    
	// Equality operator (required for TMap)
	bool operator==(const FSteamInputActionOrigin& Other) const
	{
		return ActionOrigin == Other.ActionOrigin;
	}
    
	bool operator!=(const FSteamInputActionOrigin& Other) const
	{
		return ActionOrigin != Other.ActionOrigin;
	}
    
	// For sorting (optional, but useful)
	bool operator<(const FSteamInputActionOrigin& Other) const
	{
		return ActionOrigin < Other.ActionOrigin;
	}
};

// Hash function (must be outside the struct, in the same header)
FORCEINLINE uint32 GetTypeHash(const FSteamInputActionOrigin& Origin)
{
	return GetTypeHash(static_cast<uint16>(Origin.ActionOrigin));
}

/// @brief Wrapper to allow the use of InputHandle_t inside unreal, for c++ this gets freely converted
USTRUCT(BlueprintType)
struct FInputHandle
{
	GENERATED_BODY()

	FInputHandle() = default;
	FInputHandle(const InputHandle_t Handle) : ControllerID(Handle) {}
	
	operator InputHandle_t() const {return ControllerID;}
	
	InputHandle_t ControllerID = 0;
};

FORCEINLINE uint32 GetTypeHash(const FInputHandle& Hash)
{
	return Hash;
}

/// @brief Wrapper to allow the use of InputActionSetHandle_t inside unreal, for c++ this gets freely converted
USTRUCT(BlueprintType)
struct FInputActionSetHandle
{
	GENERATED_BODY()

	FInputActionSetHandle() = default;
	FInputActionSetHandle(const InputActionSetHandle_t Handle) : ActionSetHandle(Handle) {}

	operator InputActionSetHandle_t() const {return ActionSetHandle;}

protected:
	InputActionSetHandle_t ActionSetHandle = 0;
};

UENUM()
enum class ActionType : uint8
{
	EUnknown,
	EAnalog,
	EDigital
};

/// @brief Ambiguous version of Controller action handles, represents both a Analog and Digital action handle, does no attempt to differentiate which one it represents.
USTRUCT(BlueprintType)
struct FControllerActionHandle
{
	GENERATED_BODY()

	FControllerActionHandle() = default;
	FControllerActionHandle(const ControllerActionHandle_t Handle) : ActionHandle(Handle) {}
	FControllerActionHandle(const ControllerActionHandle_t Handle, const ActionType Type) : ActionHandle(Handle), Type(Type) {}

	ControllerDigitalActionHandle_t GetDigitalActionHandle() const
	{check(Type == ActionType::EUnknown || Type == ActionType::EDigital); return ActionHandle;}
	ControllerAnalogActionHandle_t GetAnalogActionHandle() const
	{check(Type == ActionType::EUnknown || Type == ActionType::EAnalog); return ActionHandle;}

	ControllerActionHandle_t GetHandle() const {return ActionHandle;}
	ActionType GetType() const {return Type;}
protected:
	ControllerActionHandle_t ActionHandle = 0;
	UPROPERTY(BlueprintReadOnly)
	ActionType Type = ActionType::EUnknown;
};

USTRUCT(BlueprintType)
struct FControllerAnalogActionHandle : public FControllerActionHandle
{
	GENERATED_BODY()
	FControllerAnalogActionHandle() = default;
	FControllerAnalogActionHandle(const ControllerAnalogActionHandle_t Handle) : FControllerActionHandle(Handle, ActionType::EAnalog) {}

	operator ControllerAnalogActionHandle_t() const {return ActionHandle;}
};

USTRUCT(BlueprintType)
struct FControllerDigitalActionHandle : public FControllerActionHandle
{
	GENERATED_BODY()
	FControllerDigitalActionHandle() = default;
	FControllerDigitalActionHandle(const ControllerAnalogActionHandle_t Handle) : FControllerActionHandle(Handle, ActionType::EAnalog) {}

	operator ControllerDigitalActionHandle_t() const {return ActionHandle;}
};

USTRUCT(BlueprintType)
struct FSteamKey
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Steam", meta = (GetOptions="SteamInput.SteamInputSettings.GetFSteamKeysOptions"))
	FName Key = "default";

	FSteamKey() = default;
	FSteamKey(const FName Key) : Key(Key) {}
	operator FName() const {return Key;}
};
