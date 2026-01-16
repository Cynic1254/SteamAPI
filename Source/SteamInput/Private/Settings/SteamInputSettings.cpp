// Copyright 2026 Cynic. All Rights Reserved.

#include "Settings/SteamInputSettings.h"

#include "Globals.h"
#include "SteamInput.h"
#include "SteamInputTypes.h"
#include "Framework/Application/NavigationConfig.h"
#include "Framework/Application/SlateApplication.h"
#include "steam/isteaminput.h"
#include "steam/isteamutils.h"

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

void FSteamInputAction::GenerateKey(const bool RefreshHandle)
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

TArray<FName> USteamInputSettings::GetKeyList()
{
	TArray<FName> Out{};
	const USteamInputSettings* Settings = GetDefault<USteamInputSettings>();
	Out.Reserve(Settings->Keys.Num());
	for (const FSteamInputAction& Key : Settings->Keys)
	{
		Out.Add(Key.ActionName);
	}

	return Out;
}

void USteamInputSettings::RefreshHandles()
{
	EKeys::RemoveKeysWithCategory(MenuCategory);

	for (FSteamInputAction& Key : Keys)
	{
		Key.GenerateKey(true);
	}

	UpdateSlateNavigationConfig();
}

void USteamInputSettings::UpdateSlateNavigationConfig()
{
	if (!FSlateApplication::IsInitialized())
	{
		UE_LOG(SteamInputLog, Warning, TEXT("Slate not initialized, skipping navigation config update"));
		return;
	}

	TSharedRef<FNavigationConfig> NavConfig = FSlateApplication::Get().GetNavigationConfig();
	
	UE_LOG(SteamInputLog, Log, TEXT("Updating Slate navigation configuration"));
	
	// Clear existing Steam Input bindings from navigation config
	for (const FSteamInputAction& Key : Keys)
	{
		const FKey SteamKey(Key.ActionName);
		NavConfig->KeyEventRules.Remove(SteamKey);
		NavConfig->KeyActionRules.Remove(SteamKey);
		
		// Handle 2D keys
		if (Key.KeyType == EKeyType::Joystick)
		{
			NavConfig->KeyEventRules.Remove(FKey(GetXAxisName(Key.ActionName)));
			NavConfig->KeyEventRules.Remove(FKey(GetYAxisName(Key.ActionName)));
		}
	}
	
	// Apply new bindings
	for (const FSlateNavigationBinding& Binding : SlateNavigationBindings)
	{
		const FKey SteamKey(Binding.SteamActionName);

		if (Binding.NavigationType == EUINavigationOptions::Invalid)
		{
			continue;
		}
		
		if (FNavigationOptionHelper::IsNavigation(Binding.NavigationType))
		{
			NavConfig->KeyEventRules.Add(SteamKey, FNavigationOptionHelper::ToNavigation(Binding.NavigationType));
			UE_LOG(SteamInputLog, Log, TEXT("Added navigation binding: %s -> %d"), 
				   *Binding.SteamActionName.ToString(), FNavigationOptionHelper::ToNavigation(Binding.NavigationType));
		}
		
		if (FNavigationOptionHelper::IsAction(Binding.NavigationType))
		{
			NavConfig->KeyActionRules.Add(SteamKey, FNavigationOptionHelper::ToAction(Binding.NavigationType));
			UE_LOG(SteamInputLog, Log, TEXT("Added action binding: %s -> %d"), 
				   *Binding.SteamActionName.ToString(), FNavigationOptionHelper::ToAction(Binding.NavigationType));
		}
	}
	
	UE_LOG(SteamInputLog, Log, TEXT("Slate navigation configuration updated with %d bindings"), 
		   SlateNavigationBindings.Num());
}

void USteamInputSettings::SetupDefaultSlateBindings()
{
	UE_LOG(SteamInputLog, Log, TEXT("Setting up default Slate navigation bindings"));
	
	// Clear existing auto-generated bindings (those using common menu action names)
	SlateNavigationBindings.RemoveAll([](const FSlateNavigationBinding& Binding) {
		const FString ActionStr = Binding.SteamActionName.ToString().ToLower();
		return ActionStr.StartsWith("menu_") || ActionStr.StartsWith("ui_") || ActionStr.StartsWith("nav_");
	});
	
	// Helper to add binding if the action exists
	auto AddBindingIfActionExists = [this](FName ActionName, EUINavigation NavType) {
		if (Keys.ContainsByPredicate([ActionName](const FSteamInputAction& Action) {
			return Action.ActionName == ActionName;
		}))
		{
			SlateNavigationBindings.Add(FSlateNavigationBinding(ActionName, NavType));
			UE_LOG(SteamInputLog, Log, TEXT("Auto-added navigation binding: %s"), *ActionName.ToString());
		}
	};
	
	auto AddActionBindingIfExists = [this](FName ActionName, EUINavigationAction ActionType) {
		if (Keys.ContainsByPredicate([ActionName](const FSteamInputAction& Action) {
			return Action.ActionName == ActionName;
		}))
		{
			SlateNavigationBindings.Add(FSlateNavigationBinding(ActionName, ActionType));
			UE_LOG(SteamInputLog, Log, TEXT("Auto-added action binding: %s"), *ActionName.ToString());
		}
	};
	
	// Add navigation bindings
	AddBindingIfActionExists(NavigateUpAction, EUINavigation::Up);
	AddBindingIfActionExists(NavigateDownAction, EUINavigation::Down);
	AddBindingIfActionExists(NavigateLeftAction, EUINavigation::Left);
	AddBindingIfActionExists(NavigateRightAction, EUINavigation::Right);
	
	// Add action bindings
	AddActionBindingIfExists(AcceptAction, EUINavigationAction::Accept);
	AddActionBindingIfExists(BackAction, EUINavigationAction::Back);
}

void USteamInputSettings::PostInitProperties()
{
	Super::PostInitProperties();

	FSteamInputModule::Get().BindToOnInputInitialized(
		SteamInputInitialized::FDelegate::CreateUObject(this, &USteamInputSettings::SteamInputInitialized)
	);
}

void USteamInputSettings::PostLoad()
{
	Super::PostLoad();

	// Setup default bindings if needed and auto-configure is enabled
	if (bAutoConfigureCommonNavigation && SlateNavigationBindings.Num() == 0)
	{
		SetupDefaultSlateBindings();
	}
}

void USteamInputSettings::SteamInputInitialized()
{
#if WITH_EDITORONLY_DATA
	AppID = SteamUtils()->GetAppID();
#endif

	RefreshHandles();

	// Setup default Slate bindings if enabled
	if (bAutoConfigureCommonNavigation)
	{
		SetupDefaultSlateBindings();
		UpdateSlateNavigationConfig();
	}
}


#if WITH_EDITOR

void USteamInputSettings::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	const FName MemberPropertyName = PropertyChangedEvent.MemberProperty ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;

	bool bNeedSlateUpdate = false;
	
	// Check if we're dealing with the Keys array
	if (PropertyChangedEvent.PropertyChain.GetActiveMemberNode() && 
		PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue() &&
		PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue()->GetFName() == GET_MEMBER_NAME_CHECKED(USteamInputSettings, Keys))
	{
		RefreshHandles();

		// Update Slate config if auto-configure is enabled
		if (bAutoConfigureCommonNavigation)
		{
			SetupDefaultSlateBindings();
		}
		bNeedSlateUpdate = true;
	}

	// Handle Slate navigation changes
	if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(USteamInputSettings, SlateNavigationBindings) ||
		MemberPropertyName == GET_MEMBER_NAME_CHECKED(USteamInputSettings, bAutoConfigureCommonNavigation) ||
		MemberPropertyName == GET_MEMBER_NAME_CHECKED(USteamInputSettings, NavigateUpAction) ||
		MemberPropertyName == GET_MEMBER_NAME_CHECKED(USteamInputSettings, NavigateDownAction) ||
		MemberPropertyName == GET_MEMBER_NAME_CHECKED(USteamInputSettings, NavigateLeftAction) ||
		MemberPropertyName == GET_MEMBER_NAME_CHECKED(USteamInputSettings, NavigateRightAction) ||
		MemberPropertyName == GET_MEMBER_NAME_CHECKED(USteamInputSettings, AcceptAction) ||
		MemberPropertyName == GET_MEMBER_NAME_CHECKED(USteamInputSettings, BackAction))
	{
		if (bAutoConfigureCommonNavigation)
		{
			SetupDefaultSlateBindings();
		}
		bNeedSlateUpdate = true;
	}
	
	if (bNeedSlateUpdate)
	{
		UpdateSlateNavigationConfig();
	}
}

void USteamInputSettings::ValidateSlateIntegration()
{
	if (!FSlateApplication::IsInitialized())
    {
        UE_LOG(LogTemp, Error, TEXT("Slate not initialized!"));
        return;
    }
    
    TSharedRef<FNavigationConfig> NavConfig = FSlateApplication::Get().GetNavigationConfig();
    
    UE_LOG(LogTemp, Log, TEXT("=== Slate Navigation Config Validation ==="));
    UE_LOG(LogTemp, Log, TEXT("Total KeyEventRules: %d"), NavConfig->KeyEventRules.Num());
    UE_LOG(LogTemp, Log, TEXT("Total KeyActionRules: %d"), NavConfig->KeyActionRules.Num());
    
    // Check our Steam Input bindings
    int32 SteamBindingCount = 0;
    for (const auto& Rule : NavConfig->KeyEventRules)
    {
        if (Rule.Key.GetFName().ToString().Contains("menu_") || 
            Keys.ContainsByPredicate([&Rule](const FSteamInputAction& Action) {
                return FKey(Action.ActionName) == Rule.Key;
            }))
        {
            UE_LOG(LogTemp, Log, TEXT("Steam KeyEvent: %s -> %d"), *Rule.Key.ToString(), (int32)Rule.Value);
            SteamBindingCount++;
        }
    }
    
    for (const auto& Rule : NavConfig->KeyActionRules)
    {
        if (Rule.Key.GetFName().ToString().Contains("menu_") ||
            Keys.ContainsByPredicate([&Rule](const FSteamInputAction& Action) {
                return FKey(Action.ActionName) == Rule.Key;
            }))
        {
            UE_LOG(LogTemp, Log, TEXT("Steam KeyAction: %s -> %d"), *Rule.Key.ToString(), (int32)Rule.Value);
            SteamBindingCount++;
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("Total Steam Input Slate bindings: %d"), SteamBindingCount);
    UE_LOG(LogTemp, Log, TEXT("Configured SlateNavigationBindings: %d"), SlateNavigationBindings.Num());
    
    // Validate that our bindings match what's in Slate
    for (const FSlateNavigationBinding& Binding : SlateNavigationBindings)
    {
        const FKey SteamKey(Binding.SteamActionName);
        bool bFoundInSlate = false;
        
        if (Binding.NavigationType != EUINavigationOptions::Invalid && FNavigationOptionHelper::IsNavigation(Binding.NavigationType))
        {
            if (const EUINavigation* Found = NavConfig->KeyEventRules.Find(SteamKey))
            {
                if (*Found == FNavigationOptionHelper::ToNavigation(Binding.NavigationType))
                {
                    UE_LOG(LogTemp, Log, TEXT("✓ Navigation binding valid: %s"), *Binding.SteamActionName.ToString());
                    bFoundInSlate = true;
                }
            }
        }
        
        if (Binding.NavigationType != EUINavigationOptions::Invalid && FNavigationOptionHelper::IsAction(Binding.NavigationType))
        {
            if (const EUINavigationAction* Found = NavConfig->KeyActionRules.Find(SteamKey))
            {
                if (*Found == FNavigationOptionHelper::ToAction(Binding.NavigationType))
                {
                    UE_LOG(LogTemp, Log, TEXT("✓ Action binding valid: %s"), *Binding.SteamActionName.ToString());
                    bFoundInSlate = true;
                }
            }
        }
        
        if (!bFoundInSlate)
        {
            UE_LOG(LogTemp, Warning, TEXT("✗ Binding not found in Slate: %s"), *Binding.SteamActionName.ToString());
        }
    }
}

void USteamInputSettings::LogCurrentSlateConfig()
{
	if (!FSlateApplication::IsInitialized()) return;
    
	TSharedRef<FNavigationConfig> NavConfig = FSlateApplication::Get().GetNavigationConfig();
    
	UE_LOG(LogTemp, Log, TEXT("=== Current Slate Navigation Config ==="));
    
	for (const auto& Rule : NavConfig->KeyEventRules)
	{
		UE_LOG(LogTemp, Log, TEXT("KeyEvent: %s -> %s"), 
			   *Rule.Key.ToString(), 
			   *UEnum::GetValueAsString(Rule.Value));
	}
    
	for (const auto& Rule : NavConfig->KeyActionRules)
	{
		UE_LOG(LogTemp, Log, TEXT("KeyAction: %s -> %s"), 
			   *Rule.Key.ToString(), 
			   *UEnum::GetValueAsString(Rule.Value));
	}
}

#endif
