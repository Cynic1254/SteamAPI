#include "SteamInput.h"

#include "ISettingsModule.h"
#include "Controller/FSteamInputController.h"
#include "Settings/FSteamInputActionCustomization.h"
#include "Settings/SteamInputSettings.h"
#include "steam/isteaminput.h"

#define LOCTEXT_NAMESPACE "FSteamInputModule"

void FSteamInputModule::StartupModule()
{
    IInputDeviceModule::StartupModule();

    if (SteamInput())
    {
	    SteamInput()->Init(false);
    	bSteamInputInitialized = true;
    }

#if WITH_EDITOR
    ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");
	SettingsModule.RegisterSettings("Project", "Plugins", "SteamInput", LOCTEXT("SteamInputName", "Steam Input Settings"), LOCTEXT("SteamInputDescription", "Steam Input Settings"), GetMutableDefault<USteamInputSettings>());
	
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditorModule.RegisterCustomPropertyTypeLayout(
		FSteamInputAction::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FSteamInputActionCustomization::MakeInstance)
		);
#endif
}

void FSteamInputModule::ShutdownModule()
{
    IInputDeviceModule::ShutdownModule();

    if (SteamInput())
    {
	    SteamInput()->Shutdown();
    }

	bSteamInputInitialized = false;

#if WITH_EDITOR
    if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
    {
	    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    	PropertyEditorModule.UnregisterCustomPropertyTypeLayout(FSteamInputAction::StaticStruct()->GetFName());
    }

    if (FModuleManager::Get().IsModuleLoaded("Settings"))
    {
	    ISettingsModule& SettingsModule = FModuleManager::GetModuleChecked<ISettingsModule>("Settings");
    	SettingsModule.UnregisterSettings("Project", "Plugins", "SteamInput");
    }
#endif
}

TSharedPtr<class IInputDevice> FSteamInputModule::CreateInputDevice(
	const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
	if (bSteamInputInitialized)
	{
		Controller = MakeShared<FSteamInputController>(InMessageHandler);
	}

	return Controller;
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FSteamInputModule, SteamInput)