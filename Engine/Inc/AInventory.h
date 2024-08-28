/*=============================================================================
	AInventory.h
=============================================================================*/

	AInventory() {}
	BYTE ActorCanSee(APlayerPawn* RealViewer, AActor* Viewer, const FVector& SrcLocation);
	FLOAT GetZoneDeltaTime( const FLOAT& InDelta );
	virtual INT* NativeReplicationRep(NETCOND_PARAMS);
	void PostScriptDestroyed();
	void BeginPlay();
	void AddToNavigation();
	void RemoveFromNavigation();
	UBOOL ShouldExportProperty(UProperty* Property) const;
	UBOOL ShouldBaseOnStart() const;

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
