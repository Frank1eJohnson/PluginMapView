// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PluginMapViewRuntime.h"
#include "PluginMapViewActor.h"
#include "PluginMapViewComponent.h"


APluginMapViewActor::APluginMapViewActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PluginMapViewComponent = CreateDefaultSubobject<UPluginMapViewComponent>(TEXT("PluginMapViewComp"));
	RootComponent = PluginMapViewComponent;
}
