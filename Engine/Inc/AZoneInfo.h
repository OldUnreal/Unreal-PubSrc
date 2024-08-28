/*=============================================================================
	AZoneInfo.h.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

	// Constructors.
	AZoneInfo() {}

	// UObject interface.
	void PostEditChange();

	virtual INT* NativeReplicationRep(NETCOND_PARAMS);
	class ASkyZoneInfo* GetSkyBox();
	void GetFog(struct FFogSurf* F);
	FLOAT GetFogDistance(FLOAT ZDist, FSceneNode* Frame);
	
	virtual UBOOL IsWarpZone() const { return FALSE; }
	virtual UBOOL IsZoneRotator() const { return FALSE; }

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
