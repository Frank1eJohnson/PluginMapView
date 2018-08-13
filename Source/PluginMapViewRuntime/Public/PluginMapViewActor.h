// Copyright 2017 Zhongqi Shan. All Rights Reserved.

#pragma once

#include "PluginMapViewActor.generated.h"

/** An actor that renders a street map mesh component */
UCLASS(hidecategories = (Physics)) // Physics category in detail panel is hidden. Our component/Actor is not simulated !
class PLUGINMAPVIEWRUNTIME_API APluginMapViewActor : public AActor
{
	GENERATED_UCLASS_BODY()

	/**  Component that represents a section of street map roads and buildings */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PluginMapView")
		class UPluginMapViewComponent* PluginMapViewComponent;
};
