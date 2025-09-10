// Fill out your copyright notice in the Description page of Project Settings.


#include "SteamApi.h"

#include "steam/steam_api.h"
#include "Globals.h"

bool USteamApi::RestartAppIfNecessary(const uint32 AppID)
{
	if (SteamAPI_RestartAppIfNecessary(AppID))
	{
		UE_LOG(SteamLog, Log, TEXT("Steam requesting app restart"));

		if (!GIsEditor || GWorld->WorldType == EWorldType::PIE)
		{
			RequestGameExit();
			return true;
		}

		UE_LOG(SteamLog, Warning, TEXT("Restart requested but running in editor - ignoring"));
		return false;
	}

	return false;
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
