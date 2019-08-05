// Copyright 2017 Zhongqi Shan. All Rights Reserved.

#include "PluginMapViewImporting.h"
#include "PluginMapViewFactory.h"
#include "OSMFile.h"
#include "PluginMapView.h"


// Latitude/longitude scale factor
//			- https://en.wikipedia.org/wiki/Equator#Exact_length
static const double EarthCircumference = 40075036.0;
const double UPluginMapViewFactory::LatitudeLongitudeScale = EarthCircumference / 360.0; // meters per degree


UPluginMapViewFactory::UPluginMapViewFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UPluginMapView::StaticClass();

	Formats.Add( TEXT( "osm;OpenPluginMapView XML" ) );
	bCreateNew = false;
	bEditorImport = true;
	bEditAfterNew = false;
	bText = true;
}


UObject* UPluginMapViewFactory::FactoryCreateText( UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const TCHAR*& Buffer, const TCHAR* BufferEnd, FFeedbackContext* Warn )
{
	UPluginMapView* PluginMapView = NewObject<UPluginMapView>( Parent, Name, Flags | RF_Transactional );

	PluginMapView->AssetImportData->Update( this->GetCurrentFilename() );

	// @todo: Performance: This will copy the entire text buffer into an FString.  We need to do this
	//        because the FFastXml parser is expecting a buffer that it can mutate as it parses.
	const int32 CharacterCount = BufferEnd - Buffer;
	FString MutableTextBuffer( CharacterCount, Buffer );

	const bool bIsFilePathActuallyTextBuffer = true;
	const bool bLoadedOkay = LoadFromOpenPluginMapViewXMLFile( PluginMapView, MutableTextBuffer, bIsFilePathActuallyTextBuffer, Warn );

	if( !bLoadedOkay )
	{
		PluginMapView->MarkPendingKill();
		PluginMapView = nullptr;
	}

	return PluginMapView;
}


bool UPluginMapViewFactory::LoadFromOpenPluginMapViewXMLFile( UPluginMapView* PluginMapView, FString& OSMFilePath, const bool bIsFilePathActuallyTextBuffer, FFeedbackContext* FeedbackContext )
{
	// OSM data is stored in meters.  This is the scale factor to convert those units into UE4's native units (cm)
	// Keep in mind that if this is changed, UPluginMapViewComponent sizes for roads may need to be updated too!
	// @todo: We should make this scale factor customizable as an import option
	const float OSMToCentimetersScaleFactor = 100.0f;


	// Converts latitude to meters
	auto ConvertLatitudeToMeters = []( const double Latitude ) -> double
	{
		return -Latitude * LatitudeLongitudeScale;
	};

	// Converts longitude to meters
	auto ConvertLongitudeToMeters = []( const double Longitude, const double Latitude ) -> double
	{
		return Longitude * LatitudeLongitudeScale * FMath::Cos( FMath::DegreesToRadians( Latitude ) );
	};

	// Converts latitude and longitude to X/Y coordinates, relative to some other latitude/longitude
	auto ConvertLatLongToMetersRelative = [ConvertLatitudeToMeters, ConvertLongitudeToMeters]( 
		const double Latitude, 
		const double Longitude, 
		const double RelativeToLatitude, 
		const double RelativeToLongitude ) -> FVector2D
	{
		// Applies Sanson-Flamsteed (sinusoidal) Projection (see http://www.progonos.com/furuti/MapProj/Normal/CartHow/HowSanson/howSanson.html)
		return FVector2D(
			(float)( ConvertLongitudeToMeters( Longitude, Latitude ) - ConvertLongitudeToMeters( RelativeToLongitude, Latitude ) ),
			(float)( ConvertLatitudeToMeters( Latitude ) - ConvertLatitudeToMeters( RelativeToLatitude ) ) );
	};

	// Adds a road to the street map using the OpenPluginMapView data, flattening the road's coordinates into our map's space
	auto AddRoadForWay = [ConvertLatLongToMetersRelative, OSMToCentimetersScaleFactor]( 
		const FOSMFile& OSMFile, 
		UPluginMapView& PluginMapViewRef, 
		const FOSMFile::FOSMWayInfo& OSMWay, 
		int32& OutRoadIndex ) -> bool
	{
		EPluginMapViewRoadType RoadType = EPluginMapViewRoadType::Other;
		switch( OSMWay.WayType )
		{
			case FOSMFile::EOSMWayType::Motorway:
			case FOSMFile::EOSMWayType::Motorway_Link:
			case FOSMFile::EOSMWayType::Trunk:
			case FOSMFile::EOSMWayType::Trunk_Link:
			case FOSMFile::EOSMWayType::Primary:
			case FOSMFile::EOSMWayType::Primary_Link:
				RoadType = EPluginMapViewRoadType::Highway;
				break;

			case FOSMFile::EOSMWayType::Secondary:
			case FOSMFile::EOSMWayType::Secondary_Link:
			case FOSMFile::EOSMWayType::Tertiary:
			case FOSMFile::EOSMWayType::Tertiary_Link:
				RoadType = EPluginMapViewRoadType::MajorRoad;
				break;

			case FOSMFile::EOSMWayType::Residential:
			case FOSMFile::EOSMWayType::Service:
			case FOSMFile::EOSMWayType::Unclassified:
			case FOSMFile::EOSMWayType::Road:	// @todo: Consider excluding "Road" from our data set, as it could be a highway that wasn't properly tagged in OSM yet
				RoadType = EPluginMapViewRoadType::Street;
				break;
		}

		if( RoadType != EPluginMapViewRoadType::Other )
		{
			// Require at least two points!
			if( OSMWay.Nodes.Num() > 1 )
			{
				// Create a road for this way
				OutRoadIndex = PluginMapViewRef.Roads.Num();
				FPluginMapViewRoad& NewRoad = *new( PluginMapViewRef.Roads )FPluginMapViewRoad();

				FVector2D BoundsMin( TNumericLimits<float>::Max(), TNumericLimits<float>::Max() );
				FVector2D BoundsMax( TNumericLimits<float>::Lowest(), TNumericLimits<float>::Lowest() );

				NewRoad.RoadPoints.AddUninitialized( OSMWay.Nodes.Num() );
				int32 CurRoadPoint = 0;

				// Set defaults for each node index on this road.  INDEX_NONE means the node is not valid, which may be the case
				// for nodes that we filter out entirely.  This will be filled in by valid indices to nodes later on.
				NewRoad.NodeIndices.AddUninitialized( OSMWay.Nodes.Num() );
				for( int32& NodeIndex : NewRoad.NodeIndices )
				{
					NodeIndex = INDEX_NONE;
				}


				for( const FOSMFile::FOSMNodeInfo* OSMNodePtr : OSMWay.Nodes )
				{
					const FOSMFile::FOSMNodeInfo& OSMNode = *OSMNodePtr;

					// Transform all points relative to the center of the latitude/longitude bounds, so that
					// we get as much precision as possible.
					const double RelativeToLatitude = OSMFile.AverageLatitude;
					const double RelativeToLongitude = OSMFile.AverageLongitude;
					const FVector2D NodePos = ConvertLatLongToMetersRelative(
						OSMNode.Latitude,
						OSMNode.Longitude,
						RelativeToLatitude,
						RelativeToLongitude ) * OSMToCentimetersScaleFactor;

					// Update bounding box
					{
						if( NodePos.X < BoundsMin.X )
						{
							BoundsMin.X = NodePos.X;
						}
						if( NodePos.Y < BoundsMin.Y )
						{
							BoundsMin.Y = NodePos.Y;
						}
						if( NodePos.X > BoundsMax.X )
						{
							BoundsMax.X = NodePos.X;
						}
						if( NodePos.Y > BoundsMax.Y )
						{
							BoundsMax.Y = NodePos.Y;
						}
					}

					// Fill in the points
					NewRoad.RoadPoints[ CurRoadPoint++ ] = NodePos;
				}


				NewRoad.RoadName = OSMWay.Name;
				if( NewRoad.RoadName.IsEmpty() )
				{
					NewRoad.RoadName = OSMWay.Ref;
				}
				NewRoad.RoadType = RoadType;
				NewRoad.BoundsMin = BoundsMin;
				NewRoad.BoundsMax = BoundsMax;

				NewRoad.bIsOneWay = OSMWay.bIsOneWay;

				PluginMapViewRef.BoundsMin.X = FMath::Min( PluginMapViewRef.BoundsMin.X, BoundsMin.X );
				PluginMapViewRef.BoundsMin.Y = FMath::Min( PluginMapViewRef.BoundsMin.Y, BoundsMin.Y );
				PluginMapViewRef.BoundsMax.X = FMath::Max( PluginMapViewRef.BoundsMax.X, BoundsMax.X );
				PluginMapViewRef.BoundsMax.Y = FMath::Max( PluginMapViewRef.BoundsMax.Y, BoundsMax.Y );

				return true;
			}
			else
			{
				// NOTE: Skipped adding road for way because it has less than 2 points
				// @todo: Log this for the user as an import warning
			}
		}

		return false;
	};


	// Adds a building to the street map using the OpenPluginMapView data, flattening the road's coordinates into our map's space
	auto AddBuildingForWay = [ConvertLatLongToMetersRelative, OSMToCentimetersScaleFactor]( 
		const FOSMFile& OSMFile, 
		UPluginMapView& PluginMapViewRef, 
		const FOSMFile::FOSMWayInfo& OSMWay ) -> bool
	{
		if( OSMWay.WayType == FOSMFile::EOSMWayType::Building )
		{
			// Require at least three points so that we don't have degenerate polygon!
			if( OSMWay.Nodes.Num() > 2 )
			{
				// Create a building for this way
				FPluginMapViewBuilding& NewBuilding = *new( PluginMapViewRef.Buildings )FPluginMapViewBuilding();

				FVector2D BoundsMin( TNumericLimits<float>::Max(), TNumericLimits<float>::Max() );
				FVector2D BoundsMax( TNumericLimits<float>::Lowest(), TNumericLimits<float>::Lowest() );

				NewBuilding.BuildingPoints.AddUninitialized( OSMWay.Nodes.Num() );
				int32 CurBuildingPoint = 0;

				for( const FOSMFile::FOSMNodeInfo* OSMNodePtr : OSMWay.Nodes )
				{
					const FOSMFile::FOSMNodeInfo& OSMNode = *OSMNodePtr;

					// Transform all points relative to the center of the latitude/longitude bounds, so that
					// we get as much precision as possible.
					const double RelativeToLatitude = OSMFile.AverageLatitude;
					const double RelativeToLongitude = OSMFile.AverageLongitude;
					const FVector2D NodePos = ConvertLatLongToMetersRelative(
						OSMNode.Latitude,
						OSMNode.Longitude,
						RelativeToLatitude,
						RelativeToLongitude ) * OSMToCentimetersScaleFactor;

					// Update bounding box
					{
						if( NodePos.X < BoundsMin.X )
						{
							BoundsMin.X = NodePos.X;
						}
						if( NodePos.Y < BoundsMin.Y )
						{
							BoundsMin.Y = NodePos.Y;
						}
						if( NodePos.X > BoundsMax.X )
						{
							BoundsMax.X = NodePos.X;
						}
						if( NodePos.Y > BoundsMax.Y )
						{
							BoundsMax.Y = NodePos.Y;
						}
					}

					// Fill in the points
					NewBuilding.BuildingPoints[ CurBuildingPoint++ ] = NodePos;
				}

				// Make sure the building ended up with a closed polygon, then remove the final (redundant) point
				const bool bIsClosed = NewBuilding.BuildingPoints[ 0 ].Equals( NewBuilding.BuildingPoints[ NewBuilding.BuildingPoints.Num() - 1 ], KINDA_SMALL_NUMBER );
				if( bIsClosed )
				{
					// Remove the final redundant point
					NewBuilding.BuildingPoints.Pop();
				}
				else
				{
					// Wasn't expecting to have an unclosed shape.  Our tolerances might be off, or the data was malformed.
					// Either way, it shouldn't be a problem as we'll close the shape ourselves below.
					// @todo: Log this for the user as an import warning
				}

				NewBuilding.BuildingName = OSMWay.Name;
				if( NewBuilding.BuildingName.IsEmpty() )
				{
					NewBuilding.BuildingName = OSMWay.Ref;
				}

				NewBuilding.Height = OSMWay.Height * OSMToCentimetersScaleFactor;

				NewBuilding.BoundsMin = BoundsMin;
				NewBuilding.BoundsMax = BoundsMax;

				PluginMapViewRef.BoundsMin.X = FMath::Min( PluginMapViewRef.BoundsMin.X, BoundsMin.X );
				PluginMapViewRef.BoundsMin.Y = FMath::Min( PluginMapViewRef.BoundsMin.Y, BoundsMin.Y );
				PluginMapViewRef.BoundsMax.X = FMath::Max( PluginMapViewRef.BoundsMax.X, BoundsMax.X );
				PluginMapViewRef.BoundsMax.Y = FMath::Max( PluginMapViewRef.BoundsMax.Y, BoundsMax.Y );

				return true;
			}
			else
			{
				// NOTE: Skipped adding building for way because it has less than 3 points
				// @todo: Log this for the user as an import warning
			}
		}

		return false;
	};


	// Load up the OSM file.  It's in XML format.
	FOSMFile OSMFile;
	if( !OSMFile.LoadOpenPluginMapViewFile( OSMFilePath, bIsFilePathActuallyTextBuffer, FeedbackContext ) )
	{
		// Loading failed.  The actual error message will be sent to the FeedbackContext's log.
		return false;
	}

	// @todo: The loaded OSMFile stores data in double precision, but our runtime representation (UPluginMapView)
	//        truncates everything to single precision, after transposing coordinates to be relative to the
	//        center of the map's 2D bounds.  Large maps will suffer from floating point precision issues.
	//        To solve this we'd need to either store everything in double precision, or store map elements
	//        in integral grid cells with coordinates relative to their cell.  Of course, there will be many
	//        other considerations for handling huge maps (loading, rendering, collision, etc.)

	// Maps OSMWayInfos to the RoadIndex we created for that way
	TMap< const FOSMFile::FOSMWayInfo*, int32 > OSMWayToRoadIndexMap;

	PluginMapView->BoundsMin = FVector2D( TNumericLimits<float>::Max(), TNumericLimits<float>::Max() );
	PluginMapView->BoundsMax = FVector2D( TNumericLimits<float>::Lowest(), TNumericLimits<float>::Lowest() );

	for( const FOSMFile::FOSMWayInfo* OSMWay : OSMFile.Ways )
	{
		// Handle buildings differently than roads
		if( OSMWay->WayType == FOSMFile::EOSMWayType::Building )
		{
			if( AddBuildingForWay( OSMFile, *PluginMapView, *OSMWay ) )
			{
				// ...
			}
		}
		else
		{
			int32 RoadIndex = INDEX_NONE;
			if( AddRoadForWay( OSMFile, *PluginMapView, *OSMWay, RoadIndex ) )
			{
				OSMWayToRoadIndexMap.Add( OSMWay, RoadIndex );
			}
		}
	}

	for( const auto& NodeMapHashPair : OSMFile.NodeMap )
	{
		const FOSMFile::FOSMNodeInfo& OSMNode = *NodeMapHashPair.Value;

		// Any ways touching this node?
		if( OSMNode.WayRefs.Num() > 0 )
		{
			FPluginMapViewNode NewNode;

			for( const FOSMFile::FOSMWayRef& OSMWayRef : OSMNode.WayRefs )
			{
				const int32* FoundRoundIndexPtr = OSMWayToRoadIndexMap.Find( OSMWayRef.Way );
				if( FoundRoundIndexPtr != nullptr )
				{
					const int32 FoundRoadIndex = *FoundRoundIndexPtr;

					FPluginMapViewRoadRef RoadRef;
					RoadRef.RoadIndex = FoundRoadIndex;

					const int32 RoadPointIndex = OSMWayRef.NodeIndex;
					RoadRef.RoadPointIndex = RoadPointIndex;
					NewNode.RoadRefs.Add( RoadRef );
				}
				else
				{
					// Skipped ref because we didn't keep this road in our data set							
				}
			}

			// Only store nodes that are attached to at least one road.  We must have at least a connection to a single
			// road, otherwise we've filtered this node's road out and there's no point in wasting memory on the node itself.
			if( NewNode.RoadRefs.Num() > 0 )
			{
				// Most nodes from OpenPluginMapView will only be touching a single road.  These nodes usually make up the points
				// along the length of the road, even for roads with no intersections except at the beginning and end.  We
				// don't need to store these points unless they are at the ends of the road.  Keeping the points at the
				// beginning and end of the road is useful when calculating navigation data, but the other nodes can go!
				// In the road's NodeIndices array, any nodes we filter out here will simply have an INDEX_NONE value in that
				// array, and we'll only store the positions of the road at these points in the road's RoadPoints array.

				const FPluginMapViewRoadRef& FirstRoadRef = NewNode.RoadRefs[ 0 ];
				const FPluginMapViewRoad& FirstRoad = PluginMapView->Roads[ FirstRoadRef.RoadIndex ];

				if( NewNode.RoadRefs.Num() > 1 ||					// Does the node connect to more than one road?
					FirstRoadRef.RoadPointIndex == 0 ||				// Does the node connect to the beginning of the road?
					FirstRoadRef.RoadPointIndex == ( FirstRoad.NodeIndices.Num() - 1 ) )	// Does the node connect to the end of the road?
				{
					const int32 NewNodeIndex = PluginMapView->Nodes.Num();
					PluginMapView->Nodes.Add( NewNode );

					// Update the roads that are overlapping this node
					for( const FPluginMapViewRoadRef& RoadRef : NewNode.RoadRefs )
					{
						FPluginMapViewRoad& Road = PluginMapView->Roads[ RoadRef.RoadIndex ];
						check( Road.NodeIndices[ RoadRef.RoadPointIndex ] == INDEX_NONE );
						Road.NodeIndices[ RoadRef.RoadPointIndex ] = NewNodeIndex;
					}
				}
				else
				{
					// Node has only one road that is references, and it wasn't the beginning or end of the road, so filter it out!
				}
			}
			else
			{
				// Node doesn't reference any roads that we kept, or the data was malformed.  Filter it out.
			}
		}
	}

	// Validation test: Make sure that all roads have at least two nodes referencing them, one at the beginning and
	// one at the end.
	for( const FPluginMapViewRoad& Road : PluginMapView->Roads )
	{
		const bool bHasNodeAtBeginning = Road.NodeIndices[ 0 ] != INDEX_NONE;
		const bool bHasNodeAtEnd = Road.NodeIndices[ Road.NodeIndices.Num() - 1 ] != INDEX_NONE;

		// All roads should have at least two nodes referencing them, one at the beginning and one at the end
		ensure( bHasNodeAtBeginning && bHasNodeAtEnd );
	}

	return true;
}


