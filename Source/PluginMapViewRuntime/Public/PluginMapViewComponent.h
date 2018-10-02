// Copyright 2017 Zhongqi Shan. All Rights Reserved.
#pragma once

#include "Components/MeshComponent.h"
#include "PluginMapViewSceneProxy.h"
#include "PluginMapViewComponent.generated.h"


/**
 * Component that represents a section of street map roads and buildings
 */
UCLASS( meta=(BlueprintSpawnableComponent) )
class PLUGINMAPVIEWRUNTIME_API UPluginMapViewComponent : public UMeshComponent
{
	GENERATED_BODY()

public:

	/** UPluginMapViewComponent constructor */
	UPluginMapViewComponent( const class FObjectInitializer& ObjectInitializer );

	/** @return Gets the street map object associated with this component */
	class UPluginMapView* GetPluginMapView()
	{
		return PluginMapView;
	}

	/** 
	 * Assigns a street map asset to this component.  Render state will be updated immediately.
	 * 
	 * @param NewPluginMapView The street map to use
	 * 
	 * @return Sets the street map object 
	 */
	UFUNCTION( BlueprintCallable, Category="PluginMapView" )
	void SetPluginMapView( class UPluginMapView* NewPluginMapView );


	// UPrimitiveComponent interface
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual FBoxSphereBounds CalcBounds( const FTransform& LocalToWorld ) const override;
	virtual int32 GetNumMaterials() const override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
#endif

protected:

	/** Generates a cached mesh from raw street map data */
	void GenerateMesh();

	/** Returns true if we have valid cached mesh data from our assigned street map asset */
	bool HasValidMesh() const
	{
		return Vertices.Num() != 0 && Indices.Num() != 0;
	}
	
	/** Wipes out our cached mesh data.  It will be recreated on demand. */
	void InvalidateMesh();

	/** Rebuilds the graphics and physics mesh representation if we don't have one right now.  Designed to be called on demand. */
	void BuildMeshIfNeeded();

	/** Adds a 2D line to the raw mesh */
	void AddThick2DLine( const FVector2D Start, const FVector2D End, const float Z, const float Thickness, const FColor& StartColor, const FColor& EndColor, FBox& MeshBoundingBox );

	/** Adds 3D triangles to the raw mesh */
	void AddTriangles( const TArray<FVector>& Points, const TArray<int32>& PointIndices, const FVector& ForwardVector, const FVector& UpVector, const FColor& Color, FBox& MeshBoundingBox );


protected:
	
	/** The street map we're representing */
	UPROPERTY( EditAnywhere, Category="PluginMapView" )
	class UPluginMapView* PluginMapView;


	//
	// Cached mesh representation
	//

	/** Cached raw mesh vertices */
	TArray< struct FPluginMapViewVertex > Vertices;

	/** Cached raw mesh triangle indices */
	TArray< uint32 > Indices;

	/** Cached bounding box */
	FBoxSphereBounds CachedLocalBounds;

};
