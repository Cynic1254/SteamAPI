#pragma once

#include "CoreMinimal.h"
#include "IInputDeviceModule.h"

DECLARE_MULTICAST_DELEGATE(SteamInputInitialized);

class FSteamInputModule : public IInputDeviceModule
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    static FSteamInputModule& Get()
    {
        return FModuleManager::LoadModuleChecked<class FSteamInputModule>("SteamInput");
    };
    
    bool BindToOnInputInitialized(const SteamInputInitialized::FDelegate& InNewDelegate);
    
    virtual TSharedPtr<class IInputDevice> CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;
private:
    bool bSteamInputInitialized = false;

    SteamInputInitialized InputInitialized;
    
    TSharedPtr<class FSteamInputController> Controller = nullptr;
    TSharedPtr<class FSteamClientInstanceHandler> ClientHandle = nullptr;

    void InitializeSlateIntegration() const;
};
