#pragma once

#include "CoreMinimal.h"
#include "IInputDeviceModule.h"

class FSteamInputModule : public IInputDeviceModule
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    virtual TSharedPtr<class IInputDevice> CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;
private:
    bool bSteamInputInitialized = false;
    
    TSharedPtr<class FSteamInputController> Controller = nullptr;
};
