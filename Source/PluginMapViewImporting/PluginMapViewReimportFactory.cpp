// Copyright 2017 Zhongqi Shan. All Rights Reserved.

#include "PluginMapViewImporting.h"
#include "PluginMapViewReimportFactory.h"
#include "PluginMapView.h"


UPluginMapViewReimportFactory::UPluginMapViewReimportFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


bool UPluginMapViewReimportFactory::CanReimport( UObject* Obj, TArray<FString>& OutFilenames )
{
	UPluginMapView* PluginMapView = Cast<UPluginMapView>( Obj );
	if( PluginMapView != nullptr )
	{
		OutFilenames.Add( PluginMapView->AssetImportData->GetFirstFilename() );
	}
	return true;
}


void UPluginMapViewReimportFactory::SetReimportPaths( UObject* Obj, const TArray<FString>& NewReimportPaths )
{
	UPluginMapView* PluginMapView = CastChecked<UPluginMapView>( Obj );
	PluginMapView->Modify();
	PluginMapView->AssetImportData->Update( NewReimportPaths[0] );
}


EReimportResult::Type UPluginMapViewReimportFactory::Reimport( UObject* Obj ) 
{ 
	UPluginMapView* PluginMapView = CastChecked<UPluginMapView>( Obj );

	const FString Filename = PluginMapView->AssetImportData->GetFirstFilename();
	const FString FileExtension = FPaths::GetExtension(Filename);

	// If there is no file path provided, can't reimport from source
	if ( !Filename.Len() )
	{
		return EReimportResult::Failed;
	}

	// Suppress the import overwrite dialog, we want to keep existing settings when re-importing
	USoundFactory::SuppressImportOverwriteDialog();

	if( UFactory::StaticImportObject( PluginMapView->GetClass(), PluginMapView->GetOuter(), *PluginMapView->GetName(), RF_Public|RF_Standalone, *Filename, nullptr, this ) )
	{
		// Mark the package dirty after the successful import
		PluginMapView->MarkPackageDirty();
		return EReimportResult::Succeeded;
	}

	return EReimportResult::Failed;
}


int32 UPluginMapViewReimportFactory::GetPriority() const
{
	return 0;
}
