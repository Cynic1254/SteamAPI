// Fill out your copyright notice in the Description page of Project Settings.

#include "SteamCoreSubsystem.h"

#include "SteamSharedModule.h"
#include "steam/steam_api_common.h"

void USteamCoreSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	SteamClientHandle = FSteamSharedModule::Get().ObtainSteamClientInstanceHandle();
}

void USteamCoreSubsystem::Deinitialize()
{
	Super::Deinitialize();

	SteamClientHandle.Reset();
}

void USteamCoreSubsystem::Tick(float DeltaTime)
{
	SteamAPI_RunCallbacks();
}

TStatId USteamCoreSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USteamCoreSubsystem, STATGROUP_Tickables)
}
