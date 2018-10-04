// Copyright 2017 Zhongqi Shan. All Rights Reserved.

// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PluginMapViewImporting.h"
#include "AssetData.h"
#include "PluginMapViewActorFactory.h"
#include "PluginMapViewActor.h"
#include "PluginMapViewComponent.h"
#include "PluginMapView.h"


//////////////////////////////////////////////////////////////////////////
// UPluginMapViewActorFactory

UPluginMapViewActorFactory::UPluginMapViewActorFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DisplayName = NSLOCTEXT("PluginMapView", "PluginMapViewFactoryDisplayName", "Add PluginMapView Actor");
	NewActorClass = APluginMapViewActor::StaticClass();
}

void UPluginMapViewActorFactory::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	Super::PostSpawnActor(Asset, NewActor);

	if (UPluginMapView* PluginMapViewAsset = Cast<UPluginMapView>(Asset))
	{
		APluginMapViewActor* PluginMapViewActor = CastChecked<APluginMapViewActor>(NewActor);
		UPluginMapViewComponent* PluginMapViewComponent = PluginMapViewActor->GetPluginMapViewComponent();
		PluginMapViewComponent->SetPluginMapView(PluginMapViewAsset, false, true);
	}
}

void UPluginMapViewActorFactory::PostCreateBlueprint(UObject* Asset, AActor* CDO)
{
	if (Asset != nullptr && CDO != nullptr)
	{
		UPluginMapView* PluginMapViewAsset = CastChecked<UPluginMapView>(Asset);
		APluginMapViewActor* PluginMapViewActor = CastChecked<APluginMapViewActor>(CDO);
		UPluginMapViewComponent* PluginMapViewComponent = PluginMapViewActor->GetPluginMapViewComponent();
		PluginMapViewComponent->SetPluginMapView(PluginMapViewAsset, true, false);
	}
}

bool UPluginMapViewActorFactory::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg)
{
	return (AssetData.IsValid() && AssetData.GetClass()->IsChildOf(UPluginMapView::StaticClass()));
}
