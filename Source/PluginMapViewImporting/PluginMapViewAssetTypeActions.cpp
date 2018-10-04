// Copyright 2017 Zhongqi Shan. All Rights Reserved.

#include "PluginMapViewImporting.h"
#include "PluginMapViewAssetTypeActions.h"
#include "PluginMapView.h"
#include "AssetData.h"


#define LOCTEXT_NAMESPACE "PluginMapViewImporting"


FText FPluginMapViewAssetTypeActions::GetName() const
{
	return LOCTEXT("PluginMapViewAssetTypeActionsName", "Street Map");
}


FColor FPluginMapViewAssetTypeActions::GetTypeColor() const
{
	return FColor(50, 255, 120);
}


UClass* FPluginMapViewAssetTypeActions::GetSupportedClass() const
{
	return UPluginMapView::StaticClass();
}


void FPluginMapViewAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	FSimpleAssetEditor::CreateEditor( EToolkitMode::Standalone, EditWithinLevelEditor, InObjects );
}


uint32 FPluginMapViewAssetTypeActions::GetCategories()
{
	return EAssetTypeCategories::Misc;
}


FText FPluginMapViewAssetTypeActions::GetAssetDescription(const FAssetData& AssetData) const
{
	return FText::GetEmpty();
}


bool FPluginMapViewAssetTypeActions::IsImportedAsset() const
{
	return true;
}


void FPluginMapViewAssetTypeActions::GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const
{
	for (auto* Asset : TypeAssets)
	{
		const auto* PluginMapView = CastChecked<UPluginMapView>(Asset);
		if( !PluginMapView->AssetImportData->GetFirstFilename().IsEmpty() )
		{
			OutSourceFilePaths.Add( PluginMapView->AssetImportData->GetFirstFilename() );
		}
	}
}

#undef LOCTEXT_NAMESPACE
