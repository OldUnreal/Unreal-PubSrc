/*=============================================================================
	AxisCollision.h: Unreal 3D Collision code,
	Seperating axis theorem
	Revision history:
		* Created by .:..:
=============================================================================*/


// Separating axis theorem

#define TEST_AXIS(BoxAxis) !TestNormal((V2 - V1) ^ BoxAxis,V1,V2,V3) || !TestNormal((V3 - V2) ^ BoxAxis,V1,V2,V3) || !TestNormal((V1 - V3) ^ BoxAxis,V1,V2,V3)

// Useage: First init one struct, then call PointTest/LineTest for each triangle and voila...
// Box point check.
struct FSABoxPointCheck
{
public:
	// Hit result data.
	FVector& ResultNormal;
	FLOAT& BestTime;

private:
	const FVector BoxX, BoxY, BoxZ;
	const FVector& BoxExtent, Point;
	FLOAT SqareX,SqareY,SqareZ;

	inline UBOOL TestFace( const FVector& Axis, FLOAT ProjExt, const FVector& V1, const FVector& V2, const FVector& V3 )
	{
		FLOAT PPoint = (Axis | Point);
		FLOAT PV1=(Axis | V1),PV2=(Axis | V2),PV3=(Axis | V3),TriMin=(Min(PV1,Min(PV2,PV3))-ProjExt),TriMax=(Max(PV1,Max(PV2,PV3))+ProjExt);

		if( PPoint>=TriMin && PPoint<=TriMax )
		{
			FLOAT AxisDist=Axis.Size(),MinDist=(PPoint-TriMin),MaxDist=(TriMax-PPoint);
			if( MinDist<(BestTime*AxisDist) )
			{
				FLOAT	InvertedAxisDist = 1.f / AxisDist;
				BestTime = MinDist * InvertedAxisDist;
				ResultNormal = -Axis * InvertedAxisDist;
			}
			if( MaxDist<(BestTime*AxisDist) )
			{
				FLOAT	InvertedAxisDist = 1.f / AxisDist;
				BestTime = MaxDist * InvertedAxisDist;
				ResultNormal = Axis * InvertedAxisDist;
			}
			return TRUE;
		}
		else return FALSE;
	}
	UBOOL TestNormal( const FVector& Dir, const FVector& V1, const FVector& V2, const FVector& V3 )
	{
		FLOAT PExt=(BoxExtent.X*Abs(Dir | BoxX) + BoxExtent.Y*Abs(Dir | BoxY) + BoxExtent.Z*Abs(Dir | BoxZ));
		return TestFace(Dir,PExt,V1,V2,V3);
	}
public:
	// Init data.
	FSABoxPointCheck(const FVector& BX, const FVector& BY, const FVector& BZ, const FVector& BE, const FVector& Pnt, FLOAT& ResTime, FVector& ResNormal)
		: ResultNormal(ResNormal)
		, BestTime(ResTime)
		, BoxX(BX)
		, BoxY(BY)
		, BoxZ(BZ)
		, BoxExtent(BE)
		, Point(Pnt)
		, SqareX(BoxExtent.X* BX.SizeSquared())
		, SqareY(BoxExtent.Y* BY.SizeSquared())
		, SqareZ(BoxExtent.Z* BZ.SizeSquared())
	{
		BestTime = 1.f;
	}

	// Short version.
	FSABoxPointCheck(const FVector& BE, const FVector& Pnt, FLOAT& ResTime, FVector& ResNormal)
		: ResultNormal(ResNormal)
		, BestTime(ResTime)
		, BoxX(1.f, 0.f, 0.f)
		, BoxY(0.f, 1.f, 0.f)
		, BoxZ(0.f, 0.f, 1.f)
		, BoxExtent(BE)
		, Point(Pnt)
		, SqareX(BoxExtent.X)
		, SqareY(BoxExtent.Y)
		, SqareZ(BoxExtent.Z)
	{
		BestTime = 1.f;
	}

	// Point test
	UBOOL PointTest( const FVector& V1, const FVector& V2, const FVector& V3, const FVector& Normal )
	{
		// Test against each face of the box.
		if( !TestFace(BoxX,SqareX,V1,V2,V3) || !TestFace(BoxY,SqareY,V1,V2,V3) || !TestFace(BoxZ,SqareZ,V1,V2,V3) )
			return FALSE;

		// Now with the triangle normal...
		if( !TestNormal(Normal,V1,V2,V3) )
			return FALSE;

		// Test with box edges
		if( TEST_AXIS(BoxX) || TEST_AXIS(BoxY) || TEST_AXIS(BoxZ) )
			return FALSE;

		return TRUE;
	}
};

#undef TEST_AXIS
#define TEST_AXIS(BoxAxis) !TestNormal((V2 - V1),BoxAxis,V1,V2,V3) || !TestNormal((V3 - V2),BoxAxis,V1,V2,V3) || !TestNormal((V1 - V3),BoxAxis,V1,V2,V3)

// Box line check
struct FSABoxLineCheck
{
public:
	// Hit result data.
	FVector& ResultNormal;
	FLOAT& BestTime;
	INT& BestItem;

private:
	const FVector BoxX, BoxY, BoxZ;
	const FVector& BoxExtent,Start,End;
	FVector TempNormal, ParallelNormal;
	FLOAT MinTime,MaxTime,SqareX,SqareY,SqareZ;
	UBOOL bAlmostParallel;

	inline void SetHitTime( FLOAT NewTime, const FVector& Normal )
	{
		if( NewTime>MinTime )
		{
			MinTime = NewTime;
			TempNormal = Normal;
		}
	}
	inline UBOOL TestFace( const FVector& Axis, FLOAT ProjExtent, const FVector& V1, const FVector& V2, const FVector& V3 )
	{
		FLOAT ProjStart=(Axis | Start),ProjEnd=(Axis | End),ProjDir=(ProjEnd-ProjStart),ProjV1=(Axis | V1),ProjV2=(Axis | V2),ProjV3=(Axis | V3);
		FLOAT TriMin=(Min(ProjV1,Min(ProjV2,ProjV3))-ProjExtent),TriMax=(Max(ProjV1,Max(ProjV2,ProjV3))+ProjExtent);

		if( ProjStart<TriMin )
		{
			if (!bAlmostParallel && (ProjStart > (TriMin - SLERP_DELTA)))
			{
				ParallelNormal = -Axis;
				bAlmostParallel = TRUE;
			}
			if( ProjDir<DELTA )
				return FALSE;
			SetHitTime((TriMin-ProjStart) / ProjDir,-Axis);
			MaxTime = Min(MaxTime,(TriMax - ProjStart) / ProjDir);
		}
		else if( ProjStart>TriMax )
		{
			if (!bAlmostParallel && (ProjStart < (TriMax + SLERP_DELTA)))
			{
				ParallelNormal = Axis;
				bAlmostParallel = TRUE;
			}
			if( ProjDir>-DELTA )
				return FALSE;
			SetHitTime((TriMax-ProjStart) / ProjDir,Axis);
			MaxTime = Min(MaxTime,(TriMin - ProjStart) / ProjDir);
		}
		else if( ProjDir>DELTA)
			MaxTime = Min(MaxTime,(TriMax - ProjStart) / ProjDir);
		else if( ProjDir<-DELTA)
			MaxTime = Min(MaxTime,(TriMin - ProjStart) / ProjDir);
		return (MaxTime>=MinTime);
	}
	inline UBOOL TestNormal( const FVector& Line, const FVector& Axis, const FVector& V1, const FVector& V2, const FVector& V3 )
	{
		FVector Normal=(Axis ^ Line);
		if( Normal.SizeSquared()<DELTA )
			return TRUE;
		FLOAT ProjExtent=(BoxExtent.X*Abs(Normal | BoxX) + BoxExtent.Y*Abs(Normal | BoxY) + BoxExtent.Z*Abs(Normal | BoxZ));
		return TestFace(Normal,ProjExtent,V1,V2,V3);
	}
public:
	// Init data.
	FSABoxLineCheck( const FVector& BX, const FVector& BY, const FVector& BZ, const FVector& BE, const FVector& LineStart, const FVector& LineEnd, FCheckResult& HitInfo)
		: ResultNormal(HitInfo.Normal)
        , BestTime(HitInfo.Time)
		, BestItem(HitInfo.Item)
		, BoxX(BX)
		, BoxY(BY)
		, BoxZ(BZ)
		, BoxExtent(BE)
		, Start(LineStart)
		, End(LineEnd)
		, SqareX(BoxExtent.X*BX.SizeSquared())
		, SqareY(BoxExtent.Y*BY.SizeSquared())
		, SqareZ(BoxExtent.Z*BZ.SizeSquared())
		, bAlmostParallel(FALSE)
	{
		BestTime = 1.f;
	}

	// Shorted version.
	FSABoxLineCheck(const FVector& BE, const FVector& LineStart, const FVector& LineEnd, FCheckResult& HitInfo)
		: ResultNormal(HitInfo.Normal)
		, BestTime(HitInfo.Time)
		, BestItem(HitInfo.Item)
		, BoxX(1.f, 0.f, 0.f)
		, BoxY(0.f, 1.f, 0.f)
		, BoxZ(0.f, 0.f, 1.f)
		, BoxExtent(BE)
		, Start(LineStart)
		, End(LineEnd)
		, SqareX(BoxExtent.X)
		, SqareY(BoxExtent.Y)
		, SqareZ(BoxExtent.Z)
		, bAlmostParallel(FALSE)
	{
		BestTime = 1.f;
	}

	// Line test
	UBOOL LineTest( const FVector& V1, const FVector& V2, const FVector& V3, INT ItemIndex )
	{
		MinTime = -1.0f;
		MaxTime = BestTime;

		// Test with box faces
		if( !TestFace(BoxX,SqareX,V1,V2,V3) || !TestFace(BoxY,SqareY,V1,V2,V3) || !TestFace(BoxZ,SqareZ,V1,V2,V3) )
			return FALSE;

		// Test with triangle normal
		if( !TestNormal((V3 - V2),(V2 - V1),V1,V2,V3) )
			return FALSE;

		// Test with Box edges
		if( TEST_AXIS(BoxX) || TEST_AXIS(BoxY) || TEST_AXIS(BoxZ) )
			return FALSE;

		// From UE4:
		// If we are close and parallel, and there is not another axis which provides a good separation.
		if (bAlmostParallel && !(MinTime > BestTime && ParallelNormal != TempNormal && ParallelNormal != -TempNormal))
		{
			// If in danger, disallow movement, in case it puts us inside the object.
			BestTime = 0.f;

			// Tilt the normal slightly back towards the direction of travel. 
			// This will cause the next movement to be away from the surface (hopefully out of the 'danger region')
			const FVector CheckDir = (End - Start).SafeNormal();
			ResultNormal = (ParallelNormal - (0.05f * CheckDir)).SafeNormal();

			// Say we hit.
			return TRUE;
		}

		// See if has valid minimum intersection time.
		if( MinTime>=0.f )
		{
			BestTime = MinTime;
			ResultNormal = TempNormal;
			BestItem = ItemIndex;
			return TRUE;
		}
		return FALSE;
	}
};
#undef TEST_AXIS
