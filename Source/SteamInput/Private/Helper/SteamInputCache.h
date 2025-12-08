// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "SteamInputCache.generated.h"

struct FStreamableHandle;
/**
 * 
 */
UCLASS()
class STEAMINPUT_API USteamInputCache : public UEngineSubsystem
{
	GENERATED_BODY()
public:
	UTexture2D* GetGlyphTexture(const FString& Origin);
	void ClearCache();
	
	static USteamInputCache* Get()
	{
		return GEngine->GetEngineSubsystem<USteamInputCache>();
	}
private:
	UPROPERTY()
	TMap<FString, UTexture2D*> TextureCache;
	
	// Load texture synchronously
	UTexture2D* LoadGlyph(const FString& Origin);
};
