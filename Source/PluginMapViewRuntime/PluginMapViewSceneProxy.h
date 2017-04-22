// Copyright 2017 Zhongqi Shan. All Rights Reserved.
#pragma once

#include "Runtime/Engine/Public/PrimitiveSceneProxy.h"
#include "Runtime/Engine/Public/LocalVertexFactory.h"


/**	A single vertex on a street map mesh */
struct FPluginMapViewVertex
{
	/** Location of the vertex in local space */
	FVector Position;

	/** Texture coordinate */
	FVector2D TextureCoordinate;

	/** Tangent vector X */
	FPackedNormal TangentX;

	/** Tangent vector Z (normal) */
	FPackedNormal TangentZ;

	/** Color */
	FColor Color;


	/** Default constructor, leaves everything uninitialized */
	FPluginMapViewVertex()
	{
	}

	/** Construct with a supplied position and tangents for the vertex */
	FPluginMapViewVertex( const FVector InitLocation, const FVector2D InitTextureCoordinate, const FVector InitTangentX, const FVector InitTangentZ, const FColor InitColor )
		: Position( InitLocation ),
		  TextureCoordinate( InitTextureCoordinate ),
		  TangentX( InitTangentX ),
		  TangentZ( InitTangentZ ),
		  Color( InitColor )
	{
	}
};


/** Street map mesh vertex buffer */
class FPluginMapViewVertexBuffer : public FVertexBuffer
{

public:

	/** All of the vertices in this mesh */
	TArray< FPluginMapViewVertex > Vertices;


	// FRenderResource interface
	virtual void InitRHI() override;
};


/** Street map mesh index buffer */
class FPluginMapViewIndexBuffer : public FIndexBuffer
{

public:

	/** 16-bit indices */
	TArray< uint16 > Indices16;

	/** 32-bit indices */
	TArray< uint32 > Indices32;


	// FRenderResource interface
	virtual void InitRHI() override;
};


/** Street map mesh vertex factory */
class FPluginMapViewVertexFactory : public FLocalVertexFactory
{

public:

	/** Initialize this vertex factory */
	void InitVertexFactory( const FPluginMapViewVertexBuffer& VertexBuffer );
};


/** Scene proxy for rendering a section of a street map mesh on the rendering thread */
class FPluginMapViewSceneProxy : public FPrimitiveSceneProxy
{

public:

	/** Construct this scene proxy */
	FPluginMapViewSceneProxy( const class UPluginMapViewComponent* InComponent );

	/**
	* Init this street map mesh scene proxy for the specified component (32-bit indices)
	*
	* @param	InComponent			The street map mesh component to initialize this with
	* @param	Vertices			The vertices for this street map mesh
	* @param	Indices				The vertex indices for this street map mesh
	*/
	void Init( const UPluginMapViewComponent* InComponent, const TArray< FPluginMapViewVertex >& Vertices, const TArray< uint32 >& Indices );

	/**
	* Init this street map mesh scene proxy for the specified component (16-bit indices)
	*
	* @param	InComponent			The street map mesh component to initialize this with
	* @param	Vertices			The vertices for this street map mesh
	* @param	Indices				The vertex indices for this street map mesh
	*/
	void Init( const UPluginMapViewComponent* InComponent, const TArray< FPluginMapViewVertex >& Vertices, const TArray< uint16 >& Indices );

	/** Destructor that cleans up our rendering data */
	virtual ~FPluginMapViewSceneProxy();


protected:

	/** Called from the constructor to finish construction after the index buffer is setup */
	void InitAfterIndexBuffer( const class UPluginMapViewComponent* PluginMapViewComponent, const TArray< FPluginMapViewVertex >& Vertices );

	/** Initializes this scene proxy's vertex buffer, index buffer and vertex factory (on the render thread.) */
	void InitResources();

	/** Makes a MeshBatch for rendering.  Called every time the mesh is drawn */
	void MakeMeshBatch( struct FMeshBatch& Mesh, class FMaterialRenderProxy* WireframeMaterialRenderProxyOrNull ) const;

	/** Checks to see if this mesh must be drawn during the dynamic pass.  Note that even when this returns false, we may still
	have other (debug) geometry to render as dynamic */
	bool MustDrawMeshDynamically( const class FSceneView& View ) const;

	// FPrimitiveSceneProxy interface
	virtual void DrawStaticElements( class FStaticPrimitiveDrawInterface* PDI ) override;
	virtual void GetDynamicMeshElements( const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, class FMeshElementCollector& Collector ) const override;
	virtual uint32 GetMemoryFootprint( void ) const override;
	virtual FPrimitiveViewRelevance GetViewRelevance( const class FSceneView* View ) const override;
	virtual bool CanBeOccluded() const override;


protected:

	/** Contains all of the vertices in our street map mesh */
	FPluginMapViewVertexBuffer VertexBuffer;

	/** All of the vertex indices in our street map mesh */
	FPluginMapViewIndexBuffer IndexBuffer;

	/** Our vertex factory specific to street map meshes */
	FPluginMapViewVertexFactory VertexFactory;

	/** Cached material relevance */
	FMaterialRelevance MaterialRelevance;

	/** The material we'll use to render this street map mesh */
	class UMaterialInterface* MaterialInterface;

};
