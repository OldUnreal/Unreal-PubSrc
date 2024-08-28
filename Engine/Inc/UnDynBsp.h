/*=============================================================================
	UnDynBsp.h: Unreal dynamic Bsp object support
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

/*---------------------------------------------------------------------------------------
	FMovingBrushTrackerBase virtual base class.
---------------------------------------------------------------------------------------*/

//
// Moving brush tracker.
//
class FMovingBrushTrackerBase
{
public:
	// Constructors/destructors.
	virtual ~FMovingBrushTrackerBase() noexcept(false) {};

	// Public operations:
	virtual UBOOL SurfIsDynamic( INT iSurf ) _VF_BASE_RET(0);

	virtual void Update( AActor* Actor ) _VF_BASE;
	virtual void Flush( AActor* Actor ) _VF_BASE;
	virtual void CountBytes( FArchive& Ar ) _VF_BASE;
};
ENGINE_API FMovingBrushTrackerBase* GNewBrushTracker( ULevel* Level );

/*---------------------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------------------*/
