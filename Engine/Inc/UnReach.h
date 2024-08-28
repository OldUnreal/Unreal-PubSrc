/*=============================================================================
	UnReach.h: AI reach specs.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Steven Polge 3/97
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

// AI latent functions.
enum EAIFunctions
{
	AI_PollMoveTo			= 501,
	AI_PollMoveToward		= 503,
	AI_PollStrafeTo			= 505,
	AI_PollStrafeFacing		= 507,
	AI_PollTurnTo			= 509,
	AI_PollTurnToward		= 511,
	AI_PollWaitForLanding	= 528,
};

inline bool PathIsForced( ANavigationPoint* A, ANavigationPoint* B )
{
	for( BYTE i=0; i<4; i++ )
		if( A->ForcedPaths[i]!=NAME_None && (A->ForcedPaths[i]==B->GetFName() || A->ForcedPaths[i]==B->Tag) )
			return true;
	return false;
}
inline bool PathIsProscribed( ANavigationPoint* A, ANavigationPoint* B )
{
	for( BYTE i=0; i<4; i++ )
		if( A->ProscribedPaths[i]!=NAME_None && (A->ProscribedPaths[i]==B->GetFName() || A->ProscribedPaths[i]==B->Tag) )
			return true;
	return false;
}
inline bool PathHasForced( ANavigationPoint* A )
{
	for( BYTE i=0; i<4; i++ )
		if( A->ForcedPaths[i]!=NAME_None )
			return true;
	return false;
}

// Editor reach preview data.
struct FEdReachNode
{
	AActor *Start,*End;
	BYTE Type;

	FEdReachNode( AActor* A, AActor* B, BYTE R )
		: Start(A)
		, End(B)
		, Type(R)
	{}
	inline FPlane GetColor() const
	{
		switch( Type )
		{
		case 0: // Large
			return FPlane(1,1,1,1);
		case 1: // Medium
			return FPlane(0,1,0,1);
		case 2: // Small
			return FPlane(1,0,0,1);
		case 254: // Special
			return FPlane(1,0,1,1);
		default: // Flying
			return FPlane(0,0,1,1);
		}
	}
};
struct FComputedReachability
{
	TArray<FEdReachNode> ReachList;
};

class ENGINE_API FReachSpec
{

public:
	INT distance;
	AActor *Start;
	AActor *End; //actor at endpoint of this path (next waypoint or goal)
	INT CollisionRadius;
    INT CollisionHeight;
	INT reachFlags; //see defined bits above
	BYTE  bPruned;

	FReachSpec()
		: Start(NULL), End(NULL)
	{}

	/*
	supports() -
	 returns true if it supports the requirements of aPawn.  Distance is not considered.
	*/
	inline int supports (int iRadius, int iHeight, int moveFlags)
	{
		return ( (CollisionRadius >= iRadius)
			&& (CollisionHeight >= iHeight)
			&& ((reachFlags & moveFlags) == reachFlags) );
	}
	FReachSpec operator+ (const FReachSpec &Spec) const;
	int defineFor (AActor * begin, AActor * dest, APawn * Scout);
	int operator<= (const FReachSpec &Spec);
	int operator== (const FReachSpec &Spec);
	int MonsterPath(APawn * Scout);
	int BotOnlyPath(APawn * Scout);

	FPlane PathColor();

	void Init()
	{
		guard(FReachSpec::Init);
		// Init everything here.
		Start = End = NULL;
		distance = CollisionRadius = CollisionHeight = 0;
		reachFlags = 0;
		bPruned = 0;
		unguard;
	};

	friend FArchive& operator<< (FArchive &Ar, FReachSpec &ReachSpec )
	{
		guard(FReachSpec<<);
		if(Ar.SerializeRefs())
			Ar << ReachSpec.Start << ReachSpec.End;
		else
		{
			Ar << ReachSpec.distance << ReachSpec.Start << ReachSpec.End;
			Ar << ReachSpec.CollisionRadius << ReachSpec.CollisionHeight;
			Ar << ReachSpec.reachFlags << ReachSpec.bPruned;
		}
		return Ar;
		unguard;
	};

	int findBestReachable(FVector &Start, FVector &Destination, APawn * Scout);
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif
