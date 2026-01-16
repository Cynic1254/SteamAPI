// Copyright 2026 Cynic. All Rights Reserved.


#include "SteamInputCache.h"

#include "ImageUtils.h"
#include "Engine/Engine.h"
#include "Engine/Texture2D.h"

UTexture2D* USteamInputCache::GetGlyphTexture(const FString& Origin)
{
	// Check cache first
	if (UTexture2D** CachedTexture = TextureCache.Find(Origin))
	{
		return *CachedTexture;
	}
	
	return LoadGlyph(Origin);
}

void USteamInputCache::ClearCache()
{
	TextureCache.Empty();
}

USteamInputCache* USteamInputCache::Get()
{
	return GEngine->GetEngineSubsystem<USteamInputCache>();
}

UTexture2D* USteamInputCache::LoadGlyph(const FString& Origin)
{
	return TextureCache.Add(Origin, FImageUtils::ImportFileAsTexture2D(Origin));
}
