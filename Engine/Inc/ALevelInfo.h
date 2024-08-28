/*=============================================================================
	ALevelInfo.h.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

	ALevelInfo(){}
	virtual INT* NativeReplicationRep(NETCOND_PARAMS);

	static UPlayer* LocalPlayer;
	static ALevelInfo* CurrentLevel;
	void Serialize(FArchive& Ar);
	UBOOL ShouldExportProperty(UProperty* Property) const;

	void PreNetReceive();
	void PostNetReceive();

	void AddPawn(APawn* Pawn);
	void RemovePawn(APawn* Pawn);

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
