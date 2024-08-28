/*=============================================================================
	ABrush.h.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

	// Constructors.
	ABrush();

	// UObject interface.
	void PostLoad();

	// AActor interface.
	FCoords ToLocal() const
	{
		guardSlow(ABrush::ToLocal);
		return (Brush ?
			(GMath.UnitCoords / -PrePivot / MainScale / Rotation / PostScale / Location) :
			AActor::ToLocal());
		unguardSlow;
	}
	FCoords ToWorld() const
	{
		guardSlow(ABrush::ToWorld);
		return (Brush ?
			(GMath.UnitCoords * Location * PostScale * Rotation * MainScale * -PrePivot) :
			AActor::ToWorld());
		unguardSlow;
	}
	FLOAT BuildCoords( FModelCoords* Coords, FModelCoords* Uncoords )
	{
		guard(ABrush::BuildCoords);
		if( Coords )
		{
			Coords->PointXform    = (GMath.UnitCoords * PostScale * Rotation * MainScale);
			Coords->VectorXform   = (GMath.UnitCoords / MainScale / Rotation / PostScale).Transpose();
		}
		if( Uncoords )
		{
			Uncoords->PointXform  = (GMath.UnitCoords / MainScale / Rotation / PostScale);
			Uncoords->VectorXform = (GMath.UnitCoords * PostScale * Rotation * MainScale).Transpose();
		}
		return MainScale.Orientation() * PostScale.Orientation();
		unguard;
	}
	UBOOL IsEncroacher() const
	{
		return TRUE;
	}

	// ABrush interface.
	virtual void CopyPosRotScaleFrom(ABrush* Other);
	virtual void InitPosRotScale();
	void PostEditMove();
	void Modify();
	virtual void TransformPermanently( BYTE KeepRotation );
	virtual void PrepareDynamicBSP(FBspSurf* Surfs);

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
