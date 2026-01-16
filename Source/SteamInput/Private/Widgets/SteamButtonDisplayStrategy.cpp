// Copyright 2026 Cynic. All Rights Reserved.


#include "Widgets/SteamButtonDisplayStrategy.h"

#include "Helper/SteamInputFunctionLibrary.h"
#include "Engine/Texture2D.h"

FSlateBrush USteamButtonDisplayStrategy::CreatePromptBrush_Implementation(
	const TArray<FSteamInputActionOrigin>& ActionOrigins, const FSlateBrush& FallbackBrush)
{
	if (ActionOrigins.Num() == 0)
	{
		return FallbackBrush;
	}

	UTexture2D* Texture = USteamInputFunctionLibrary::GetTextureFromActionOrigin(ActionOrigins[0]);
	
	if (!Texture)
	{
		return FallbackBrush;
	}

	FSlateBrush Brush;
	Brush.SetResourceObject(Texture);
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.Tiling = ESlateBrushTileType::NoTile;
	
	return Brush;
}
