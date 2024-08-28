/*=============================================================================
	ANavigationPoint.h
=============================================================================*/

	ANavigationPoint(){}
	void Destroy();
	void PostScriptDestroyed();
	void PostEditChange();
	void PostEditMove();
	void RenderSelectInfo( struct FSceneNode* Frame );
	void Spawned();
	void PostLoad();
	void NotifyPathsDefine( UBOOL bPreNotify );
	virtual void UpdateReachability( class APawn* Scout );
	virtual BYTE EdTestReach( class APawn* Scout, class ANavigationPoint* End, INT* ForceDist=NULL, bool bDoCollisionTest=false );
	void clearPath();
	virtual UBOOL IsInventorySpot() const;
	virtual class AInventory* GetMarkedItem() const;
	UBOOL ShouldExportProperty(UProperty* Property) const;
	virtual UBOOL CheckPath(APawn* User); // Check if path can be taken, also update cost here.

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
