	ATerrainInfo() {}
	
	// UObject interface.
	void Serialize(FArchive& Ar);
	void PostLoad();
	UBOOL ShouldExportProperty(UProperty* Property) const;
	void PostEditChange();
	void Destroy();

	// AActor interface.
	void Spawned();
	void PostEditMove();
	void ScriptDestroyed();
	FBox GetVisibilityBox();
	FStaticLightActor* GetStaticLights();
	BYTE HasSlowRayTrace();
	void PostRaytrace(FThreadLock& Mutex);

	// Terrain.
	void RebuildTerrain();
	void UpdateBounds();