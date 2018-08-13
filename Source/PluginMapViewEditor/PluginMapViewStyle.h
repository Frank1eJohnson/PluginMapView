// Copyright 2017 Zhongqi Shan. All Rights Reserved.

#pragma once

/** PluginMapView Editor Style Helper Class. */
class FPluginMapViewStyle
{
public:

	static void Initialize();
	static void Shutdown();

	static TSharedPtr< class ISlateStyle > Get();

private:

	static FString InContent(const FString& RelativePath, const ANSICHAR* Extension);

private:

	static TSharedPtr< class FSlateStyleSet > StyleSet;
};
