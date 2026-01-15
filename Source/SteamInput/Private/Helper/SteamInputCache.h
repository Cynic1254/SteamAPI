// Copyright 2026 Cynic. All Rights Reserved.
// Licensed under the Apache License, Version 2.0.
// See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "SteamInputCache.generated.h"

struct FStreamableHandle;
class UTexture2D;
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
	
	static USteamInputCache* Get();

private:
	UPROPERTY()
	TMap<FString, UTexture2D*> TextureCache;
	
	// Load texture synchronously
	UTexture2D* LoadGlyph(const FString& Origin);
};
