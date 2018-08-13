// Copyright 2017 Zhongqi Shan. All Rights Reserved.

#include "PluginMapViewEditor.h"
#include "PluginMapViewStyle.h"
#include "SlateStyle.h"
#include "IPluginManager.h"

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( FPluginMapViewStyle::InContent( RelativePath, ".png" ), __VA_ARGS__ )

FString FPluginMapViewStyle::InContent(const FString& RelativePath, const ANSICHAR* Extension)
{
	static FString IconsDir = IPluginManager::Get().FindPlugin(TEXT("PluginMapView"))->GetContentDir() / TEXT("Icons");
	return (IconsDir / RelativePath) + Extension;
}

TSharedPtr< FSlateStyleSet > FPluginMapViewStyle::StyleSet = NULL;
TSharedPtr< class ISlateStyle > FPluginMapViewStyle::Get() { return StyleSet; }

void FPluginMapViewStyle::Initialize()
{
	// Const icon & thumbnail sizes
	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon128x128(128.0f, 128.0f);

	// Only register once
	if (StyleSet.IsValid())
	{
		return;
	}

	StyleSet = MakeShareable(new FSlateStyleSet("PluginMapViewStyle"));
	FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("PluginMapView"))->GetContentDir();
	StyleSet->SetContentRoot(ContentDir);

	StyleSet->Set("ClassIcon.PluginMapView", new IMAGE_BRUSH("sm_asset_icon_32", Icon16x16));
	StyleSet->Set("ClassThumbnail.PluginMapView", new IMAGE_BRUSH("sm_asset_icon_128", Icon128x128));

	StyleSet->Set("ClassIcon.PluginMapViewActor", new IMAGE_BRUSH("sm_actor_icon_32", Icon16x16));
	StyleSet->Set("ClassThumbnail.PluginMapViewActor", new IMAGE_BRUSH("sm_actor_icon_128", Icon128x128));

	StyleSet->Set("ClassIcon.PluginMapViewComponent", new IMAGE_BRUSH("sm_component_icon_32", Icon16x16));
	StyleSet->Set("ClassThumbnail.PluginMapViewComponent", new IMAGE_BRUSH("sm_component_icon_128", Icon128x128));

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
};

#undef IMAGE_BRUSH


void FPluginMapViewStyle::Shutdown()
{
	if (StyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
		ensure(StyleSet.IsUnique());
		StyleSet.Reset();
	}
}
