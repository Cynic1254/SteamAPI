#include "SteamCore.h"

#include "SteamSharedModule.h"

#define LOCTEXT_NAMESPACE "FSteamCoreModule"

void FSteamCoreModule::StartupModule()
{
}

void FSteamCoreModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FSteamCoreModule, SteamCore)