/*=============================================================================
	UnPrim.h: Unreal UPrimitive definition.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

/*-----------------------------------------------------------------------------
	FCheckResult.
-----------------------------------------------------------------------------*/

//
// Results of an actor check.
//
struct FIteratorActorList : public FIteratorList
{
	// Variables.
	AActor* Actor;

	// Functions.
	FIteratorActorList()
	:	Actor(NULL)
	{}
	FIteratorActorList( FIteratorActorList* InNext, AActor* InActor )
	:	FIteratorList	(InNext)
	,	Actor			(InActor)
	{}
	FIteratorActorList* GetNext()
	{ return (FIteratorActorList*) Next; }
};

//
// Results from a collision check.
//
struct FCheckResult : public FIteratorActorList
{
	// Variables.
	FVector		Location;	// Location of the hit in coordinate system of the returner.
	FVector		Normal;		// Normal vector in coordinate system of the returner. Zero=none.
	UPrimitive*	Primitive;	// Actor primitive which was hit, or NULL=none.
	FLOAT		Time;		// Time until hit, if line check.
	INT			Item;		// Primitive data item which was hit, INDEX_NONE=none.
	UTexture*	Texture;	// Hit texture.
	DWORD		Flags;		// Hit surface flags.
	BYTE		bShoveCollision;

	// Functions.
	FCheckResult()
	:	FIteratorActorList()
	,	Primitive	(NULL)
	,	Time		(1.f)
	,	Texture		(NULL)
	,	bShoveCollision(FALSE)
	{}
	FCheckResult( FLOAT InTime, FCheckResult* InNext=NULL )
	:	FIteratorActorList( InNext, NULL )
	,	Location	(0,0,0)
	,	Normal		(0,0,0)
	,	Primitive	(NULL)
	,	Time		(InTime)
	,	Item		(INDEX_NONE)
	,	Texture		(NULL)
	,	Flags		(0)
	,	bShoveCollision(FALSE)
	{}
	FCheckResult*& GetNext()
		{ return *(FCheckResult**)&Next; }
	static QSORT_RETURN CDECL CompareHits( const FCheckResult* A, const FCheckResult* B )
		{ return A->Time<B->Time ? -1 : A->Time>B->Time ? 1 : 0; } // friend change because of gcc - Smirftsch
};

/*-----------------------------------------------------------------------------
	UPrimitive.
-----------------------------------------------------------------------------*/

//
// UPrimitive, the base class of geometric entities capable of being
// rendered and collided with.
//
class ENGINE_API UPrimitive : public UObject
{
	DECLARE_CLASS(UPrimitive,UObject,0,Engine)

	// Variables.
	FBox BoundingBox;
	FSphere BoundingSphere;

	// Constructor.
	UPrimitive()
	: BoundingBox(0)
	, BoundingSphere(0)
	{}

	// UObject interface.
	void Serialize( FArchive& Ar );
	void Destroy();

	UBOOL AllowScriptDestroy()
	{
		return FALSE;
	}

	// UPrimitive collision interface.
	virtual UBOOL PointCheck
	(
		FCheckResult	&Result,
		AActor			*Owner,
		FVector			Location,
		FVector			Extent,
		DWORD           ExtraNodeFlags
	);
	virtual UBOOL LineCheck
	(
		FCheckResult	&Result,
		AActor			*Owner,
		FVector			End,
		FVector			Start,
		FVector			Extent,
		DWORD           ExtraNodeFlags
	);
	virtual FBox GetRenderBoundingBox( const AActor* Owner, UBOOL Exact );
	virtual FSphere GetRenderBoundingSphere( const AActor* Owner, UBOOL Exact );
	virtual FBox GetCollisionBoundingBox( const AActor* Owner );
	virtual BYTE FastLineCheck( const FVector& End, const FVector& Start, AActor* Owner=NULL );
	virtual BYTE FastLineCheckLight(const FVector& End, const FVector& Start, AActor* Owner = NULL)
	{
		return FastLineCheck(End, Start, Owner);
	}
	virtual BYTE CheckLightrayBlock( const FVector& Pos, const FVector** Clips, const FVector** Pts, INT iSurf, BYTE bBacksideHit, AActor* Owner )
	{
		return 1;
	}

	// Physics engine support.
	virtual PX_MeshShape* GetConvexVolume()
	{
		return NULL;
	}
	virtual PX_MeshShape* GetTriangleMesh()
	{
		return NULL;
	}
	virtual void DeletePhysicsMesh()
	{}
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
