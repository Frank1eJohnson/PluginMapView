// Copyright 2017 Zhongqi Shan. All Rights Reserved.
#pragma once

#include "PluginMapViewFactory.h"
#include "EditorReimportHandler.h"
#include "PluginMapViewReimportFactory.generated.h"


/**
 * Import factory object for OpenPluginMapView assets
 */
UCLASS()
class UPluginMapViewReimportFactory : public UPluginMapViewFactory, public FReimportHandler
{
	GENERATED_BODY()

public:

	/** UPluginMapViewReimportFactory constructor */
	UPluginMapViewReimportFactory( const class FObjectInitializer& ObjectInitializer );

protected:

	// FReimportHandler overrides
	virtual bool CanReimport( UObject* Obj, TArray<FString>& OutFilenames ) override;
	virtual void SetReimportPaths( UObject* Obj, const TArray<FString>& NewReimportPaths ) override;
	virtual EReimportResult::Type Reimport( UObject* Obj ) override;
	virtual int32 GetPriority() const override;

};

