// Copyright 2017 Zhongqi Shan. All Rights Reserved.

#include "PluginMapViewImporting.h"

#include "PluginMapViewComponentDetails.h"

#include "SlateBasics.h"
#include "RawMesh.h"
#include "PropertyEditorModule.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "PropertyCustomizationHelpers.h"
#include "IDetailsView.h"
#include "AssetRegistryModule.h"
#include "DlgPickAssetPath.h"

#include "SNotificationList.h"
#include "NotificationManager.h"
#include "AssertionMacros.h"


#include "PluginMapViewComponent.h"


#define LOCTEXT_NAMESPACE "PluginMapViewComponentDetails"


FPluginMapViewComponentDetails::FPluginMapViewComponentDetails() :
	SelectedPluginMapViewComponent(nullptr),
	LastDetailBuilderPtr(nullptr)
{

}

TSharedRef<IDetailCustomization> FPluginMapViewComponentDetails::MakeInstance()
{
	return MakeShareable(new FPluginMapViewComponentDetails());
}

void FPluginMapViewComponentDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	LastDetailBuilderPtr = &DetailBuilder;

	TArray <TWeakObjectPtr<UObject>> SelectedObjects = DetailBuilder.GetDetailsView().GetSelectedObjects();

	for (const TWeakObjectPtr<UObject>& Object : SelectedObjects)
	{
		UPluginMapViewComponent* TempPluginMapViewComp = Cast<UPluginMapViewComponent>(Object.Get());
		if (TempPluginMapViewComp != nullptr && !TempPluginMapViewComp->IsTemplate())
		{
			SelectedPluginMapViewComponent = TempPluginMapViewComp;
			break;
		}
	}


	if (SelectedPluginMapViewComponent == nullptr)
	{
		TArray<TWeakObjectPtr<AActor>> SelectedActors = DetailBuilder.GetDetailsView().GetSelectedActors();

		for (const TWeakObjectPtr<UObject>& Object : SelectedObjects)
		{
			AActor* TempActor = Cast<AActor>(Object.Get());
			if (TempActor != nullptr && !TempActor->IsTemplate())
			{
				UPluginMapViewComponent* TempPluginMapViewComp = TempActor->FindComponentByClass<UPluginMapViewComponent>();
				if (TempPluginMapViewComp != nullptr && !TempPluginMapViewComp->IsTemplate())
				{
					SelectedPluginMapViewComponent = TempPluginMapViewComp;
					break;
				}
				break;
			}
		}
	}



	if (SelectedPluginMapViewComponent == nullptr)
	{
		return;
	}


	IDetailCategoryBuilder& PluginMapViewCategory = DetailBuilder.EditCategory("PluginMapView", FText::GetEmpty(), ECategoryPriority::Important);
	PluginMapViewCategory.InitiallyCollapsed(false);


	const bool bCanRebuildMesh = HasValidMapObject();
	const bool bCanClearMesh = HasValidMeshData();
	const bool bCanCreateMeshAsset = HasValidMeshData();

	TSharedPtr< SHorizontalBox > TempHorizontalBox;

	PluginMapViewCategory.AddCustomRow(FText::GetEmpty(), false)
		[
			SAssignNew(TempHorizontalBox, SHorizontalBox)
			+ SHorizontalBox::Slot()
		[
			SNew(SButton)
			.ToolTipText(LOCTEXT("GenerateMesh_Tooltip", "Generate a cached mesh from raw street map data."))
		.OnClicked(this, &FPluginMapViewComponentDetails::OnBuildMeshClicked)
		.IsEnabled(bCanRebuildMesh)
		.HAlign(HAlign_Center)
		[
			SNew(STextBlock)
			.Text(bCanClearMesh ? LOCTEXT("RebuildMesh", "Rebuild Mesh") : LOCTEXT("BuildMesh", "Build Mesh"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
		]
		]
		];

	TempHorizontalBox->AddSlot()
		[
			SNew(SButton)
			.ToolTipText(LOCTEXT("ClearMesh_Tooltip", "Clear current mesh data , in case we have a valid mesh "))
		.OnClicked(this, &FPluginMapViewComponentDetails::OnClearMeshClicked)
		.IsEnabled(bCanClearMesh)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ClearMesh", "Clear Mesh"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
		]
		];


	PluginMapViewCategory.AddCustomRow(FText::GetEmpty(), false)
		[
			SAssignNew(TempHorizontalBox, SHorizontalBox)
			+ SHorizontalBox::Slot()
		[
			SNew(SButton)
			.ToolTipText(LOCTEXT("CreateStaticMeshAsset_Tooltip", "Create a new Static Mesh Asset from selected PluginMapViewComponent."))
		.OnClicked(this, &FPluginMapViewComponentDetails::OnCreateStaticMeshAssetClicked)
		.IsEnabled(bCanCreateMeshAsset)
		.HAlign(HAlign_Center)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("CreateStaticMeshAsset", "Create Static Mesh Asset"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
		]
		]
		];

	if (bCanCreateMeshAsset)
	{

		const int32 NumVertices = SelectedPluginMapViewComponent->GetRawMeshVertices().Num();
		const FString NumVerticesToString = TEXT("Vertex Count : ") + FString::FromInt(NumVertices);

		const int32 NumTriangles = SelectedPluginMapViewComponent->GetRawMeshIndices().Num() / 3;
		const FString NumTrianglesToString = TEXT("Triangle Count : ") + FString::FromInt(NumTriangles);

		const bool bCollisionEnabled = SelectedPluginMapViewComponent->IsCollisionEnabled();
		const FString CollisionStatusToString = bCollisionEnabled ? TEXT("Collision : ON") : TEXT("Collision : OFF");

		PluginMapViewCategory.AddCustomRow(FText::GetEmpty(), true)
			[
				SAssignNew(TempHorizontalBox, SHorizontalBox)
				+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Font(FSlateFontInfo("Verdana", 8))
			.Text(FText::FromString(NumVerticesToString))
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Font(FSlateFontInfo("Verdana", 8))
			.Text(FText::FromString(NumTrianglesToString))
			]
			+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Font(FSlateFontInfo("Verdana", 8))
				.Text(FText::FromString(CollisionStatusToString))
				]
			];
	}

}

bool FPluginMapViewComponentDetails::HasValidMeshData() const
{
	return SelectedPluginMapViewComponent != nullptr && SelectedPluginMapViewComponent->HasValidMesh();
}


bool FPluginMapViewComponentDetails::HasValidMapObject() const
{
	return SelectedPluginMapViewComponent != nullptr && SelectedPluginMapViewComponent->GetPluginMapView() != nullptr;
}

FReply FPluginMapViewComponentDetails::OnCreateStaticMeshAssetClicked()
{
	if (SelectedPluginMapViewComponent != nullptr)
	{
		FString NewNameSuggestion = SelectedPluginMapViewComponent->GetPluginMapViewAssetName();
		FString PackageName = FString(TEXT("/Game/Meshes/")) + NewNameSuggestion;
		FString Name;
		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		AssetToolsModule.Get().CreateUniqueAssetName(PackageName, TEXT(""), PackageName, Name);

		TSharedPtr<SDlgPickAssetPath> PickAssetPathWidget =
			SNew(SDlgPickAssetPath)
			.Title(LOCTEXT("ConvertToStaticMeshPickName", "Choose New StaticMesh Location"))
			.DefaultAssetPath(FText::FromString(PackageName));

		if (PickAssetPathWidget->ShowModal() == EAppReturnType::Ok)
		{
			// Get the full name of where we want to create the physics asset.
			FString UserPackageName = PickAssetPathWidget->GetFullAssetPath().ToString();
			FName MeshName(*FPackageName::GetLongPackageAssetName(UserPackageName));

			// Check if the user inputed a valid asset name, if they did not, give it the generated default name
			if (MeshName == NAME_None)
			{
				// Use the defaults that were already generated.
				UserPackageName = PackageName;
				MeshName = *Name;
			}

			// Raw mesh data we are filling in
			FRawMesh RawMesh;
			// Materials to apply to new mesh
			TArray<UMaterialInterface*> MeshMaterials = SelectedPluginMapViewComponent->GetMaterials();

			
			const TArray<FPluginMapViewVertex > RawMeshVertices = SelectedPluginMapViewComponent->GetRawMeshVertices();
			const TArray< uint32 > RawMeshIndices = SelectedPluginMapViewComponent->GetRawMeshIndices();


			// Copy verts
			for (int32 VertIndex = 0; VertIndex < RawMeshVertices.Num();VertIndex++)
			{
				RawMesh.VertexPositions.Add(RawMeshVertices[VertIndex].Position);
			}

			// Copy 'wedge' info
			int32 NumIndices = RawMeshIndices.Num();
			for (int32 IndexIdx = 0; IndexIdx < NumIndices; IndexIdx++)
			{
				int32 VertexIndex = RawMeshIndices[IndexIdx];

				RawMesh.WedgeIndices.Add(VertexIndex);

				const FPluginMapViewVertex& PluginMapViewVertex = RawMeshVertices[VertexIndex];

				FVector TangentX = PluginMapViewVertex.TangentX;
				FVector TangentZ = PluginMapViewVertex.TangentZ;
				FVector TangentY = (TangentX ^ TangentZ).GetSafeNormal();

				RawMesh.WedgeTangentX.Add(TangentX);
				RawMesh.WedgeTangentY.Add(TangentY);
				RawMesh.WedgeTangentZ.Add(TangentZ);

				RawMesh.WedgeTexCoords[0].Add(PluginMapViewVertex.TextureCoordinate);
				RawMesh.WedgeColors.Add(PluginMapViewVertex.Color);
			}

			// copy face info
			int32 NumTris = NumIndices / 3;
			for (int32 TriIdx = 0; TriIdx < NumTris; TriIdx++)
			{
				RawMesh.FaceMaterialIndices.Add(0);
				RawMesh.FaceSmoothingMasks.Add(0); // Assume this is ignored as bRecomputeNormals is false
			}

			// If we got some valid data.
			if (RawMesh.VertexPositions.Num() > 3 && RawMesh.WedgeIndices.Num() > 3)
			{
				// Then find/create it.
				UPackage* Package = CreatePackage(NULL, *UserPackageName);
				check(Package);

				// Create StaticMesh object
				UStaticMesh* StaticMesh = NewObject<UStaticMesh>(Package, MeshName, RF_Public | RF_Standalone);
				StaticMesh->InitResources();

				StaticMesh->LightingGuid = FGuid::NewGuid();

				// Add source to new StaticMesh
				FStaticMeshSourceModel* SrcModel = new (StaticMesh->SourceModels) FStaticMeshSourceModel();
				SrcModel->BuildSettings.bRecomputeNormals = false;
				SrcModel->BuildSettings.bRecomputeTangents = false;
				SrcModel->BuildSettings.bRemoveDegenerates = false;
				SrcModel->BuildSettings.bUseHighPrecisionTangentBasis = false;
				SrcModel->BuildSettings.bUseFullPrecisionUVs = false;
				SrcModel->BuildSettings.bGenerateLightmapUVs = true;
				SrcModel->BuildSettings.SrcLightmapIndex = 0;
				SrcModel->BuildSettings.DstLightmapIndex = 1;
				SrcModel->RawMeshBulkData->SaveRawMesh(RawMesh);

				// Copy materials to new mesh
				for (UMaterialInterface* Material : MeshMaterials)
				{
					StaticMesh->StaticMaterials.Add(FStaticMaterial(Material));
				}

				//Set the Imported version before calling the build
				StaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;

				// Build mesh from source
				StaticMesh->Build(/** bSilent =*/ false);
				StaticMesh->PostEditChange();

				StaticMesh->MarkPackageDirty();

				// Notify asset registry of new asset
				FAssetRegistryModule::AssetCreated(StaticMesh);


				// Display notification so users can quickly access the mesh
				if (GIsEditor)
				{
					FNotificationInfo Info(FText::Format(LOCTEXT("PluginMapViewMeshConverted", "Successfully Converted Mesh"), FText::FromString(StaticMesh->GetName())));
					Info.ExpireDuration = 8.0f;
					Info.bUseLargeFont = false;
					Info.Hyperlink = FSimpleDelegate::CreateLambda([=]() { FAssetEditorManager::Get().OpenEditorForAssets(TArray<UObject*>({ StaticMesh })); });
					Info.HyperlinkText = FText::Format(LOCTEXT("OpenNewAnimationHyperlink", "Open {0}"), FText::FromString(StaticMesh->GetName()));
					TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
					if (Notification.IsValid())
					{
						Notification->SetCompletionState(SNotificationItem::CS_Success);
					}
				}
			}
		}
	}

	return FReply::Handled();
}

FReply FPluginMapViewComponentDetails::OnBuildMeshClicked()
{

	if(SelectedPluginMapViewComponent != nullptr)
	{
		//
		SelectedPluginMapViewComponent->BuildMesh();

		// regenerates details panel layouts , to take in consideration new changes.
		RefreshDetails();
	}

	return FReply::Handled();
}

FReply FPluginMapViewComponentDetails::OnClearMeshClicked()
{
	if (SelectedPluginMapViewComponent != nullptr)
	{
		//
		SelectedPluginMapViewComponent->InvalidateMesh();

		// regenerates details panel layouts , to take in consideration new changes.
		RefreshDetails();
	}

	return FReply::Handled();
}

void FPluginMapViewComponentDetails::RefreshDetails()
{
	if(LastDetailBuilderPtr != nullptr)
	{
		LastDetailBuilderPtr->ForceRefreshDetails();
	}
}

#undef LOCTEXT_NAMESPACE
