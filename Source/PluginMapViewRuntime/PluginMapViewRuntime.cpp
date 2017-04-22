// Copyright 2017 Zhongqi Shan. All Rights Reserved.

#include "PluginMapViewRuntime.h"
#include "ModuleManager.h"


class FPluginMapViewRuntimeModule : public IModuleInterface
{

public:

	// IModuleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};


IMPLEMENT_MODULE( FPluginMapViewRuntimeModule, PluginMapViewRuntime )



void FPluginMapViewRuntimeModule::StartupModule()
{
}


void FPluginMapViewRuntimeModule::ShutdownModule()
{
}

