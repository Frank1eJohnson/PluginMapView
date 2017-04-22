// Copyright 2017 Zhongqi Shan. All Rights Reserved.
#pragma once

#include "Factories/Factory.h"
#include "PluginMapViewFactory.generated.h"


/**
 * Import factory object for OpenPluginMapView assets
 */
UCLASS()
class UPluginMapViewFactory : public UFactory
{
	GENERATED_BODY()

public:

	/** UPluginMapViewFactory constructor */
	UPluginMapViewFactory( const class FObjectInitializer& ObjectInitializer );

protected:

	// UFactory overrides
	virtual UObject* FactoryCreateText( UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const TCHAR*& Buffer, const TCHAR* BufferEnd, FFeedbackContext* Warn ) override;

	/** Loads the street map from an OpenPluginMapView XML file.  Note that in the case of the file path containing the XML data, the string must be mutable for us to parse it quickly. */
	bool LoadFromOpenPluginMapViewXMLFile( class UPluginMapView* PluginMapView, FString& OSMFilePath, const bool bIsFilePathActuallyTextBuffer, class FFeedbackContext* FeedbackContext );

	/** Static: Latitude/longitude scale factor */
	static const double LatitudeLongitudeScale;
};

