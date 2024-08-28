	AStaticMeshActor() {}

	FStaticLightActor* GetStaticLights();
	void Modify();
	void PostEditMove();
	BYTE HasSlowRayTrace();
	void PostRaytrace(FThreadLock& Mutex);
	void DeleteData();
	UBOOL IsValidOnImport();
	void PostScriptDestroyed();
	UBOOL ShouldExportProperty(UProperty* Property) const;
