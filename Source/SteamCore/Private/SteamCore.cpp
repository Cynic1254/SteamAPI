#include "SteamCore.h"

#include "steam/isteamutils.h"

#define LOCTEXT_NAMESPACE "FSteamCoreModule"

DEFINE_LOG_CATEGORY_STATIC(LogSteamCore, Log, All);

extern "C" void __cdecl SteamAPIDebugTextHook( int nSeverity, const char *pchDebugText )
{
	switch (nSeverity)
	{
	case 0:
		UE_LOG(LogSteamCore, Log, TEXT("%hs"), pchDebugText);
		break;
	case 1:
		UE_LOG(LogSteamCore, Warning, TEXT("%hs"), pchDebugText);
		break;
	default:
		UE_LOG(LogSteamCore, Error, TEXT("%hs"), pchDebugText);
		break;
	}
}

void FSteamCoreModule::StartupModule()
{
	ClientHandle = FSteamSharedModule::Get().ObtainSteamClientInstanceHandle();
	
	SteamUtils()->SetWarningMessageHook(&SteamAPIDebugTextHook);
	UE_LOG(LogSteamCore, Log, TEXT("Steam Warnings hooked"));

	Initialized = true;
}

void FSteamCoreModule::ShutdownModule()
{
	ClientHandle.Reset();
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FSteamCoreModule, SteamCore)