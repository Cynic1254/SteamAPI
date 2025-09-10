// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "SteamCoreSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class STEAMCORE_API USteamCoreSubsystem : public UEngineSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
public:
	// Subsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// TickableObject interface
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual ETickableTickType GetTickableTickType() const override {return ETickableTickType::Always; }
	virtual bool IsTickableWhenPaused() const override {return true;}
	virtual bool IsTickableInEditor() const override {return true;}
	
private:
	TSharedPtr<class FSteamClientInstanceHandler> SteamClientHandle = nullptr;
};
