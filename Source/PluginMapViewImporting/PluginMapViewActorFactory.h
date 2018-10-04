// Copyright 2017 Zhongqi Shan. All Rights Reserved.

#pragma once

#include "PluginMapViewActorFactory.generated.h"

UCLASS()
class UPluginMapViewActorFactory : public UActorFactory
{
	GENERATED_UCLASS_BODY()

		//~ Begin UActorFactory Interface
		virtual void PostSpawnActor(UObject* Asset, AActor* NewActor) override;
	virtual void PostCreateBlueprint(UObject* Asset, AActor* CDO) override;
	virtual bool CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) override;
	//~ End UActorFactory Interface
};
