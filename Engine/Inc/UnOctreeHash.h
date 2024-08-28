/*=============================================================================
	UnOctreeHash.h: Octree collision hash
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Marco
=============================================================================*/

#define MAX_ActorsPerNode 3
#define MAX_GeometrySize 32768
#define MAX_NumLeafs 8 // Maximum detail level of leafs
#define MAX_StoreDepth 4

enum EBoundsArea
{
	EBA_ZMax = (1<<0),
	EBA_YMax = (1<<1),
	EBA_XMax = (1<<2),
	EBA_ZMin = (1<<3),
	EBA_YMin = (1<<4),
	EBA_XMin = (1<<5),
};

struct FIVector
{
	INT X,Y,Z;

	FIVector()
	{}
	FIVector( const FVector& V )
		: X(appRound(V.X)),Y(appRound(V.Y)),Z(appRound(V.Z))
	{}
	FIVector( INT InX, INT InY, INT InZ )
		: X(InX),Y(InY),Z(InZ)
	{}
	inline FVector GetVector() const
	{
		return FVector(X,Y,Z);
	}
	inline void SetVector(const FVector& V)
	{
		X = appRound(V.X);
		Y = appRound(V.Y);
		Z = appRound(V.Z);
	}
};
struct FIBox
{
	FIVector Min,Max;

	FIBox()
	{}
	FIBox( const FBox& B )
		: Min(B.Min),Max(B.Max)
	{}
	inline FBox GetBox() const
	{
		return FBox(Min.GetVector(),Max.GetVector());
	}
	inline bool PointInside( const FVector& P ) const
	{
		return (P.X>=Min.X && P.X<=Max.X && P.Y>=Min.Y && P.Y<=Max.Y && P.Z>=Min.Z && P.Z<=Max.Z);
	}
	inline void SetBounds(const FVector& BoxMin, const FVector& BoxMax)
	{
		Min.SetVector(BoxMin);
		Max.SetVector(BoxMax);
	}
	inline void SetPoints(const FVector& A, const FVector& B)
	{
		INT aX = appRound(A.X);
		INT aY = appRound(A.Y);
		INT aZ = appRound(A.Z);
		INT bX = appRound(B.X);
		INT bY = appRound(B.Y);
		INT bZ = appRound(B.Z);

		Min.X = ::Min(aX, bX);
		Min.Y = ::Min(aY, bY);
		Min.Z = ::Min(aZ, bZ);
		Max.X = ::Max(aX, bX);
		Max.Y = ::Max(aY, bY);
		Max.Z = ::Max(aZ, bZ);
	}
	inline void SetPointsExt(const FVector& A, const FVector& B, const FVector& Ext)
	{
		INT aX = appRound(A.X);
		INT aY = appRound(A.Y);
		INT aZ = appRound(A.Z);
		INT bX = appRound(B.X);
		INT bY = appRound(B.Y);
		INT bZ = appRound(B.Z);
		INT eX = appRound(Ext.X);
		INT eZ = appRound(Ext.Z);

		Min.X = ::Min(aX, bX) - eX;
		Min.Y = ::Min(aY, bY) - eX;
		Min.Z = ::Min(aZ, bZ) - eZ;
		Max.X = ::Max(aX, bX) + eX;
		Max.Y = ::Max(aY, bY) + eX;
		Max.Z = ::Max(aZ, bZ) + eZ;
	}
};

struct FOctreeBounds
{
	FIVector Point;
	FVector Extent;
	FIBox Bounds;

	FOctreeBounds( const FBox& B )
	:	Bounds( B )
	{
		FVector Center( (B.Min+B.Max)*0.5f );
		Point = Center;
		Extent = (Center-B.Min);
	}
	inline bool InsideBoxOther( const FIBox& Box ) const
	{
		if( Bounds.Min.X>Box.Max.X || Box.Min.X>Bounds.Max.X ||
			Bounds.Min.Y>Box.Max.Y || Box.Min.Y>Bounds.Max.Y ||
			Bounds.Min.Z>Box.Max.Z || Box.Min.Z>Bounds.Max.Z )
			return false;
		return true;
	}
};

struct FTreeActor
{
	AActor* Actor;
	FOctreeBounds Bounds;
	struct FOctreeNode* Node;

	FTreeActor( AActor* Other )
	:	Actor( Other )
	,	Bounds( Other->GetPrimitive()->GetCollisionBoundingBox(Other).ExpandBy(1.f) )
	,	Node(NULL)
	{}
};

struct FZeroExtLineData
{
	BYTE bInterpolating,bShoveCheck;
	FVector Start,End,Extent,Dir,SqDir;
	FIBox VisBox;
	BYTE ExtraFlags;
	FLOAT Distance;
	FCheckResult* FirstHit;
	AActor* TestActor;
	DWORD TraceFlags;
	FMemStack* Mem;

	inline void SetTraceData()
	{
		Dir = (End-Start);
		SqDir = FVector(1.f/Dir.X,1.f/Dir.Y,1.f/Dir.Z);
	}
};

struct FOctreeNode
{
	TArray<FTreeActor*> ActorList;
	FIVector MidPoint;
	INT Extent;
	FOctreeNode* Leafs; // 8 octree leafs if any.
	FOctreeNode* Parent;
	BYTE LeafDepth;
	bool bHasActors; // Whatever if children down here have any actors in it.

	FOctreeNode( const FVector& P, INT Ex )
	:	ActorList(0)
	,	MidPoint(P)
	,	Extent(Ex)
	,	Leafs(NULL)
	,	Parent(NULL)
	,	LeafDepth(0)
	,   bHasActors(false)
	{}
	FOctreeNode()
	:	ActorList(0)
	,	MidPoint( FVector(0,0,0) )
	,	Extent( 0 )
	,	Leafs(NULL)
	,	Parent(NULL)
	,	LeafDepth(0)
	,   bHasActors(false)
	{}
	inline BYTE GetBoundingLeafItem( const FIBox& Bounds ) const;
	inline BYTE GetMultiBoundingLeafs( const FIBox& Bounds ) const;
	inline bool LeafsHaveActors() const;
	void InitChildren();
	void AddActor(FTreeActor* NewActor);
	bool RemoveActor(FTreeActor* Other);
	void AddDebugLines();
	bool LineCheck();
	bool PointCheck();
	bool RadiusCheck();
	bool FastTrace();
	bool EncroachmentCheck();
	bool LineCheckReverse();
	void ZeroLineCheck( struct FOctreeCollisionHash* tree, FLOAT T0X, FLOAT T0Y, FLOAT T0Z, FLOAT T1X, FLOAT T1Y, FLOAT T1Z );
	~FOctreeNode();
private:
	void AddActorInternal( FTreeActor* NewActor );
	void UpdateHasActors();
};


