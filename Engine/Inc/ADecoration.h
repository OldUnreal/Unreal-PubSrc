/*=============================================================================
	ADecoration.h: Class functions residing in the ADecoration class.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

	ADecoration(){}
	void processLanded(FVector HitNormal, AActor *HitActor, FLOAT remainingTime, INT Iterations);
	void PostScriptDestroyed();
	void BeginPlay();
	UBOOL ShouldBaseOnStart() const;

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
