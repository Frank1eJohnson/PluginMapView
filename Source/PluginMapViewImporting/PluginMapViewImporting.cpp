// Copyright 2017 Zhongqi Shan. All Rights Reserved.

#include "PluginMapViewImporting.h"
#include "PluginMapViewAssetTypeActions.h"
#include "ModuleManager.h"


class FPluginMapViewImportingModule : public IModuleInterface
{

public:

	// IModuleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TSharedPtr< FPluginMapViewAssetTypeActions > PluginMapViewAssetTypeActions;
};


IMPLEMENT_MODULE( FPluginMapViewImportingModule, PluginMapViewImporting )



void FPluginMapViewImportingModule::StartupModule()
{
	// Register asset types
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>( "AssetTools" ).Get();
	PluginMapViewAssetTypeActions = MakeShareable( new FPluginMapViewAssetTypeActions() );
	AssetTools.RegisterAssetTypeActions( PluginMapViewAssetTypeActions.ToSharedRef() );
}


void FPluginMapViewImportingModule::ShutdownModule()
{
	// Unregister all the asset types that we registered
	if( FModuleManager::Get().IsModuleLoaded( "AssetTools" ) )
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" ).Get();
		AssetTools.UnregisterAssetTypeActions( PluginMapViewAssetTypeActions.ToSharedRef() );
		PluginMapViewAssetTypeActions.Reset();
	}
}
