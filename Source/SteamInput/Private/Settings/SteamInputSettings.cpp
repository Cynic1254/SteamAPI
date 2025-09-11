// Fill out your copyright notice in the Description page of Project Settings.

#include "Settings/SteamInputSettings.h"

#include "Framework/Application/NavigationConfig.h"
#include "Globals.h"
#include "steam/isteaminput.h"

bool FSteamInputAction::Update(const FName KeyName)
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
			const ControllerDigitalActionHandle_t Handle = SteamInput()->GetDigitalActionHandle(TCHAR_TO_UTF8(*KeyName.ToString()));
			CachedHandle = Handle;
			bHandleValid = (Handle != 0);
		}
		break;
	default:
		{
			const ControllerAnalogActionHandle_t Handle = SteamInput()->GetAnalogActionHandle(TCHAR_TO_UTF8(*KeyName.ToString()));
			CachedHandle = Handle;
			bHandleValid = (Handle != 0);
		}
		break;
	}

	return bHandleValid;
}

USteamInputSettings::USteamInputSettings()
{
	if (FSlateApplication::IsInitialized())
	{
		const TSharedRef<FNavigationConfig> Config = FSlateApplication::Get().GetNavigationConfig();
		KeyActionRules = Config->KeyActionRules;
		KeyEventRules = Config->KeyEventRules;
	}
}

void USteamInputSettings::RegenerateKeys()
{
	USteamInputSettings* Settings = GetMutableDefault<USteamInputSettings>();

	EKeys::RemoveKeysWithCategory(Settings->MenuCategory);

	for (const auto Key : Settings->Keys)
	{
		Settings->GenerateKey(Key.Key, Key.Value.KeyType);
	}
}

void USteamInputSettings::RefreshHandles()
{
	UpdateAllHandles();
	
#if WITH_EDITOR
	Modify();
#endif
}

FName USteamInputSettings::GetXAxisName(const FName Name)
{
	return FName{Name.ToString() + TEXT("_AxisX")};
}

FName USteamInputSettings::GetYAxisName(const FName Name)
{
	return FName{Name.ToString() + TEXT("_AxisY")};
}

void USteamInputSettings::ApplySlateConfig()
{
	if (FSlateApplication::IsInitialized())
	{
		const USteamInputSettings* Settings = GetDefault<USteamInputSettings>();
		const TSharedRef<FNavigationConfig> Config = FSlateApplication::Get().GetNavigationConfig();
		Config->KeyActionRules = Settings->KeyActionRules;
		Config->KeyEventRules = Settings->KeyEventRules;
	}
}

TArray<FName> USteamInputSettings::GetFSteamKeysOptions()
{
	TArray<FName> Keys;
	GetDefault<USteamInputSettings>()->Keys.GetKeys(Keys);
	return Keys;
}

void USteamInputSettings::PostInitProperties()
{
	UObject::PostInitProperties();

	RegenerateKeys();
	ApplySlateConfig();
}

void USteamInputSettings::GenerateKey(FName ActionName, EKeyType KeyType) const
{
	switch (KeyType)
	{
	case EKeyType::Button:
		{
			const FKey Key{ActionName};
			if (EKeys::GetKeyDetails(Key))
			{
				UE_LOG(SteamInputLog, Error, TEXT("Key %s already exists"), *Key.ToString());
				break;
			}

			EKeys::AddKey({Key, Key.GetDisplayName(), FKeyDetails::GamepadKey, MenuCategory});
			break;
		}
	case EKeyType::Analog:
		{
			const FKey Key{ActionName};
			if (EKeys::GetKeyDetails(Key))
			{
				UE_LOG(SteamInputLog, Error, TEXT("Key %s already exists"), *Key.ToString());
				break;
			}

			EKeys::AddKey({Key, Key.GetDisplayName(), FKeyDetails::GamepadKey | FKeyDetails::Axis1D, MenuCategory});
			break;
		}
	case EKeyType::MouseInput:
	case EKeyType::Joystick:
		{
			const FKey KeyX{GetXAxisName(ActionName)};
			const FKey KeyY{GetYAxisName(ActionName)};
			const FKey Key{ActionName};

			EKeys::AddKey({KeyX, KeyX.GetDisplayName(), FKeyDetails::GamepadKey | FKeyDetails::Axis1D, MenuCategory});
			EKeys::AddKey({KeyY, KeyY.GetDisplayName(), FKeyDetails::GamepadKey | FKeyDetails::Axis1D, MenuCategory});
			EKeys::AddPairedKey({Key, Key.GetDisplayName(), FKeyDetails::GamepadKey | FKeyDetails::Axis2D, MenuCategory}, KeyX, KeyY);
		}
		break;
	}
}

void USteamInputSettings::UpdateAllHandles()
{
	for (auto& Key : Keys)
	{
		Key.Value.Update(Key.Key);
	}
}

#if WITH_EDITOR
void USteamInputSettings::PreEditChange(FProperty* PropertyAboutToChange)
{
	UObject::PreEditChange(PropertyAboutToChange);

	if (PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(USteamInputSettings, MenuCategory))
	{
		EKeys::RemoveKeysWithCategory(MenuCategory);
	}
}

void USteamInputSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	const FName MemberPropertyName = PropertyChangedEvent.MemberProperty ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;

	if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(USteamInputSettings, Keys))
	{
		if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ArrayAdd ||
			PropertyChangedEvent.ChangeType == EPropertyChangeType::ValueSet ||
			PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive)
		{
			UpdateAllHandles();
		}

		RegenerateKeys();
	}
	else
	{
		RegenerateKeys();
		ApplySlateConfig();
	}
}
#endif