/*=============================================================================
	U3DCollision.h: Unreal 3D Collision code,
	based on seperating axis theorem.

	Revision history:
		* Created by .:..:
	Box collision check maths based on seperating axis theorem.
	Fast access maths...
=============================================================================*/
// Unreal 3D collision header file.
// Box collision check maths based on separating axis theorem.

// Fast access maths...
inline bool PolyIsValid( const FVector& V1, const FVector& V2, const FVector& V3 )
{
	return !(FPointsAreSame(V1,V2) || FPointsAreSame(V2,V3) || FPointsAreSame(V1,V3));
}
inline FPlane FastPlane( const FVector& V1, const FVector& V2, const FVector& V3 )
{
	return FPlane(V1, ((V2 - V1) ^ (V3 - V1)).SafeNormalSlow());
}
struct FCollisionNode;

// Triangle...
struct FCollisionTriangle
{
	friend struct FCollisionModel;
	friend struct FCollisionNode;

	INT Verts[3];
	FPlane LocalPlane,LocalClips[3]; // Store the normal and clipping planes.
	FVector CenterPoint;
	INT TextureIndex,PolygonIndex;

	FCollisionTriangle( INT A, INT B, INT C, const FVector* VPtr, INT TexIndex, INT PolyIndex )
		: LocalPlane(FastPlane(VPtr[A],VPtr[B],VPtr[C]))
		, TextureIndex(TexIndex)
		, PolygonIndex(PolyIndex)
	{
		Verts[0] = A;
		Verts[1] = B;
		Verts[2] = C;
		for( BYTE i=0; i<3; i++ )
			LocalClips[i] = FastPlane(VPtr[Verts[i]],VPtr[Verts[(i+1)%3]],VPtr[Verts[i]]+LocalPlane*5.f);
		CenterPoint = (VPtr[A]+VPtr[B]+VPtr[C])/3.f;
	}

private:
	UBOOL LineCheckPoly(const struct FLineCheckData* Data, FCheckResult& Result);
	UBOOL FastLineCheckPoly(const struct FLineCheckData* Data);
	UBOOL LineCheckPolyShort(const struct FLineCheckData* Data, FCheckResult& Result);
	UBOOL FastLineCheckPolyShort(const struct FLineCheckData* Data);
};

// Main collision data
struct ENGINE_API FCollisionModel
{
	friend struct FCollisionNode;

private:
	TArray<FCollisionNode> Nodes;
public:
	TArray<FVector> Verts;
	TArray<FCollisionTriangle> Tris;
	TArray<FCollisionTriangle> LightTris;

	FCollisionModel()
	{}

	// Basic straight forward collision checks.
	// Returns True if DID hit.
	// LocalCoords = SomeActor->ToLocal()
	// Hit Result fills in: Normal/Location/Time
	UBOOL LineCheck( FCheckResult& Result, const FVector& Start, const FVector& End, const FCoords& LocalCoords);
	UBOOL FastLineCheck( FVector Start, FVector End, const FCoords& LocalCoords );
	UBOOL BoxLineCheck( FCheckResult& Result, const FVector& Start, const FVector& End, const FVector& Extent, const FCoords& LocalCoords);
	UBOOL BoxPointCheck( FCheckResult& Result, FVector Point, const FVector& Extent, const FCoords& LocalCoords);
	UBOOL LightRayLineCheck( const FVector& Start, const FVector& End, const FCoords& LocalCoords, AActor* Owner, UPrimitive* Prim );

	// When collision shape is already in world coordinates (terrain) and only should block one-sided.
	UBOOL LineCheckShort(FCheckResult& Result, const FVector& Start, const FVector& End);
	UBOOL FastLineCheckShort(const FVector& Start, const FVector& End);
	UBOOL BoxLineCheckShort(FCheckResult& Result, const FVector& Start, const FVector& End, const FVector& Extent);
	UBOOL BoxPointCheckShort(FCheckResult& Result, FVector Point, const FVector& Extent);

	// Buildup collision tree by splitting polys into groups.
	void BuildCollisionTree();
	void DebugDrawTree( const FCoords& WorldCoords ); // Draw debug lines of collision tree bounding boxes.

private:
	void InitNode( INT Idx, INT StartOffset, INT Count );
};

// Line check data cache.
struct FLineCheckData
{
	const FVector& Start;
	const FVector& End;
	const FVector& Dir;
	const FVector InDir;
	FCollisionModel& Model;

	FLineCheckData(const FVector& A, const FVector& B, const FVector& C, FCollisionModel* M)
		: Start(A)
		, End(B)
		, Dir(C)
		, InDir((C.X ? 1.f / C.X : 0.f),(C.Y ? 1.f / C.Y : 0.f), (C.Z ? 1.f / C.Z : 0.f))
		, Model(*M)
	{}
};
struct FBoxLineCheckData : public FLineCheckData
{
	const FVector& Extent;

	FBoxLineCheckData( const FVector& A, const FVector& B, const FVector& C, const FVector& D, FCollisionModel* M )
		: FLineCheckData(A,B,C,M)
		, Extent(D)
	{}
	inline FBox ExpandBox( const FBox& B )
	{
		return FBox(B.Min-Extent,B.Max+Extent);
	}
};

// Point check data cache.
struct FBoxPointCheckData
{
	const FVector& Location,Extent;
	FCollisionModel& Model;

	FBoxPointCheckData(const FVector& A, const FVector& B, FCollisionModel* M )
		: Location(A)
		, Extent(B)
		, Model(*M)
	{}
	inline bool BoxOverleaps( const FBox& B )
	{
		return Location.X >= (B.Min.X-Extent.X) && Location.X <= (B.Max.X+Extent.X)
			&& Location.Y >= (B.Min.Y-Extent.Y) && Location.Y <= (B.Max.Y+Extent.Y)
			&& Location.Z >= (B.Min.Z-Extent.Z) && Location.Z <= (B.Max.Z+Extent.Z);
	}
};

struct FCollisionNode
{
	FBox Bounds; // Bounds
	INT Start,End; // Polylist.
	INT A,B; // Two nodes split from this one.

	UBOOL LineCheck( FCheckResult& Result, FLineCheckData& Data );
	UBOOL FastLineCheck( FLineCheckData& Data );
	UBOOL LineCheckShort(FCheckResult& Result, FLineCheckData& Data);
	UBOOL FastLineCheckShort(FLineCheckData& Data);
	UBOOL BoxLineCheck( FCheckResult& Result, FBoxLineCheckData& Data, struct FSABoxLineCheck& BoxCheck );
	UBOOL PointCheck( FBoxPointCheckData& Data, struct FSABoxPointCheck& BoxCheck );
	void DrawBounds( const FCoords& Coords );
};

/* Simple 3D trace function */
struct UMeshSurfaceType
{
private:
	FVector PointA, PointB, PointC, Normal;
	FLOAT HitTime;
	UBOOL bIsTwoSided, bHitBackside;

	inline bool isOnSameSide(const FVector& p1, const FVector& p2, const FVector& a, const FVector& b) const
	{
		FVector bminusa = b - a;
		FVector cp1 = bminusa ^ (p1 - a);
		FVector cp2 = bminusa ^ (p2 - a);
		return ((cp1 | cp2) >= 0.0f);
	}
	inline bool isPointInside(const FVector& Point) const
	{
		return (isOnSameSide(Point, PointA, PointB, PointC) &&
			isOnSameSide(Point, PointB, PointA, PointC) &&
			isOnSameSide(Point, PointC, PointA, PointB));
	}
	inline bool getIntersection(const FVector& lineS, const FVector& Dir, FVector& outInter)
	{
		FLOAT t2 = Normal | Dir;
		if (t2 <= SMALL_NUMBER)
			return false;

		FLOAT d = PointA | Normal;
		HitTime = -((Normal | lineS) - d) / t2;
		if (HitTime < 0.f)
			return false;
		outInter = lineS + (Dir * HitTime);
		bHitBackside = FALSE;
		return true;
	}
	inline bool getIntersectionInvert(const FVector& lineS, const FVector& Dir, FVector& outInter)
	{
		FLOAT t2 = Normal | Dir;
		if (t2 >= -SMALL_NUMBER)
			return false;

		FLOAT d = PointA | Normal;
		HitTime = -((Normal | lineS) - d) / t2;
		if (HitTime < 0.f)
			return false;
		outInter = lineS + (Dir * HitTime);
		bHitBackside = TRUE;
		return true;
	}

public:
	inline void SetSurf(const FVector& A, const FVector& B, const FVector& C, UBOOL bTwoSided = FALSE)
	{
		PointA = A;
		PointB = B;
		PointC = C;
		Normal = -((B - A) ^ (C - A)).SafeNormalSlow();
		bIsTwoSided = bTwoSided;
	}
	inline bool getIntersectionWithLine(const FVector& lineStart, const FVector& Dir, FVector& outInter)
	{
		if (getIntersection(lineStart, Dir, outInter) || (bIsTwoSided && getIntersectionInvert(lineStart, Dir, outInter)))
			return isPointInside(outInter);
		return false;
	}
	inline FVector GetNormal() const
	{
		return bHitBackside ? Normal : -Normal;
	}
	inline FLOAT GetTime() const
	{
		return HitTime;
	}
};
