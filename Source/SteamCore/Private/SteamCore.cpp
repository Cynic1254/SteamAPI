#include "SteamCore.h"

#include "Globals.h"
#include "steam/isteamutils.h"
#include "steam/steam_api.h"

#define LOCTEXT_NAMESPACE "FSteamCoreModule"

DEFINE_LOG_CATEGORY(SteamCoreLog);

extern "C" void __cdecl SteamAPIDebugTextHook( int nSeverity, const char *pchDebugText )
{
	switch (nSeverity)
	{
	case 0:
		UE_LOG(SteamCoreLog, Log, TEXT("%hs"), pchDebugText);
		break;
	case 1:
		UE_LOG(SteamCoreLog, Warning, TEXT("%hs"), pchDebugText);
		break;
	default:
		UE_LOG(SteamCoreLog, Error, TEXT("%hs"), pchDebugText);
		break;
	}
}

void FSteamCoreModule::StartupModule()
{
	ClientHandle = FSteamSharedModule::Get().ObtainSteamClientInstanceHandle();

	if (!ClientHandle)
		return;
	
	SteamUtils()->SetWarningMessageHook(&SteamAPIDebugTextHook);
	UE_LOG(SteamCoreLog, Log, TEXT("Steam Warnings hooked"));

	TickHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateRaw(this, &FSteamCoreModule::Tick)
		);
	
	Initialized = true;
}

void FSteamCoreModule::ShutdownModule()
{
	ClientHandle.Reset();

	FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
	TickHandle.Reset();
}

bool FSteamCoreModule::Tick(float DeltaTime)
{
	if (Initialized)
	{
		SteamAPI_RunCallbacks();
	}

	return true;
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FSteamCoreModule, SteamCore)