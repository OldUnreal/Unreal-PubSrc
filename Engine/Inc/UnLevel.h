/*=============================================================================
	UnLevel.h: ULevel definition.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

/*-----------------------------------------------------------------------------
	Network notification sink.
-----------------------------------------------------------------------------*/

//
// Accepting connection responses.
//
enum EAcceptConnection
{
	ACCEPTC_Reject,	// Reject the connection.
	ACCEPTC_Accept, // Accept the connection.
	ACCEPTC_Ignore, // Ignore it, sending no reply, while server travelling.
};

//
// The net code uses this to send notifications.
//
class ENGINE_API FNetworkNotify
{
public:
	virtual EAcceptConnection NotifyAcceptingConnection() _VF_BASE_RET(ACCEPTC_Reject);
	virtual UBOOL NotifyAcceptingChannel( struct FChannel* Channel ) _VF_BASE_RET(0);
	virtual UBOOL NotifySendingFile( UNetConnection* Connection, FGuid GUID ) _VF_BASE_RET(0);
	virtual ULevel* NotifyGetLevel() _VF_BASE_RET(NULL);
	virtual class UClientPreloginScene* NotifyGetClientScene() { return NULL; }

	virtual void NotifyReceivedText( UNetConnection* Connection, const TCHAR* Text ) _VF_BASE;
	virtual void NotifyAcceptedConnection( class UNetConnection* Connection ) _VF_BASE;
	virtual void NotifyReceivedFile( UNetConnection* Connection, INT PackageIndex, const TCHAR* Error, UBOOL Skipped ) _VF_BASE;
	virtual void NotifyProgress( const TCHAR* Str1, const TCHAR* Str2, FLOAT Seconds ) _VF_BASE;
};

/*-----------------------------------------------------------------------------
	FCollisionHashBase.
-----------------------------------------------------------------------------*/

class FCollisionHashBase
{
public:
	// FCollisionHashBase interface.
	virtual ~FCollisionHashBase() {};
	virtual void Tick() _VF_BASE;
	virtual void AddActor( AActor *Actor ) _VF_BASE;
	virtual void RemoveActor( AActor *Actor ) _VF_BASE;
	virtual FCheckResult* ActorLineCheck( FMemStack& Mem, const FVector& End, const FVector& Start, const FVector& Extent, BYTE ExtraNodeFlags, DWORD TraceFlags, AActor* SourceActor ) _VF_BASE_RET(NULL);
	virtual FCheckResult* ActorLineCheckReverse( FMemStack& Mem, const FVector& EndPoint, const FVector& Extent, DWORD TraceFlags, AActor* SourceActor ) { return NULL; }
	virtual FCheckResult* ActorPointCheck( FMemStack& Mem, const FVector& Location, const FVector& Extent, BYTE ExtraNodeFlags, DWORD TraceFlags, AActor* SourceActor ) _VF_BASE_RET(NULL);
	virtual FCheckResult* ActorRadiusCheck( FMemStack& Mem, FVector Location, FLOAT Radius, BYTE ExtraNodeFlags, DWORD TraceFlags, AActor* SourceActor ) _VF_BASE_RET(NULL);
	virtual FCheckResult* ActorEncroachmentCheck( FMemStack& Mem, AActor* Actor, FVector Location, FRotator Rotation, BYTE ExtraNodeFlags, DWORD TraceFlags ) _VF_BASE_RET(NULL);
	virtual bool ActorFastTrace( const FVector& End, const FVector& Start, DWORD TraceFlags, AActor* SourceActor ){return true;}
	virtual void CheckActorNotReferenced( AActor* Actor ) _VF_BASE;
};

//ENGINE_API FCollisionHashBase* GNewCollisionHash();
ENGINE_API FCollisionHashBase* GNewCollisionHash( ULevel* Map );

void ENGINE_API GNewEditorCollisionHash( ULevel* Map, BYTE bBuildLights );

// Tickable object.
struct ENGINE_API FTickableObjectBase
{
private:
	FTickableObjectBase* Next;
	static FTickableObjectBase* List;

public:
	FTickableObjectBase();
	virtual ~FTickableObjectBase();
	virtual void Tick(FLOAT DeltaTime) _VF_BASE;

	static void TickTickables(FLOAT DeltaTime);
};

/*-----------------------------------------------------------------------------
	ULevel base.
-----------------------------------------------------------------------------*/

//
// A game level.
//
class ENGINE_API ULevelBase : public UObject, public FNetworkNotify
{
	DECLARE_ABSTRACT_CLASS(ULevelBase,UObject,0,Engine)

	// Database.
	TTransArray<AActor*> Actors;

	// Variables.
	class UEngine*		Engine;
    FURL				URL;
	class UNetDriver*	NetDriver;
	class UNetDriver*	DemoRecDriver;
	FTickableObjectBase* UnMisc;

	// Constructors.
	ULevelBase( UEngine* InOwner, const FURL& InURL=FURL(NULL) );

	// UObject interface.
	void Serialize( FArchive& Ar );
	void Destroy();

	UBOOL AllowScriptDestroy()
	{
		return FALSE;
	}

	// FNetworkNotify interface.
	void NotifyProgress( const TCHAR* Str1, const TCHAR* Str2, FLOAT Seconds );

	virtual UBOOL Exec(const TCHAR* Cmd, FOutputDevice& Ar = *GLog) _VF_BASE_RET(0);

protected:
	ULevelBase()
	: Actors( this )
	{}
};

/*-----------------------------------------------------------------------------
	ULevel class.
-----------------------------------------------------------------------------*/

//
// Trace actor options.
//
enum ETraceActorFlags
{
	// Bitflags.
	TRACE_Pawns			= 0x01, // Check collision with pawns.
	TRACE_Movers		= 0x02, // Check collision with movers.
	TRACE_Level			= 0x04, // Check collision with level geometry.
	TRACE_ZoneChanges	= 0x08, // Check collision with soft zone boundaries (UNUSED!).
	TRACE_NoSortOutcome	= 0x08, // Should not sort the outcome hit results
	TRACE_Others		= 0x10, // Check collision with all other kinds of actors.
	TRACE_OnlyProjActor	= 0x20, // Check collision with other actors only if they are projectile targets
	TRACE_Blocking		= 0x40, // Check collision with blocking actors only
	TRACE_SingleResult	= 0x80, // Return checking after first hit with anything.
	TRACE_IgnoreHidden	= 0x100, // Ignores bHidden actors.
	TRACE_Light			= 0x200, // This is a light-ray trace.
	TRACE_Shove			= 0x400, // Trace with shove collision extent.
	TRACE_LightBlocking = 0x800, // Only trace with bShadowCast actors!
	TRACE_AISightBlock	= 0x1000, // Hit bBlockAISight actors.
	TRACE_Volumes		= 0x2000, // Do trace volumes.
	TRACE_WalkTextures	= 0x4000, // Tracing for walk texture.

	// Combinations.
	TRACE_VisBlocking	= TRACE_Level | TRACE_Movers,
	TRACE_AllColliding	= TRACE_VisBlocking | TRACE_Pawns | TRACE_Others | TRACE_Volumes,
	TRACE_AllEverything	= TRACE_AllColliding,
	TRACE_ProjTargets	= TRACE_OnlyProjActor | TRACE_AllColliding,
	TRACE_AIVisibility	= TRACE_VisBlocking | TRACE_AISightBlock,
	TRACE_Footsteps		= (TRACE_AllColliding | TRACE_Blocking | TRACE_WalkTextures) & ~TRACE_Volumes,
};

//
// Level updating.
//
#if _MSC_VER
enum ELevelTick
{
	LEVELTICK_TimeOnly		= 0,	// Update the level time only.
	LEVELTICK_ViewportsOnly	= 1,	// Update time and viewports.
	LEVELTICK_All			= 2,	// Update all.
};
#endif

//
// The level object.  Contains the level's actor list, Bsp information, and brush list.
//
class ENGINE_API ULevel : public ULevelBase
{
	DECLARE_CLASS(ULevel,ULevelBase,0,Engine)

protected:
	ULevel();

public:
	// Number of blocks of descriptive text to allocate with levels.
	enum {NUM_LEVEL_TEXT_BLOCKS=16};

	// Main variables, always valid.
	TArray<FReachSpec>		ReachSpecs;
	UModel*					Model;
	UTextBuffer*			TextBlocks[NUM_LEVEL_TEXT_BLOCKS];
	FTime                   TimeSeconds, RealTimeSeconds, NetTimeSeconds, NetSyncTimeSeconds;
	TMap<FString,FString>	TravelInfo;

	// Only valid in memory.
	FCollisionHashBase* Hash;
	class FMovingBrushTrackerBase* BrushTracker;
	AActor* FirstDeleted;
	struct FActorLink* NewlySpawned;
	UBOOL InTick, Ticked;
	INT iFirstDynamicActor, NetTag;
	BYTE ZoneDist[64][64];
	UBOOL bShowOctree, bShowRbPhysics;
	struct FRenderOctree* DrawOctree;
	BYTE bNetTimingOutOfSync;

	ULevel* ParentLevel, * NextChild;
	class AReplicationInfo* RIList;
	TMap<FName, ULevel*> SublevelMap;
	INT NetworkID;
	TArray<ULevel*> NetworkIDList;
	TArray<AActor *> StaticLightList; //for HW lighting only!
	UTravelDataManager* ServerTravelData;
	PX_SceneBase* PhysicsScene;

	// Temporary stats.
	INT NetTickCycles, NetDiffCycles, ActorTickCycles, AudioTickCycles, FindPathCycles, MoveCycles, NumMoves, NumReps, NumPV, GetRelevantCycles, NumRPC, SeePlayer, Spawning, Unused, IsEntry;

	// Constructor.
	ULevel( UEngine* InEngine, UBOOL RootOutside );

	// UObject interface.
	void Serialize( FArchive& Ar );
	void Destroy();
	void PostLoad();

	// ULevel interface.
	virtual void Modify( UBOOL DoTransArrays=0 );
	virtual void SetActorCollision( UBOOL bCollision );
	virtual void Tick( ELevelTick TickType, FLOAT DeltaSeconds );
	virtual void TickNetClient( FLOAT DeltaSeconds );
	virtual void TickNetServer( FLOAT DeltaSeconds );
	virtual INT ServerTickClients( FLOAT DeltaSeconds );
	virtual void ReconcileActors();
	virtual void RememberActors();
	UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar=*GLog );
	virtual void ShrinkLevel();
	virtual void CompactActors();
	virtual UBOOL Listen( FString& Error );
	virtual UBOOL IsServer();
	virtual UBOOL MoveActor( AActor *Actor, FVector Delta, FRotator NewRotation, FCheckResult &Hit, UBOOL Test=0, UBOOL IgnorePawns=0, UBOOL bIgnoreBases=0, UBOOL bNoFail=0 );
	virtual UBOOL FarMoveActor(AActor* Actor, FVector DestLocation, UBOOL Test = 0, UBOOL bNoCheck = 0, FRotator* DestRotation = NULL);
	virtual UBOOL DropToFloor( AActor* Actor );
	virtual UBOOL DestroyActor( AActor* Actor, UBOOL bNetForce=0 );
	virtual void CleanupDestroyed( UBOOL bForce );
	virtual AActor* SpawnActor( UClass* Class, FName InName=NAME_None, AActor* Owner=NULL, class APawn* Instigator=NULL, FVector Location=FVector(0,0,0), FRotator Rotation=FRotator(0,0,0), AActor* Template=NULL, UBOOL bNoCollisionFail=0, UBOOL bRemoteOwned=0 );
	virtual ABrush*	SpawnBrush();
	virtual void SpawnViewActor( UViewport* Viewport );
	virtual APlayerPawn* SpawnPlayActor( UPlayer* Viewport, ENetRole RemoteRole, const FURL& URL, FString& Error );
	virtual void SetActorZone( AActor* Actor, UBOOL bTest=0, UBOOL bForceRefresh=0 );
	void GetPointRegion(AActor* Actor, const FVector& Location, ALevelInfo* Level, FPointRegion& Region);
	virtual UBOOL FindSpot( AActor* Src, const FVector& Extent, FVector& Location, UBOOL bCheckActors, UBOOL bAssumeFit );
	virtual void AdjustSpot( AActor* Src, FVector &Adjusted, FVector TraceDest, FLOAT TraceLen, FCheckResult &Hit, DWORD Flags );
	virtual UBOOL CheckEncroachment( AActor* Actor, FVector TestLocation, FRotator TestRotation, UBOOL bTouchNotify );
	virtual BYTE FastLineCheck( const FVector& Start, const FVector& End, DWORD TraceFlags, AActor* Source=NULL );
	BYTE LineCheckSky(const FVector& Start, const FVector& Dir, DWORD TraceFlags, AActor* Source = NULL);
	virtual UBOOL SinglePointCheck( FCheckResult& Hit, const FVector& Location, const FVector& Extent, BYTE ExtraNodeFlags, DWORD TraceFlags, AActor* Source=NULL );
	virtual UBOOL SingleLineCheck( FCheckResult& Hit, AActor* SourceActor, const FVector& End, const FVector& Start, DWORD TraceFlags, FVector Extent=FVector(0,0,0), BYTE NodeFlags=0 );
	virtual FCheckResult* MultiPointCheck( FMemStack& Mem, const FVector& Location, const FVector& Extent, BYTE ExtraNodeFlags, DWORD TraceFlags, AActor* Source=NULL );
	virtual FCheckResult* MultiLineCheck( FMemStack& Mem, FVector& End, const FVector& Start, const FVector& Size, BYTE ExtraNodeFlags, DWORD TraceFlags, AActor* Source=NULL );
	virtual void InitStats();
	virtual void GetStats( TCHAR* Result );
	virtual void DetailChange( UBOOL NewDetail );
	virtual INT TickDemoRecord( FLOAT DeltaSeconds );
	virtual INT TickDemoPlayback( FLOAT DeltaSeconds );
	virtual void UpdateTime( ALevelInfo* Info );
	virtual void WelcomePlayer( UNetConnection* Connection );
	void Flush(UBOOL AllowPrecache);
	void SyncNetTimeSeconds(FLOAT NewTime);
	void InitializeLevel(const TCHAR* Options, FString& Error, UBOOL bLoadGame);
	class UTravelDataManager* GetTravelData();
	void SetMirrorMode(UBOOL bLoadGame);
	void DrawPhysicsDebug();

	// Physics engine.
	void ExitPhysics();

	// FNetworkNotify interface.
	EAcceptConnection NotifyAcceptingConnection();
	void NotifyAcceptedConnection( class UNetConnection* Connection );
	UBOOL NotifyAcceptingChannel( struct FChannel* Channel );
	ULevel* NotifyGetLevel() {return this;}
	void NotifyReceivedText( UNetConnection* Connection, const TCHAR* Text );
	void NotifyReceivedFile( UNetConnection* Connection, INT PackageIndex, const TCHAR* Error, UBOOL Skipped );
	UBOOL NotifySendingFile( UNetConnection* Connection, FGuid GUID );

	// Accessors.
	ABrush* Brush()
	{
		guardSlow(ULevel::Brush);
		check(Actors.Num()>=2);
		check(Actors(1)!=NULL);
		check(Actors(1)->Brush!=NULL);
		return (ABrush*)Actors(1);
		unguardSlow;
	}
	INT GetActorIndex( AActor* Actor )
	{
		guard(ULevel::GetActorIndex);
		for( INT i=0; i<Actors.Num(); i++ )
			if( Actors(i) == Actor )
				return i;
		appErrorf( TEXT("Actor not found: %ls"), Actor->GetFullName() );
		return INDEX_NONE;
		unguard;
	}
	ALevelInfo* GetLevelInfo()
	{
		guardSlow(ULevel::GetLevelInfo);
		check(Actors(0));
		check(Actors(0)->IsA(ALevelInfo::StaticClass()));
		return (ALevelInfo*)Actors(0);
		unguardSlow;
	}
	AZoneInfo* GetZoneActor( INT iZone )
	{
		guardSlow(ULevel::GetZoneActor);
		return Model->Zones[iZone].ZoneActor ? Model->Zones[iZone].ZoneActor : GetLevelInfo();
		unguardSlow;
	}
	void TraceVisible
	(
		FVector&		vTraceDirection,
		FCheckResult&	Hit,			// Item hit.
		AActor*			SourceActor,	// Source actor, this or its parents is never hit.
		const FVector&	Start,			// Start location.
		DWORD           TraceFlags,		// Trace flags.
		int				iDistance
	);
	void TraceVisibleObjects
	(
		UClass*			ParentClass,
		FVector&		vTraceDirection,
		FCheckResult&	Hit,			// Item hit.
		AActor*			SourceActor,	// Source actor, this or its parents is never hit.
		const FVector&	Start,			// Start location.
		DWORD           TraceFlags,		// Trace flags.
		int				iDistance
	);
};

/*-----------------------------------------------------------------------------
	Iterators.
-----------------------------------------------------------------------------*/

//
// Iterate through all static brushes in a level.
//
class FStaticBrushIterator
{
public:
	// Public constructor.
	FStaticBrushIterator( ULevel *InLevel )
		: ActorList(InLevel->Actors), Index(INDEX_NONE)
	{
		++(*this);
	}
	void operator++()
	{
		while (++Index < ActorList.Num() && (!ActorList(Index) || !ActorList(Index)->IsStaticBrush()));
	}
	ABrush* operator* ()
	{
		return (ABrush*)ActorList(Index);
	}
	ABrush* operator-> ()
	{
		return (ABrush*)ActorList(Index);
	}
	operator UBOOL()
	{
		return Index < ActorList.Num();
	}
protected:
	TTransArray<AActor*>& ActorList;
	INT Index;
};

//
// Class for iterating through all actors.
//
template< class T > class TActorIterator
{
public:
	TActorIterator(ULevel* InLevel)
		: ActorList(InLevel->Actors), Index(INDEX_NONE)
	{
		++(*this);
	}
	void operator++()
	{
		while (++Index < ActorList.Num() && (!ActorList(Index) || !ActorList(Index)->IsA(T::StaticClass())));
	}
	T* operator*()
	{
		return (T*)ActorList(Index);
	}
	T* operator->()
	{
		return (T*)ActorList(Index);
	}
	operator UBOOL()
	{
		return Index < ActorList.Num();
	}
private:
	TTransArray<AActor*>& ActorList;
	INT Index;
};

// Simple AllActors iterator.
class FActorIterator
{
public:
	FActorIterator(ULevel* InLevel)
		: ActorList(InLevel->Actors), Index(INDEX_NONE)
	{
		++(*this);
	}
	void operator++()
	{
		while (++Index < ActorList.Num() && !ActorList(Index));
	}
	AActor* operator*()
	{
		return ActorList(Index);
	}
	AActor* operator->()
	{
		return ActorList(Index);
	}
	operator UBOOL()
	{
		return Index < ActorList.Num();
	}
private:
	TTransArray<AActor*>& ActorList;
	INT Index;
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
