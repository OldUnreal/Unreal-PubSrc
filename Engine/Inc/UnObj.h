/*=============================================================================
	UnObj.h: Standard Unreal object definitions.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

/*----------------------------------------------------------------------------
	Forward declarations.
----------------------------------------------------------------------------*/

// All engine classes.
class		UBitmap;
class		UTexture;
class		UFont;
class		UPalette;
class		USound;
class		UMusic;
class		UPrimitive;
class		UMesh;
class		UModel;
class		UPolys;
class		ULevelBase;
class		ULevel;
class		UPendingLevel;
class		UNetConnection;
class		UConsole;
class		UStaticLightData;
struct		FChannel;
struct		FActorChannel;

// Other classes.
struct FTextureInfo;
class  AActor;
class  ABrush;
class  FDecal;
class  ADecal;
class  FScan;
class  AProjector;
class  UPX_SkeletalBody;
struct FMeshInstanceBase;
struct FLightVisibilityCache;
struct FComputedReachability;
struct FStaticProjector;
struct FDynamicSprite;
struct FSceneNode;

struct FShadowMoverInfo
{
	class AMover* Mover;
	TArrayNoInit<INT> BspSurfs;
};
struct FDynLightEntry
{
	AActor* Actor;
	TArray<AActor*> MoverList;

	FDynLightEntry( AActor* LightActor )
		: Actor(LightActor)
	{}
};
struct FStaticLightActor
{
	virtual AActor** GetDynamicLights(INT* NumLights) _VF_BASE_RET(NULL);
	virtual UStaticLightData* GetStaticMeshData() const { return nullptr; }
	virtual void Flush() {}

	virtual ~FStaticLightActor() noexcept(false)
	{}
protected:
	FStaticLightActor()
	{}
};

//
// Advanced animation enum.
//
enum eAnimNotifyEval
{
	ANE_Equal,
	ANE_Greater,
	ANE_Less,
	ANE_GreaterOrEqual,
	ANE_LessOrEqual,
	ANE_MAX,
};

/*-----------------------------------------------------------------------------
	UBspNode.
-----------------------------------------------------------------------------*/

// Flags associated with a Bsp node.
enum EBspNodeFlags
{
	// Flags.
	NF_NotCsg			= 0x001, // Node is not a Csg splitter, i.e. is a transparent poly.
	NF_ShootThrough		= 0x002, // Can shoot through (for projectile solid ops).
	NF_NotVisBlocking   = 0x004, // Node does not block visibility, i.e. is an invisible collision hull.
	NF_PolyOccluded		= 0x008, // Node's poly was occluded on the previously-drawn frame.
	NF_BoxOccluded		= 0x010, // Node's bounding box was occluded.
	NF_HitBoxes			= NF_BoxOccluded, // Reuse for trace skeletal mesh hitboxes.
	NF_IsNew 		 	= 0x020, // Editor: Node was newly-added.
	NF_HitTexture 		= NF_IsNew, // Reuse for collision tracing: return hit texture info.
	NF_IsFront     		= 0x040, // Filter operation bounding-sphere precomputed and guaranteed to be front.
	NF_IsBack      		= 0x080, // Guaranteed back.
	NF_BrightCorners	= NF_PolyOccluded, // Temporary.

	// Combinations of flags.
	NF_NeverMove		= 0, // Bsp cleanup must not move nodes with these tags.
	NF_KeepTraceFlags	= (NF_NotCsg | NF_ShootThrough | NF_NotVisBlocking | NF_BrightCorners), // Flags to keep for BSP tracing.
};


//
// Identifies a unique convex volume in the world.
//
struct ENGINE_API FPointRegion
{
	// Variables.
	class AZoneInfo* Zone;			// Zone actor.
	INT				 iLeaf;			// Bsp leaf.
	BYTE             ZoneNumber;	// Zone number.

    // Constructors
    FPointRegion(void) : Zone(NULL), iLeaf(0), ZoneNumber(0) {}

	FPointRegion( class AZoneInfo* InLevel )
	:	Zone(InLevel), iLeaf(INDEX_NONE), ZoneNumber(0)
	{}
	FPointRegion( class AZoneInfo* InZone, INT InLeaf, BYTE InZoneNumber )
	:	Zone(InZone), iLeaf(InLeaf), ZoneNumber(InZoneNumber)
	{}
};

//
// FBspNode defines one node in the Bsp, including the front and back
// pointers and the polygon data itself.  A node may have 0 or 3 to (MAX_NODE_VERTICES-1)
// vertices. If the node has zero vertices, it's only used for splitting and
// doesn't contain a polygon (this happens in the editor).
//
// vNormal, vTextureU, vTextureV, and others are indices into the level's
// vector table.  iFront,iBack should be INDEX_NONE to indicate no children.
//
// If iPlane==INDEX_NONE, a node has no coplanars.  Otherwise iPlane
// is an index to a coplanar polygon in the Bsp.  All polygons that are iPlane
// children can only have iPlane children themselves, not fronts or backs.
//
class FBspNode // 64 bytes
{
public:
	enum {MAX_NODE_VERTICES=16};	// Max vertices in a Bsp node, pre clipping.
	enum {MAX_FINAL_VERTICES=24};	// Max vertices in a Bsp node, post clipping.
	enum {MAX_ZONES=64};			// Max zones per level.

    FBspNode(void) :
        Plane(0.0f,0.0f,0.0f,0.0f),
        ZoneMask(0),
        iVertPool(0),
        iSurf(0),
        iBack(0),
        iFront(0),
        iPlane(0),
        iCollisionBound(0),
        iRenderBound(0),
        NumVertices(0),
        NodeFlags(0)
    {
        iZone[0] = iZone[1] = 0;
        iLeaf[0] = iLeaf[1] = 0;
    }

	// Persistent information.
	FPlane			Plane;			// 16 Plane the node falls into (X, Y, Z, W).
	QWORD			ZoneMask;		// 8  Bit mask for all zones at or below this node (up to 64).
	INT				iVertPool;		// 4  Index of first vertex in vertex pool, =iTerrain if NumVertices==0 and NF_TerrainFront.
	INT				iSurf;			// 4  Index to surface information.

	// iBack:  4  Index to node in front (in direction of Normal).
	// iFront: 4  Index to node in back  (opposite direction as Normal).
	// iPlane: 4  Index to next coplanar poly in coplanar list.
	union { INT iBack; INT iChild[1]; };
	        INT iFront;
			INT iPlane;

	INT				iCollisionBound;// 4  Collision bound.
	INT				iRenderBound;	// 4  Rendering bound.
	BYTE			iZone[2];		// 2  Visibility zone in 1=front, 0=back.
	BYTE			NumVertices;	// 1  Number of vertices in node.
	BYTE			NodeFlags;		// 1  Node flags.
	INT				iLeaf[2];		// 8  Leaf in back and front, INDEX_NONE=not a leaf.

	// Functions.
	UBOOL IsCsg( DWORD ExtraFlags=0 ) const
	{
		return (NumVertices>0) && !(NodeFlags & (NF_IsNew | NF_NotCsg | ExtraFlags));
	}
	UBOOL ChildOutside( INT iChild, UBOOL Outside, DWORD ExtraFlags=0 ) const
	{
		return iChild ? (Outside || IsCsg(ExtraFlags)) : (Outside && !IsCsg(ExtraFlags));
	}
	ENGINE_API friend FArchive& operator<<( FArchive& Ar, FBspNode& N );
};

//
// Properties of a zone.
//
class ENGINE_API FZoneProperties
{
public:
	// Variables.
	AZoneInfo*	ZoneActor;		// Optional actor defining the zone's property.
	FLOAT		LastRenderTime;	// Most recent level TimeSeconds when rendered.
	QWORD		Connectivity;	// (Connect[i]&(1<<j))==1 if zone i is adjacent to zone j.
	QWORD		Visibility;		// (Connect[i]&(1<<j))==1 if zone i can see zone j.

    FZoneProperties(void) :
        ZoneActor(NULL), LastRenderTime(0.0f),
        Connectivity(0), Visibility(0)
    {
        // no-op.
    }

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, FZoneProperties& P )
	{
		guard(FZoneProperties<<);
		return Ar << *(UObject**)&P.ZoneActor << P.Connectivity << P.Visibility;
//		Ar << P.LastRenderTime;
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	UBspLeaves.
-----------------------------------------------------------------------------*/

//
// Information about a convex volume.
//
class FLeaf
{
public:
	// Variables.
	INT iZone;          // The zone this convex volume is in.
	INT iPermeating;    // Lights permeating this volume considering shadowing.
	INT iVolumetric;    // Volumetric lights hitting this region, no shadowing.
	QWORD VisibleZones; // Bit mask of visible zones from this convex volume.

	// Functions.
	FLeaf()
    : iZone(0), iPermeating(0), iVolumetric(0), VisibleZones(0)
	{}

	FLeaf( INT iInZone, INT InPermeating, INT InVolumetric, QWORD InVisibleZones )
	:	iZone(iInZone), iPermeating(InPermeating), iVolumetric(InVolumetric), VisibleZones(InVisibleZones)
	{}
	friend FArchive& operator<<( FArchive& Ar, FLeaf& L )
	{
		guard(FLeaf<<);
		return Ar << AR_INDEX(L.iZone) << AR_INDEX(L.iPermeating) << AR_INDEX(L.iVolumetric) << L.VisibleZones;
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	Decals & Projectors
-----------------------------------------------------------------------------*/

// Projector entry.
struct FProjector
{
	AProjector* ProjActor;
	TMap < INT, struct FProjectorSurf*> StaticRendMap;
	BYTE bHasStatic;

	FProjector(AProjector* P)
		:ProjActor(P), bHasStatic(0)
	{}
};

//
// Decal associated with a surface
//
class FDecal
{
public:
	// Variables.
	FVector Vertices[4];	// Vertices of decal on surface (offset from the surface base).
	ADecal*	Actor;			// ADecal actor associated with this decal.
	TArray<INT> Nodes;		// The FBspNode indices on which this decal lies.
	QWORD SurfNormal;		// Attached surface normal, don't render anymore if mover rotates.

	FDecal() : Actor(NULL) {}
    FDecal(ADecal* A) : Actor(A) {}
};

/*-----------------------------------------------------------------------------
	UBspSurf.
-----------------------------------------------------------------------------*/

struct FBspRenderCache
{
	virtual ~FBspRenderCache() {}
};

//
// One Bsp polygon.  Lists all of the properties associated with the
// polygon's plane.  Does not include a point list; the actual points
// are stored along with Bsp nodes, since several nodes which lie in the
// same plane may reference the same poly.
//
class FBspSurf
{
public:

	// Persistent info.
	UTexture*	Texture;		// 4 Texture map.
	DWORD		PolyFlags;		// 4 Polygon flags.
	DWORD		ExtraPolyFlags;	// 227k: Unserialized data.
	INT			pBase;			// 4 Polygon & texture base point index (where U,V==0,0).
	INT			vNormal;		// 4 Index to polygon normal.
	INT			vTextureU;		// 4 Texture U-vector index.
	INT			vTextureV;		// 4 Texture V-vector index.
	INT			iLightMap;		// 4 Light mesh.
	INT			iBrushPoly;		// 4 Editor brush polygon index.
	SWORD		PanU;			// 2 U-Panning value.
	SWORD		PanV;			// 2 V-Panning value.

	// Persistent data, not serialized.
	ABrush*		Actor;			// 4 Brush actor owning this Bsp surface.
	TArray<FDecal>	Decals;		// 12 Array decals on this surface
	TArray<FProjector> Projectors;		// 12 Array projectors on this surface
	TArray<INT>	Nodes;			// 12 Nodes which make up this surface
	FBspRenderCache* RenderCache; // Internal render cache information.
	UINT		UpdateTag;		// Modify tag to be used by dynamic BSP to notify render when this has been altered.

    FBspSurf()
	:	Texture(NULL)
	,	PolyFlags(0)
	,	ExtraPolyFlags(0)
	,	pBase(0)
	,	vNormal(0)
	,	vTextureU(0)
	,	vTextureV(0)
	,	iLightMap(0)
	,	iBrushPoly(0)
	,	PanU(0)
	,	PanV(0)
	,	Actor(NULL)
	,	RenderCache(NULL)
	,	UpdateTag(0)
    {
        // no-op
    }
	~FBspSurf()
	{
		FlushCache();
	}

	inline void FlushCache()
	{
		guardSlow(FBspSurf::FlushCache);
		if (RenderCache)
		{
			delete RenderCache;
			RenderCache = NULL;
		}
		unguardSlow;
	}
	inline void Cleanup()
	{
		guardSlow(FBspSurf::Cleanup);
		Decals.Empty();
		Projectors.Empty();
		FlushCache();
		unguardSlow;
	}

	// Functions.
	ENGINE_API friend FArchive& operator<<( FArchive& Ar, FBspSurf& Surf );
};

#include "UnPolyFlag.h"

inline void AppEndStrLine(TCHAR* Dest, const TCHAR* Src)
{
	if (!*Dest)
		appStrcpy(Dest, Src);
	else
	{
		appStrcat(Dest, TEXT(","));
		appStrcat(Dest, Src);
	}
}
inline const TCHAR* GetEPolyFlags(DWORD Flags)
{
	TCHAR* Res = appStaticString1024();
#define DOCASE(Flag) if (Flags & Flag) AppEndStrLine(Res, reinterpret_cast<const TCHAR*>(TEXT(#Flag))+3);
	DOCASE(PF_Invisible);
	DOCASE(PF_Masked);
	DOCASE(PF_Translucent);
	DOCASE(PF_NotSolid);
	DOCASE(PF_Environment);
	DOCASE(PF_Semisolid);
	DOCASE(PF_Modulated);
	DOCASE(PF_FakeBackdrop);
	DOCASE(PF_TwoSided);
	DOCASE(PF_AutoUPan);
	DOCASE(PF_AutoVPan);
	DOCASE(PF_NoSmooth);
	DOCASE(PF_SpecialPoly);
	DOCASE(PF_SmallWavy);
	DOCASE(PF_ForceViewZone);
	DOCASE(PF_LowShadowDetail);
	DOCASE(PF_NoMerge);
	DOCASE(PF_AlphaBlend);
	DOCASE(PF_DirtyShadows);
	DOCASE(PF_BrightCorners);
	DOCASE(PF_SpecialLit);
	DOCASE(PF_NoBoundRejection);
	DOCASE(PF_Unlit);
	DOCASE(PF_HighShadowDetail);
	DOCASE(PF_Portal);
	DOCASE(PF_Mirrored);
	DOCASE(PF_Selected);
	DOCASE(PF_RenderHint);
	DOCASE(PF_RenderFog);
	DOCASE(PF_Highlighted);
	DOCASE(PF_Occlude);
#undef DOCASE
	return Res;
}

/*-----------------------------------------------------------------------------
	FLightMapIndex.
-----------------------------------------------------------------------------*/

//
// A shadow occlusion mask.
//
class ENGINE_API FShadowMask
{
public:
	AActor* Owner;
	TArray<BYTE> ShadowData;

    FShadowMask(void) : Owner(NULL) {}

	friend FArchive& operator<<( FArchive& Ar, FShadowMask& M )
	{
		return Ar << *(UObject**)&M.Owner << M.ShadowData;
	}
};

//
// Describes the mesh-based lighting applied to a Bsp poly.
//
class ENGINE_API FLightMapIndex
{
public:
	INT		DataOffset;
	INT		iLightActors;
	FVector Pan;
	FLOAT	UScale, VScale;
	INT     UClamp, VClamp;
	BYTE	UBits, VBits;
	TArray<FDynLightEntry> DynLights;
	BYTE	DynLightsChange;

    FLightMapIndex(void) :
        DataOffset(0), iLightActors(0), UScale(0.0f), VScale(0.0f),
        UClamp(0), VClamp(0), UBits(0), VBits(0)
    {
        // no-op.
    }

	friend FArchive& operator<<( FArchive& Ar, FLightMapIndex& I )
	{
		guard(FLightMapIndex<<);
		Ar << I.DataOffset;
		Ar << I.Pan;
		Ar << AR_INDEX(I.UClamp) << AR_INDEX(I.VClamp);
		Ar << I.UScale << I.VScale;
		Ar << I.iLightActors;
		return Ar;
		unguard;
	}
	inline FDynLightEntry& GetEntry( AActor* LightActor )
	{
		for( INT i=0; i<DynLights.Num(); ++i )
			if( DynLights(i).Actor==LightActor )
				return DynLights(i);
		FDynLightEntry& E = DynLights(DynLights.AddZeroed(1));
		E.Actor = LightActor;
		return E;
	}
};

//
// Describes the mesh-based 3rd render pass for translucent meshes.
//
enum EActorRenderPass : BYTE
{
	RENDPASS_None,		// Uninitialized render pass!
	RENDPASS_First,		// Solid objects only
	RENDPASS_Second,	// Transparent/Solid meshes
	RENDPASS_Third,		// Fully transperent objects
};

#define ACTOR_VIEWFADE_DIST 0.1f // Distance to when Actor.VisibilityRadius should start to fade in/out actor.

/*-----------------------------------------------------------------------------
	UPolys.
-----------------------------------------------------------------------------*/

// Results from FPoly.SplitWithPlane, describing the result of splitting
// an arbitrary FPoly with an arbitrary plane.
enum ESplitType
{
	SP_Coplanar		= 0, // Poly wasn't split, but is coplanar with plane
	SP_Front		= 1, // Poly wasn't split, but is entirely in front of plane
	SP_Back			= 2, // Poly wasn't split, but is entirely in back of plane
	SP_Split		= 3, // Poly was split into two new editor polygons
};

//
// A general-purpose polygon used by the editor.  An FPoly is a free-standing
// class which exists independently of any particular level, unlike the polys
// associated with Bsp nodes which rely on scads of other objects.  FPolys are
// used in UnrealEd for internal work, such as building the Bsp and performing
// boolean operations.
//
class ENGINE_API FPoly
{
public:
#if 0 //WDM -- remove this...?
	// The changes to these values allow things like the 2D shape editor and brush clipping
	// to create large polys with many sides if they need to.
	//
	enum {MAX_VERTICES=32}; // Maximum vertices an FPoly may have.
#else
	enum {MAX_VERTICES=32}; // Maximum vertices an FPoly may have. Why not trying with 32- Smirftsch
#endif
	enum {VERTEX_THRESHOLD=MAX_VERTICES-2}; // Threshold for splitting into two.

	FVector     Base;        	// Base point of polygon.
	FVector     Normal;			// Normal of polygon.
	FVector     TextureU;		// Texture U vector.
	FVector     TextureV;		// Texture V vector.
	FVector     Vertex[MAX_VERTICES]; // Actual vertices.
	FVector		VertexDeltas[MAX_VERTICES]; // stijn: vertex deltas accumulated but not yet applied due to grid snapping
	DWORD       PolyFlags;		// FPoly & Bsp poly bit flags (PF_).
	DWORD		ExtraPolyFlags; // 227k: Bonus polyflags.
	ABrush*		Actor;			// Brush where this originated, or NULL.
	UTexture*	Texture;		// Texture map.
	FName		ItemName;		// Item name.
	INT			NumVertices;	// Number of vertices.
	INT			iLink;			// iBspSurf, or brush fpoly index of first identical polygon, or MAXWORD.
	INT			iBrushPoly;		// Index of editor solid's polygon this originated from.
	SWORD		PanU,PanV;		// Texture panning values.
	INT			SavePolyIndex;	// Used by multiple vertex editing to keep track of original PolyIndex into owner brush
	UBOOL		bFaceDragSel;

    FPoly(void) :
        PolyFlags(PF_None), ExtraPolyFlags(EPF_None), Actor(NULL), Texture(NULL), NumVertices(0),
        iLink(0), iBrushPoly(0), PanU(0), PanV(0), SavePolyIndex(0),
        bFaceDragSel(0)
    {
        // no-op
    }

	// Custom functions.
	void  Init				();
	void  Reverse			();
	void  SplitInHalf		(FPoly *OtherHalf);
	void  Transform			(const FModelCoords &Coords, const FVector &PreSubtract,const FVector &PostAdd, FLOAT Orientation);
	int   Fix				();
	int   CalcNormal		( UBOOL bSilent = 0 );
	int   SplitWithPlane	(const FVector &Base,const FVector &Normal,FPoly *FrontPoly,FPoly *BackPoly,int VeryPrecise) const;
	int   SplitWithNode		(const UModel *Model,INT iNode,FPoly *FrontPoly,FPoly *BackPoly,int VeryPrecise) const;
	int   SplitWithPlaneFast(const FPlane Plane,FPoly *FrontPoly,FPoly *BackPoly) const;
	int   Split				(const FVector &Normal, const FVector &Base, int NoOverflow=0 );
	int   RemoveColinears	();
	int   Finalize			(int NoError);
	int   Faces				(const FPoly &Test) const;
	FLOAT Area				();
	void DiscardVertexDeltas(UBOOL All); // stijn: set to true to reinitialize all deltas (i.e., MAX_VERTICES)
	UBOOL MergePolys		(FPoly& Other); // Marco: Attempt to merge vertices with another polygon.

	// Serializer.
	ENGINE_API friend FArchive& operator<<( FArchive& Ar, FPoly& Poly );

	// Inlines.
	inline UBOOL IsBackfaced( const FVector &Point ) const
	{
		return ((Point-Vertex[0]) | Normal) < 0.f;
	}
	inline UBOOL IsCoplanar( const FPoly &Test ) const
	{
		if (Abs(Normal | Test.Normal) < 0.9999f)
			return FALSE;
		const FLOAT FirstDot = Abs((Vertex[0] - Test.Vertex[0]) | Normal);
		if (FirstDot < THRESH_POINT_ON_PLANE)
			return TRUE;
		if (FirstDot < 1.f)
		{
			// Check with all verts because numerical precision error may cause verts to be just slightly off.
			INT j;
			for (INT i = 0; i < NumVertices; ++i)
			{
				for (j = 0; j < Test.NumVertices; ++j)
					if (Abs((Vertex[i] - Test.Vertex[j]) | Normal) < THRESH_POINT_ON_PLANE)
						return TRUE;
			}
		}
		return FALSE;
	}
	inline UBOOL CanBeMerged(const FPoly& Other) const
	{
		return ((Texture == Other.Texture) && (TextureU == Other.TextureU) && (TextureV == Other.TextureV) && (PolyFlags == Other.PolyFlags) && (ExtraPolyFlags == Other.ExtraPolyFlags) && IsCoplanar(Other));
	}
};

//
// List of FPolys.
//
class ENGINE_API UPolys : public UObject
{
	DECLARE_CLASS(UPolys,UObject,(CLASS_RuntimeStatic | CLASS_EditorOnly),Engine)

	// Elements.
	TTransArray<FPoly> Element;

	// 227k model data:
	TArrayNoInit<DWORD> ExtraFlags;

	// Constructors.
	UPolys()
	: Element( this )
	{}

	// UObject interface.
	void StaticConstructor();
	void Serialize(FArchive& Ar);
};

/*-----------------------------------------------------------------------------
	FVerts.
-----------------------------------------------------------------------------*/

//
// One vertex associated with a Bsp node's polygon.  Contains a vertex index
// into the level's FPoints table, and a unique number which is common to all
// other sides in the level which are cospatial with this side.
//
class FVert
{
public:
	// Variables.
	INT 	pVertex;	// Index of vertex.
	INT		iSide;		// If shared, index of unique side. Otherwise INDEX_NONE.

    FVert(void) :
        pVertex(0),
        iSide(0)
    {
        // no-op
    }

	// Functions.
	friend FArchive& operator<< (FArchive &Ar, FVert &Vert)
	{
		guard(FVert<<);
		return Ar << AR_INDEX(Vert.pVertex) << AR_INDEX(Vert.iSide);
		unguard;
	}
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*----------------------------------------------------------------------------
	FSavedGameInfo.
----------------------------------------------------------------------------*/

struct FSavedGameInfo
{
	FString MapTitle, Timestamp, ExtraInfo;
	class UTexture* Screenshot;

	FSavedGameInfo()
		: Screenshot(NULL)
	{}
};

/*----------------------------------------------------------------------------
	Fog Render.
----------------------------------------------------------------------------*/

// Definition for a surface that has fog
struct FFogSurf
{
	struct FSavedPoly* Polys;
	FLOAT		FogDistanceStart;	// Zone fog start (zero if none)
	FLOAT		FogDistanceEnd;		// Zone fog distance (zero if none)
	FLOAT		FogDensity;			// for exponential fog.
	FPlane		FogColor;			// Zone fog color
	INT			FogMode;			// 0 Linear, 1 Exponential, 2 Exponential2
	DWORD		PolyFlags;			// Surface flags.
	BYTE		bBSP;				// this flag is unfortunately needed for the old renderers to emulate the fog.
	FFogSurf()
		:Polys(NULL),
		FogDistanceStart(0.f),
		FogDistanceEnd(0.f),
		FogDensity(0.f),
		FogColor(FPlane(0.f, 0.f, 0.f, 0.f)),
		FogMode(0),
		PolyFlags(0),
		bBSP(0)
	{}
	inline BYTE IsValid()
	{
		return (((FogDistanceEnd > FogDistanceStart) && FogMode == 0) || (FogDensity > 0.f && (FogMode == 1 || FogMode == 2)));
	}
};

/*-----------------------------------------------------------------------------
Networking
-----------------------------------------------------------------------------*/

#define NETCOND_PARAMS BYTE* Recent, INT* RepLink
#define NETCOND_CALLSUPER RepLink = Super::NativeReplicationRep(Recent, RepLink)
#define NETCOND_RETURNVAL return RepLink

//
// Priority sortable list.
//
struct FActorPriority
{
	INT						Priority;	// Update priority, higher = more important.
	AActor*					Actor;		// Actor.
	struct FActorChannel*	Channel;	// Actor channel.
	FActorPriority*			Next;		// Next on full-list.
	UBOOL					bNetForced;	// Actor was alwaysrelevant (or owned).

	FActorPriority(FActorPriority* N, AActor* InActor)
		: Actor(InActor), Next(N)
	{}
	void CalcPriority(FVector& ViewPos, FVector& ViewDir, UNetConnection* InConnection, UBOOL bWasForced);
};

struct FInterpolateData
{
	AActor* Actor;
	FVector Start, CachePos;
	FQuat StartDir;
	FRotator CacheRot;
	FLOAT StartTime, InterpolateTime;
	UINT LastTime;
	UBOOL bExpired;

	FInterpolateData(const FVector& StartPos, const FRotator& StartRot, AActor* TargetActor, FLOAT InterpTime);
	void GetRenderPos(FVector* Pos, FRotator* Rot);
	void UpdateNewPos(FLOAT InterpTime);
	UBOOL HasExpired() const;
};

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
