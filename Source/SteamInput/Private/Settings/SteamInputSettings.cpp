// Fill out your copyright notice in the Description page of Project Settings.

#include "Settings/SteamInputSettings.h"

#include "SteamInput.h"
#include "SteamInputTypes.h"
#include "steam/isteaminput.h"
#include "steam/isteamutils.h"

DEFINE_LOG_CATEGORY_STATIC(SteamInputLog, Log, All);

const FName USteamInputSettings::MenuCategory = "SteamBindings";

bool FSteamInputAction::GenerateHandle()
{

	if (!SteamInput())
	{
		bHandleValid = false;
		return bHandleValid;
	}

	switch (KeyType)
	{
	case EKeyType::Button:
		{
			const ControllerDigitalActionHandle_t Handle = SteamInput()->GetDigitalActionHandle(TCHAR_TO_UTF8(*ActionName.ToString()));
			CachedHandle = Handle;
			bHandleValid = (Handle != 0);
		}
		break;
	default:
		{
			const ControllerAnalogActionHandle_t Handle = SteamInput()->GetAnalogActionHandle(TCHAR_TO_UTF8(*ActionName.ToString()));
			CachedHandle = Handle;
			bHandleValid = (Handle != 0);
		}
		break;
	}

	return bHandleValid;
}

void FSteamInputAction::GenerateKey(bool RefreshHandle)
{
	switch (KeyType)
	{
	case EKeyType::Button:
		{
			const FKey Key{ActionName};
			if (!EKeys::GetKeyDetails(Key))
			{
				EKeys::AddKey({Key, Key.GetDisplayName(), FKeyDetails::GamepadKey, USteamInputSettings::MenuCategory});
			}
			else
			{
				UE_LOG(SteamInputLog, Warning, TEXT("Key with name: %s, already exists"), *ActionName.ToString())
			}
		}
		break;
	case EKeyType::Analog:
		{
			const FKey Key{ActionName};
			if (!EKeys::GetKeyDetails(Key))
			{
				EKeys::AddKey({Key, Key.GetDisplayName(), FKeyDetails::GamepadKey | FKeyDetails::Axis1D, USteamInputSettings::MenuCategory});
			}
			else
			{
				UE_LOG(SteamInputLog, Warning, TEXT("Key with name: %s, already exists"), *ActionName.ToString())
			}
		}
		break;
	case EKeyType::MouseInput:
	case EKeyType::Joystick:
		{
			const FKey KeyX{USteamInputSettings::GetXAxisName(ActionName)};
			const FKey KeyY{USteamInputSettings::GetYAxisName(ActionName)};
			const FKey Key{ActionName};

			if (!EKeys::GetKeyDetails(Key))
			{
				EKeys::AddKey({KeyX, KeyX.GetDisplayName(), FKeyDetails::GamepadKey | FKeyDetails::Axis1D, USteamInputSettings::MenuCategory});
				EKeys::AddKey({KeyY, KeyY.GetDisplayName(), FKeyDetails::GamepadKey | FKeyDetails::Axis1D, USteamInputSettings::MenuCategory});
				EKeys::AddPairedKey({Key, Key.GetDisplayName(), FKeyDetails::GamepadKey | FKeyDetails::Axis2D, USteamInputSettings::MenuCategory}, KeyX, KeyY);
			}
			else
			{
				UE_LOG(SteamInputLog, Warning, TEXT("Key with name: %s, already exists"), *ActionName.ToString())
			}
			
		}
		break;
	}

	if (RefreshHandle)
	{
		GenerateHandle();
	}
}

FName USteamInputSettings::GetXAxisName(const FName Name)
{
	return FName{Name.ToString() + TEXT("_AxisX")};
}

FName USteamInputSettings::GetYAxisName(const FName Name)
{
	return FName{Name.ToString() + TEXT("_AxisY")};
}

void USteamInputSettings::RefreshHandles()
{
	EKeys::RemoveKeysWithCategory(MenuCategory);

	for (FSteamInputAction Key : Keys)
	{
		Key.GenerateKey(true);
	}
}

void USteamInputSettings::PostInitProperties()
{
	UObject::PostInitProperties();

	SteamInputInitialized::FDelegate Delegate{};

	Delegate.BindUObject(this, &USteamInputSettings::SteamInputInitialized);
	
	FSteamInputModule::Get().BindToOnInputInitialized(Delegate);
}

void USteamInputSettings::PostLoad()
{
	UObject::PostLoad();
}

void USteamInputSettings::SteamInputInitialized()
{
	AppID = SteamUtils()->GetAppID();

	RefreshHandles();
}


#if WITH_EDITOR

void USteamInputSettings::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeChainProperty(PropertyChangedEvent);

	const FName MemberPropertyName = PropertyChangedEvent.MemberProperty ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;
    
	// Check if we're dealing with the Keys array
	if (PropertyChangedEvent.PropertyChain.GetActiveMemberNode() && 
		PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue() &&
		PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue()->GetFName() == GET_MEMBER_NAME_CHECKED(USteamInputSettings, Keys))
	{
		RefreshHandles();
	}
}

#endif