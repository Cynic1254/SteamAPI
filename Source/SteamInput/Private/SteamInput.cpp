#include "SteamInput.h"

#include "SteamCore.h"
#include "Controller/FSteamInputController.h"
#include "Settings/SettingsInspector.h"
#include "Settings/SteamInputSettings.h"
#include "steam/isteaminput.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#endif

#define LOCTEXT_NAMESPACE "FSteamInputModule"

DEFINE_LOG_CATEGORY_STATIC(SteamInputLog, Log, All);

void FSteamInputModule::StartupModule()
{
    IInputDeviceModule::StartupModule();

	FSteamCoreModule::Get();
	ClientHandle = FSteamSharedModule::Get().ObtainSteamClientInstanceHandle();
	
    if (ClientHandle && FSteamCoreModule::Get().IsInitialized() && SteamInput())
    {
	    if (!SteamInput()->Init(false))
	    {
		    UE_LOG(SteamInputLog, Log, TEXT("Steam Input failed to initialize"));
	    }
    	bSteamInputInitialized = true;
    }

	EKeys::AddMenuCategoryDisplayInfo(GetDefault<USteamInputSettings>()->MenuCategory, LOCTEXT("Steam Keys", "Steam Key Category"), TEXT("GraphEditor.PadEvent_16x"));

	InitializeSlateIntegration();
	
#if WITH_EDITOR
    ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");
	SettingsModule.RegisterSettings("Project", "Plugins", "SteamInput", LOCTEXT("SteamInputName", "Steam Input Settings"), LOCTEXT("SteamInputDescription", "Steam Input Settings"), GetMutableDefault<USteamInputSettings>());

	// Register custom property type customization
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditorModule.RegisterCustomPropertyTypeLayout(
		FSteamInputAction::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FSettingsInspector::MakeInstance)
	);
    
	// Notify that customizations have changed
	PropertyEditorModule.NotifyCustomizationModuleChanged();
#endif

	InputInitialized.Broadcast();
}

void FSteamInputModule::InitializeSlateIntegration() const
{
	UE_LOG(SteamInputLog, Log, TEXT("Initializing Slate integration"));
	
	USteamInputSettings* Settings = GetMutableDefault<USteamInputSettings>();
	
	// If Steam Input is already initialized, set up Slate integration immediately
	if (bSteamInputInitialized)
	{
		if (Settings->bAutoConfigureCommonNavigation)
		{
			Settings->SetupDefaultSlateBindings();
		}
		Settings->UpdateSlateNavigationConfig();
	}
	// Otherwise, it will be set up when Steam Input initializes via the delegate
	
	UE_LOG(SteamInputLog, Log, TEXT("Slate integration initialized"));
}

void FSteamInputModule::ShutdownModule()
{
    IInputDeviceModule::ShutdownModule();

    if (SteamInput())
    {
	    SteamInput()->Shutdown();
    }

	ClientHandle.Reset();
	bSteamInputInitialized = false;

#if WITH_EDITOR
	// Unregister customizations
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

bool FSteamInputModule::BindToOnInputInitialized(const SteamInputInitialized::FDelegate& InNewDelegate)
{
	if (!bSteamInputInitialized)
	{
		InputInitialized.Add(InNewDelegate);
		return false;
	}

	InNewDelegate.ExecuteIfBound();
	return true;
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