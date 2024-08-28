/*=============================================================================
	AAntiPortalActor.h.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

	// Constructors.
	AAntiPortalActor() {}

	// AActor
	void NotifyActorMoved();
	INT* NativeReplicationRep(NETCOND_PARAMS);
	UBOOL IsDynamicBSP() const;
	UBOOL IsDynamicBSPHidden() const;
	void PreNetReceive();
	void PostNetReceive();
	void PrepareDynamicBSP(FBspSurf* Surfs);

	// UObject
	void Serialize(FArchive& Ar);

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
