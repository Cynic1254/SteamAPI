#pragma once

#include "CoreMinimal.h"
#include "SteamSharedModule.h"
#include "Modules/ModuleManager.h"

class FSteamCoreModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    virtual bool SupportsDynamicReloading() override {return false;}

    static FSteamCoreModule& Get()
    {
        return FModuleManager::LoadModuleChecked<class FSteamCoreModule>("SteamCore");
    };

    bool IsInitialized() const {return Initialized;}
    
private:
    TSharedPtr<class FSteamClientInstanceHandler> ClientHandle;
    bool Initialized = false;
};
