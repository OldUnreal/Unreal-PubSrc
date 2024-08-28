/*=============================================================================
	UnRenderOctree.h: Octree render
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Marco
=============================================================================*/

// If this value is too low or too high it performs worse, 6 seams to be reasonable...
#define MAX_DrawLeafsDepth 6

#include "UnOctreeHash.h"

struct FRendOctreeNode;

struct FRenderBoundsRejection
{
	const FPlane* Planes;
	FPlane FarPlane;
	BYTE NumPlanes;
	FIVector MidPoint;
	FVector CamPosition;

private:
	FRenderBoundsRejection()
	{}

public:
	FRenderBoundsRejection( const FPlane* Ptr, BYTE Num, const FVector& Point )
		: Planes(Ptr)
		, NumPlanes(Num)
		, MidPoint(Point)
		, CamPosition(Point)
	{}
	inline FLOAT PlaneDot( const FPlane &B, const FIVector &P ) const
	{
		return B.X*P.X + B.Y*P.Y + B.Z*P.Z - B.W;
	}
	inline bool BoundsVis( const FOctreeBounds& B )
	{
		for( BYTE i=0; i<NumPlanes; ++i )
		{
			if( (PlaneDot(Planes[i],B.Point)+FBoxPushOut(Planes[i],B.Extent))<0.f )
				return false;
		}
		if( (PlaneDot(FarPlane,B.Point)+FBoxPushOut(FarPlane,B.Extent))<0.f )
			return false;
		return true;
	}
	inline bool BoundsVisBox( const FBox& B )
	{
		FVector Mid((B.Min+B.Max)*0.5f);
		FVector Extent(Mid-B.Min);
		for( BYTE i=0; i<NumPlanes; ++i )
		{
			if( (PlaneDot(Planes[i],Mid)+FBoxPushOut(Planes[i],Extent))<0.f )
				return false;
		}
		if( (PlaneDot(FarPlane,Mid)+FBoxPushOut(FarPlane,Extent))<0.f )
			return false;
		return true;
	}
	inline bool NodeBoundsVis( FRendOctreeNode& B );
	inline void SetFarPlane( FLOAT Distance, const FCoords& RendCoords )
	{
		FarPlane = FPlane(RendCoords.Origin+RendCoords.ZAxis*Distance,-RendCoords.ZAxis);
	}
};
struct FActorTreeItem
{
	AActor* Actor;
	FOctreeBounds Bounds;
	INT iDraw;
	FActorTreeItem* Next;

	FActorTreeItem( AActor* Other, const FBox& B )
	:	Actor( Other )
	,	Bounds(B)
	,	iDraw(INDEX_NONE)
	{}
};

struct FRendOctreeNode
{
	TArray<FActorTreeItem*> ActorList;
	FIVector MidPoint;
	INT Extent;
	FRendOctreeNode* Leafs; // 8 octree leafs if any.
	BYTE LeafDepth;
	bool bHasActors; // Whatever if children down here have any actors in it.

	FRendOctreeNode( const FVector& P, INT Ex )
	:	ActorList(0)
	,	MidPoint(P)
	,	Extent(Ex)
	,	Leafs(NULL)
	,	LeafDepth(0)
	,   bHasActors(false)
	{}
	FRendOctreeNode()
	:	ActorList(0)
	,	MidPoint( FVector(0,0,0) )
	,	Extent( 0 )
	,	Leafs(NULL)
	,	LeafDepth(0)
	,   bHasActors(false)
	{}
	void InitChildren();
	void AddActor( FActorTreeItem* NewActor );
	~FRendOctreeNode();
	void GetActors( FRenderBoundsRejection& Bounds );
	void AddDebugLines();
private:
	void AddActorInternal( FActorTreeItem* NewActor );
	inline BYTE GetBoundingLeafItem( const FIBox& Bounds, BYTE* Res ) const;
};

struct ENGINE_API FRenderOctree
{
	FRendOctreeNode* MainOctree;
	ULevel* Map;
	TArray<FActorTreeItem*> AllocList;

	// FOctree interface
	FRenderOctree( ULevel* M );
	~FRenderOctree() noexcept(false);

	// Debug draw tree
	void DrawTree();

	// Add actor to tree
	void AddActor(AActor* Actor, UBOOL bInternal = FALSE);

	// Get the actor draw list.
	FActorTreeItem* GetActorDrawList( FRenderBoundsRejection& Bounds, class UPortalModifier* M);
};
