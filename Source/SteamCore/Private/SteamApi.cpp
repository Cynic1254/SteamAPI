// Copyright 2026 Cynic. All Rights Reserved.
// Licensed under the Apache License, Version 2.0.
// See LICENSE file in the project root for full license information.


#include "SteamApi.h"

#include "steam/steam_api.h"
#include "Globals.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

bool USteamApi::RestartAppIfNecessary(const int AppID)
{
	if (SteamAPI_RestartAppIfNecessary(AppID))
	{
		UE_LOG(SteamCoreLog, Log, TEXT("Steam requesting app restart"));

		if (!GIsEditor || GWorld->WorldType == EWorldType::PIE)
		{
			RequestGameExit();
			return true;
		}

		UE_LOG(SteamCoreLog, Warning, TEXT("Restart requested but running in editor - ignoring"));
		return false;
	}

	return false;
}

int USteamApi::GetAppID()
{
	if (SteamUtils())
	{
		return SteamUtils()->GetAppID();
	}
	
	return 0;
}

void USteamApi::RequestGameExit()
{
	if (GEngine)
	{
		if (GIsEditor && GWorld && GWorld->WorldType == EWorldType::PIE)
		{
			GEngine->Exec(GWorld, TEXT("Exit"));
		}
		else if (!GIsEditor)
		{
			FPlatformMisc::RequestExit(false);
		}
	}
}
