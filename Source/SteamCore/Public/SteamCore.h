// Copyright 2026 Cynic. All Rights Reserved.
// Licensed under the Apache License, Version 2.0.
// See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "SteamSharedModule.h"
#include "Modules/ModuleManager.h"
#include "Containers/Ticker.h"

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

    virtual bool Tick( float DeltaTime );
    
    //Steam needs to have regular tick updates
    FTSTicker::FDelegateHandle TickHandle;
};
