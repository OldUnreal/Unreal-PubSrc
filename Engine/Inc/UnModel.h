/*=============================================================================
	UnModel.h: Unreal UModel definition.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/
#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

/*-----------------------------------------------------------------------------
	UModel.
-----------------------------------------------------------------------------*/

// U3DCollision
struct F3DCollisionType
{
	FCollisionModel* Collision;

	F3DCollisionType()
	: Collision( NULL )
	{}
	~F3DCollisionType()
	{
		if( Collision )
		{
			delete Collision;
			Collision = NULL;
		}
	}
};

//
// Model objects are used for brushes and for the level itself.
//
enum { MAX_NODES = 262144 };
enum { MAX_POINTS = 262144 };
class ENGINE_API UModel : public UPrimitive
{
	DECLARE_CLASS(UModel,UPrimitive,CLASS_RuntimeStatic,Engine)

	// Other variables.
	UBOOL					RootOutside;
	UBOOL					Linked;
	INT						MoverLink;
	INT						NumSharedSides;
	INT						NumZones;
	FZoneProperties			Zones[FBspNode::MAX_ZONES];

	// Arrays and subobjects.
	UPolys*					Polys;
    TTransArray<FBspSurf>	Surfs;
    TTransArray<FVector>	Vectors;
    TTransArray<FVector>	Points;
	TTransArray<FVert>      Verts;
	TTransArray<FBspNode>	Nodes;
	TArray<FLightMapIndex>	LightMap;
	TArray<BYTE>			LightBits;
	TArray<FBox>			Bounds;
	TArray<INT>				LeafHulls;
	TArray<FLeaf>			Leaves;
	TArray<AActor*>			Lights;

	// 227k model data:
	TArrayNoInit<DWORD> ExtraFlags;

	// In-game internal
	PX_MeshShape* RbPhysicsShape;
	F3DCollisionType CollisionHandle;
	UBOOL bModelMirrored;

	// Constructors.
	UModel()
	: RootOutside( 1 )
	, Surfs( this )
	, Vectors( this )
	, Points( this )
	, Verts( this )
	, Nodes( this )
	, RbPhysicsShape(NULL)
	, bModelMirrored(FALSE)
	{
		EmptyModel( 1, 0 );
	}
	UModel( ABrush* Owner, UBOOL InRootOutside=1 );

	// UObject interface.
	void Serialize( FArchive& Ar );
	void PostLoad();
	void StaticConstructor();

	// UPrimitive interface.
	UBOOL PointCheck
	(
		FCheckResult	&Result,
		AActor			*Owner,
		FVector			Location,
		FVector			Extent,
		DWORD           ExtraNodeFlags
	);
	UBOOL LineCheck
	(
		FCheckResult	&Result,
		AActor			*Owner,
		FVector			End,
		FVector			Start,
		FVector			Extent,
		DWORD           ExtraNodeFlags
	);
	FBox GetCollisionBoundingBox( const AActor *Owner );
	FBox GetRenderBoundingBox( const AActor* Owner, UBOOL Exact );

	// UModel interface.
	void Modify( UBOOL DoTransArrays=0 );
	void BuildBound();
	void Transform( ABrush* Owner );
	void EmptyModel( INT EmptySurfInfo, INT EmptyPolys );
	void ShrinkModel();
	UBOOL PotentiallyVisible( INT iLeaf1, INT iLeaf2 );
	BYTE FastLineCheck( const FVector &End, const FVector &Start,  AActor* Owner = NULL);
	BYTE FastLineCheckLight(const FVector& End, const FVector& Start, AActor* Owner = NULL);
	BYTE FastLineCheckRep( FVector End, FVector Start );
	UBOOL PointCheckFast(const FVector& Point, AActor* Owner = NULL) const; // Returns true if point inside BSP.
	void EmptyDecals();
	void FlushRender();
	PX_MeshShape* GetConvexVolume();
	PX_MeshShape* GetTriangleMesh();
	PX_MeshShape* GetBrushVolume(class ABrush* Brush) const; // Note: Pointer must be deleted after use.
	void DeletePhysicsMesh();
	void CleanupDestroyed();
	void MirrorModel();

	// UModel transactions.
	void ModifySelectedSurfs( UBOOL UpdateMaster );
	void ModifyAllSurfs( UBOOL UpdateMaster );
	void ModifySurf( INT Index, UBOOL UpdateMaster );

	// UModel collision functions.
	typedef void (*PLANE_FILTER_CALLBACK )(UModel *Model, INT iNode, int Param);
	typedef void (*SPHERE_FILTER_CALLBACK)(UModel *Model, INT iNode, int IsBack, int Outside, int Param);
	void GetPointRegion(class AZoneInfo* Zone, const FVector& Location, FPointRegion& Region) const;
	void InitSecCollision();
	BYTE FastLineCheckSky( const FVector& Point, const FVector& Dir, FVector* HitPos=NULL ); // Check if a point and dir have direct sight to sky.
	FLOAT FindNearestVertex
	(
		const FVector	&SourcePoint,
		FVector			&DestPoint,
		FLOAT			MinRadius,
		INT				&pVertex
	) const;
	void PrecomputeSphereFilter
	(
		const FPlane	&Sphere
	);
	FLightMapIndex* GetLightMapIndex( INT iSurf )
	{
		guard(UModel::GetLightMapIndex);
		if( iSurf == INDEX_NONE ) return NULL;
		FBspSurf& Surf = Surfs(iSurf);
		if( Surf.iLightMap==INDEX_NONE || !LightMap.Num() ) return NULL;
		return &LightMap(Surf.iLightMap);
		unguard;
	}
	void DrawCollisionErrors() const;
};

class ENGINE_API UBspNodes : public UObject
{
	DECLARE_CLASS(UBspNodes,UObject,CLASS_RuntimeStatic,Engine)
	TTransArray<FBspNode> Element;
	INT _NumZones;
	FZoneProperties	_Zones[FBspNode::MAX_ZONES];
	UBspNodes()
	: Element( this )
	{
		guard(UBspNodes::UBspNodes);
		_NumZones = 0;
		for( INT i=0; i<FBspNode::MAX_ZONES; i++ )
		{
			_Zones[i].ZoneActor    = NULL;
			_Zones[i].Connectivity = ((QWORD)1)<<i;
			_Zones[i].Visibility   = ~(QWORD)0;
		}
		unguard;
	}
	void Serialize( FArchive& Ar )
	{
		guard(UBspNodes::Serialize);
		Super::Serialize(Ar);
		if( Ar.IsLoading() )
		{
			INT DbNum=Element.Num(), DbMax=DbNum;
			Ar << DbNum << DbMax;
			Element.Empty( DbNum );
			Element.AddZeroed( DbNum );
			INT i;
			for( i=0; i<Element.Num(); i++ )
				Ar << Element(i);
			Ar << AR_INDEX(_NumZones);
			for( i=0; i<_NumZones; i++ )
				Ar << _Zones[i];
		}
		unguardobj;
	}
};

class ENGINE_API UBspSurfs : public UObject
{
	DECLARE_CLASS(UBspSurfs,UObject,CLASS_RuntimeStatic,Engine)
	UBspSurfs(){}
	TArray<FBspSurf> Element;
	void Serialize( FArchive& Ar )
	{
		guard(FBspSurfs::Serialize);
		Super::Serialize( Ar );
		if( Ar.IsLoading() )
		{
			INT DbNum=0, DbMax=0;
			Ar << DbNum << DbMax;
			Element.Empty( DbNum );
			Element.AddZeroed( DbNum );
			for( INT i=0; i<Element.Num(); i++ )
				Ar << Element(i);
		}
		unguard;
	}
};

class ENGINE_API UVectors : public UObject
{
	DECLARE_CLASS(UVectors,UObject,CLASS_RuntimeStatic,Engine)
    UVectors(){}
	TArray<FVector> Element;
	void Serialize( FArchive& Ar )
	{
		guard(UVectors::Serialize);
		Super::Serialize( Ar );
		if( Ar.IsLoading() )
		{
			INT DbNum=Element.Num(), DbMax=DbNum;
			Ar << DbNum << DbMax;
			Element.Empty( DbNum );
			Element.AddZeroed( DbNum );
			for( INT i=0; i<Element.Num(); i++ )
				Ar << Element(i);
		}
		unguard;
	}
};

class ENGINE_API UVerts : public UObject
{
	DECLARE_CLASS(UVerts,UObject,CLASS_RuntimeStatic,Engine)
	UVerts(){}
	TArray<FVert> Element;
	INT NumSharedSides;
	void Serialize( FArchive& Ar )
	{
		guard(UVerts::Serialize);
		Super::Serialize( Ar );
		if( Ar.IsLoading() )
		{
			Element.CountBytes( Ar );
			INT DbNum=Element.Num(), DbMax=DbNum;
			Ar << DbNum << DbMax;
			Element.Empty( DbNum );
			Element.AddZeroed( DbNum );
			for( INT i=0; i<Element.Num(); i++ )
				Ar << Element(i);
		}
		Ar << AR_INDEX(NumSharedSides);
		unguardobj;
	}
};

// A Convex volume which can be used to describe BSP volume.
struct ENGINE_API FConvexFace
{
	FPlane Plane;
	TArray<FPlane> Verts;
	BYTE Tag;
	INT iNode;

	FConvexFace()
		: iNode(INDEX_NONE)
	{}
	FConvexFace(const FVector& InitDir);
	FConvexFace(const FPlane& P, INT N);

	BYTE ExtrudePlane(const FPlane& P);
	inline BYTE IsValid() const
	{
		return (Verts.Num() > 2);
	}

private:
	void CreatePlane();
};
struct ENGINE_API FConvexModel
{
	TArray<FConvexFace> Faces;
	FVector Center;
	FConvexModel* Next;

	FConvexModel();
	FConvexModel(const FBox& Bounds);
	FConvexModel(INT);

	void ExtrudePlane(const FPlane& P, INT iNode = INDEX_NONE);

	// Check if any point of this convex volume is visible to a point.
	BYTE PointVisible(UModel* M, const FVector& P) const;

	// Check if any point of this convex volume is visible to the sky in given direction.
	BYTE SkyVisibile(UModel* M, const FVector& D) const;

	// Compute center point of convex volume.
	void CalcCenterPoint();

	inline FVector GetCenterEpoch(const FVector& P) const
	{
		return (Center - P).NormalApprox();
	}

	// Draw this convex model with FDebugLineData's
	void DrawDebugLines(const FPlane& Color) const;
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
