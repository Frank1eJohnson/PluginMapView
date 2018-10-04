// Copyright 2017 Zhongqi Shan. All Rights Reserved.

#pragma once

class  FPluginMapViewComponentDetails : public IDetailCustomization
{
public:

	FPluginMapViewComponentDetails();

	/** Makes a new instance of this detail layout class for a specific detail view requesting it. */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	/** Returns true if we selected a valid cached PluginMapViewComp , having valid cached mesh data. */
	bool HasValidMeshData() const;

	/** Returns true if we selected a valid cached PluginMapViewComp having valid street map representation. */
	bool HasValidMapObject() const;

	/** Handles create static mesh asset button clicking */
	FReply OnCreateStaticMeshAssetClicked();

	/** Handles build/rebuild mesh button clicking */
	FReply OnBuildMeshClicked();

	/** Handles clear mesh button clicking */
	FReply OnClearMeshClicked();

	/** Refreshes the details view and regenerates all the customized layouts. */
	void RefreshDetails();


protected:
	/** Holds Selected Street Map Component */
	 class UPluginMapViewComponent* SelectedPluginMapViewComponent;

	 /** Holds Last Detail Builder Pointer */
	 IDetailLayoutBuilder*  LastDetailBuilderPtr;

};
