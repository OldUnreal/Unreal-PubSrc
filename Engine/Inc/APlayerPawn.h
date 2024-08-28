/*=============================================================================
	APlayerPawn.h: A player pawn.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

	// APlayerPawn interface.
	APlayerPawn(){}
	void SetPlayer( UPlayer* Player );
	virtual INT* NativeReplicationRep(NETCOND_PARAMS);

	// PlayerPawn Render Control Interface (RCI)
	// override the operations in the game-specific PlayerPawn class to control rendering
	virtual bool ClearScreen();
	virtual bool RecomputeLighting();
	virtual bool CanSee( const AActor* Actor );
	virtual INT GetViewZone( INT iViewZone, const UModel* Model );
	virtual bool IsZoneVisible( INT iZone );
	virtual bool IsSurfVisible( const FBspNode* Node, INT iZone, const FBspSurf* Poly );
	virtual bool IsActorVisible( const AActor* Actor );
	void UpdateDistanceFog();
	void GetFog(struct FFogSurf* F);
	AActor* CalcPlayerView(FVector& ViewPos, FRotator& ViewRot);
	AActor* CalcPlayerViewServer(FVector& ViewPos, FRotator& ViewRot);
	void PlayerInput(FLOAT DeltaTime, UBOOL bPlayerTick = TRUE);

	void PreNetReceive();
	void PostNetReceive();
	void PostScriptDestroyed();

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
