// Copyright 2017 Zhongqi Shan. All Rights Reserved.

#include "PluginMapViewEditor.h"
#include "PluginMapViewStyle.h"



class FPluginMapViewEditor : public IModuleInterface
{

public:

	// IModuleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};


IMPLEMENT_MODULE(FPluginMapViewEditor, PluginMapViewEditor)



void FPluginMapViewEditor::StartupModule()
{
	FPluginMapViewStyle::Initialize();

	// Register PluginMapViewComponent Detail Customization
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout("PluginMapViewComponent", FOnGetDetailCustomizationInstance::CreateStatic(&FPluginMapViewComponentDetails::MakeInstance));
	PropertyModule.NotifyCustomizationModuleChanged();
}


void FPluginMapViewEditor::ShutdownModule()
{

	FPluginMapViewStyle::Shutdown();

	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomClassLayout("PluginMapViewComponent");
		PropertyModule.NotifyCustomizationModuleChanged();
	}
}
