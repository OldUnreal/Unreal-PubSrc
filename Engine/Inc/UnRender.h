/*=============================================================================
	UnRender.h: Rendering functions and structures
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _UNRENDER_H_
#define _UNRENDER_H_

/*------------------------------------------------------------------------------------
	Forward declarations.
------------------------------------------------------------------------------------*/

class  URenderDevice;
class  FSpan;
class  FSpanBuffer;
class  FRasterPoly;
struct FTransTexture;
struct FScreenBounds;
struct FDebugLineData;
struct FSurfaceInfo;
struct FSurfaceFacet;
struct FSceneNode;
struct FDynamicItem;
struct FBspDrawList;
struct FSavedPoly;
struct FDynamicSprite;
struct FFogSurf;
struct FMeshBufferData;
struct FDynamicLight;

/*------------------------------------------------------------------------------------
	Includes.
------------------------------------------------------------------------------------*/

#include "UnRenDev.h"

/*------------------------------------------------------------------------------------
	Defines.
------------------------------------------------------------------------------------*/

#define ORTHO_LOW_DETAIL 40000.0f
#define MAX_FULLPOLYLIST 8192

/*------------------------------------------------------------------------------------
	FSceneNode.
------------------------------------------------------------------------------------*/

enum
{
	EMAX_ClipPlanes = 32
};

//
// A scene frame is a temporary object representing a portion of
// the view of the world to render.
//
struct ENGINE_API FSceneNode
{
	// Variables.
	UViewport*		Viewport;		// Viewport the scene frame is attached to.
	ULevel*			Level;			// Level this scene is being rendered from.
	FSceneNode*		Parent;			// Frame from whence this was created, NULL=top level.
	FSceneNode*		Sibling;		// Next sibling scene frame.
	FSceneNode*		Child;			// Next child scene frame.
	INT				iSurf;			// Surface seen through (Parent,iSurface pair is unique).
	INT				ZoneNumber;		// Inital rendering zone of viewport in destination level (NOT the zone of the viewpoint!)
	INT				Recursion;		// Recursion depth, 0 if initial.
	FLOAT			Mirror;			// Mirror value, 1.f or -1.f.
	FCoords			Coords;			// Transform coordinate system.
	FCoords			Uncoords;		// Inverse coordinate system.
	FSpanBuffer*	Span;			// Initial span buffer for the scene.
	FBspDrawList*	Draw[3];		// Draw lists (portals, occluding, non-occluding).
	FDynamicSprite* Sprite;			// Sprites to draw.
	INT				X, Y;			// Frame size.
	INT				XB, YB;			// Offset of top left active viewport.
	FPlane			NearClip;		// Legacy: Clip plane to identify mergable portals.
	INT				NearClipNode;	// iNode that's is the clipping plane!

	// Precomputes.
	FLOAT			FX, FY;			// Floating point X,Y.
	FLOAT			FX15, FY15;		// (Floating point SXR + 1.0001f)/2.f.
	FLOAT			FX2, FY2;		// Floating point SXR / 2.f.
	FLOAT			Zoom;			// Zoom value, based on OrthoZoom and size.
	FVector			Proj;      		// Projection vector.
	FVector			RProj;			// Reverse projection vector.
	FLOAT			PrjXM, PrjYM;	// Minus clipping numbers.
	FLOAT			PrjXP, PrjYP;	// Plus clipping numbers.
	FVector			ViewSides [4];	// 4 unit vectors indicating view frustrum extent lines.
	FPlane			ViewPlanes[4];	// 4 planes indicating view frustrum extent planes.

	// Distance fog.
	FVector			FarPoints[4];
	FPlane			FarPlane;
	FLOAT			FarPlaneDistance;

	FDynamicLight*	DynLights;		// Dynamic lights of this scene.
	UPortalModifier* FrameModifier;	// Modifier to this frame.

	// Should ClearZ after drawing this frame (for skybox).
	BITFIELD		bShouldClearZ : 1;
	BITFIELD		bFarPlaneEnabled : 1;
	BITFIELD		bDrawViewportOwner : 1; // Should draw viewport owner (i.e. mirror reflection)

	// Software clipping planes.
	static FPlane		ClipPlanes[EMAX_ClipPlanes];
	static BYTE			NumClipPlanes;	// Num software clipping planes.

	// Functions.
	BYTE* Screen( INT X, INT Y ) {return Viewport->ScreenPointer + (X+XB+(Y+YB)*Viewport->Stride)*Viewport->ColorBytes;}
	void ComputeRenderSize();
	void ComputeRenderCoords( FVector& Location, FRotator& Rotation );
	void InitDistanceFog( BYTE bEnabled, FLOAT MaxDist );
	void SetFrameModifier(UPortalModifier* Mod);

	UBOOL PushClipPlane(const FPlane& Plane);
	void PopClipPlane();
};

inline FLOAT AZoneInfo::GetFogDistance(FLOAT ZDist, FSceneNode* Frame)
{
	if (Frame->FrameModifier && Frame->FrameModifier->bOverrideDistanceFog)
	{
		if (Frame->FrameModifier->bDistanceFogActors)
			return Clamp(1.f - ((ZDist - Frame->FrameModifier->DistanceFogStart) / (Frame->FrameModifier->DistanceFogEnd - Frame->FrameModifier->DistanceFogStart)), 0.f, 1.f);
		return 1.f;
	}
	else if (bDistanceFog && bZoneBasedFog)
		return Clamp(1.f - ((ZDist - FogDistanceStart) / (FogDistance - FogDistanceStart)), 0.f, 1.f);
	else if (Frame->Viewport->Actor->bDistanceFogEnabled)
		return Clamp(1.f - ((ZDist - Frame->Viewport->Actor->DistanceFogDistance[0]) / (Frame->Viewport->Actor->DistanceFogDistance[1] - Frame->Viewport->Actor->DistanceFogDistance[0])), 0.f, 1.f);
	return 1.f;
}

// Written by Marco for 227k: Render a frame to a texture.
struct ENGINE_API FRenderToTexture
{
	FSceneNode* Frame;
	UViewport* const Viewport;
	const INT ScreenX, ScreenY;
	BYTE* const ScreenPtr;
	const FVector Location;
	const FRotator Rotation;

	// @InViewport - Client to render it.
	// @SizeX/SizeY - Screen resolution.
	// @TargetBuffer - FColor screen buffer pointer (must be sizeof(FColor)*SizeX*SizeY)
	FRenderToTexture(UViewport* InViewport, INT SizeX, INT SizeY, BYTE* TargetBuffer, const FVector& CamPos, const FRotator& CamRot);
	FRenderToTexture() = delete;

	// Destructor will automatically pop this render state.
	~FRenderToTexture() noexcept(false);

	// Before using this you should check its valid first.
	inline UBOOL IsValid() const
	{
		return (Frame != NULL);
	}
};

/*------------------------------------------------------------------------------------
	Transformations.
------------------------------------------------------------------------------------*/

//
// Transformed vector with outcode info.
//
struct FOutVector
{
	FVector Point;
	BYTE    Flags;

	inline void ComputeOutcode(const FSceneNode* Frame)
	{
		BYTE outcode = 0;
		if ((Frame->PrjXM * Point.Z + Point.X) < 0.f)
			outcode |= FVF_OutXMin;
		if ((Frame->PrjXP * Point.Z - Point.X) < 0.f)
			outcode |= FVF_OutXMax;
		if ((Frame->PrjYM * Point.Z + Point.Y) < 0.f)
			outcode |= FVF_OutYMin;
		if ((Frame->PrjYP * Point.Z - Point.Y) < 0.f)
			outcode |= FVF_OutYMax;
		Flags = outcode;
	}
};

//
// Transformed and projected vector.
//
struct FTransform : public FOutVector
{
	FLOAT ScreenX;
	FLOAT ScreenY;
	INT   IntY;
	FLOAT RZ;
	inline void Project( const FSceneNode* Frame )
	{
		RZ      = Frame->Proj.Z / Point.Z;
		ScreenX = Point.X * RZ + Frame->FX15;
		ScreenY = Point.Y * RZ + Frame->FY15;
		IntY    = appFloor( ScreenY );
	}
	FTransform operator+( const FTransform& V ) const
	{
		FTransform Temp;
		Temp.Point = Point + V.Point;
		return Temp;
	}
	FTransform operator-( const FTransform& V ) const
	{
		FTransform Temp;
		Temp.Point = Point - V.Point;
		return Temp;
	}
	FTransform operator*(FLOAT Scale ) const
	{
		FTransform Temp;
		Temp.Point = Point * Scale;
		return Temp;
	}
};

//
// Transformed sample point.
//
struct FTransSample : public FTransform
{
	FPlane Normal, Light, Fog;
	FTransSample operator+( const FTransSample& T ) const
	{
		FTransSample Temp;
		Temp.Point = Point + T.Point;
		Temp.Light = Light + T.Light;
		Temp.Fog.X = Fog.X + T.Fog.X;
		Temp.Fog.Y = Fog.Y + T.Fog.Y;
		Temp.Fog.Z = Fog.Z + T.Fog.Z;
		Temp.Fog.W = Fog.W + T.Fog.W;
		return Temp;
	}
	FTransSample operator-( const FTransSample& T ) const
	{
		FTransSample Temp;
		Temp.Point = Point - T.Point;
		Temp.Light = Light - T.Light;
		Temp.Fog.X = Fog.X - T.Fog.X;
		Temp.Fog.Y = Fog.Y - T.Fog.Y;
		Temp.Fog.Z = Fog.Z - T.Fog.Z;
		Temp.Fog.W = Fog.W - T.Fog.W;
		return Temp;
	}
	FTransSample operator*( FLOAT Scale ) const
	{
		FTransSample Temp;
		Temp.Point = Point * Scale;
		Temp.Light = Light * Scale;
		Temp.Fog.X = Fog.X * Scale;
		Temp.Fog.Y = Fog.Y * Scale;
		Temp.Fog.Z = Fog.Z * Scale;
		Temp.Fog.W = Fog.W * Scale;
		return Temp;
	}
};

//
// Transformed texture mapped point.
//
struct FTransTexture : public FTransSample
{
	FLOAT U, V;
	FTransTexture operator+( const FTransTexture& T ) const
	{
		FTransTexture Temp;
		Temp.Point = Point + T.Point;
		Temp.Light = Light + T.Light;
		Temp.Fog.X = Fog.X + T.Fog.X;
		Temp.Fog.Y = Fog.Y + T.Fog.Y;
		Temp.Fog.Z = Fog.Z + T.Fog.Z;
		Temp.Fog.W = Fog.W + T.Fog.W;
		Temp.U     = U     + T.U;
		Temp.V     = V     + T.V;
		return Temp;
	}
	FTransTexture operator-( const FTransTexture& T ) const
	{
		FTransTexture Temp;
		Temp.Point = Point - T.Point;
		Temp.Light = Light - T.Light;
		Temp.Fog.X = Fog.X - T.Fog.X;
		Temp.Fog.Y = Fog.Y - T.Fog.Y;
		Temp.Fog.Z = Fog.Z - T.Fog.Z;
		Temp.Fog.W = Fog.W - T.Fog.W;
		Temp.U     = U     - T.U;
		Temp.V     = V     - T.V;
		return Temp;
	}
	FTransTexture operator*( FLOAT Scale ) const
	{
		FTransTexture Temp;
		Temp.Point = Point * Scale;
		Temp.Light = Light * Scale;
		Temp.Fog.X = Fog.X * Scale;
		Temp.Fog.Y = Fog.Y * Scale;
		Temp.Fog.Z = Fog.Z * Scale;
		Temp.Fog.W = Fog.W * Scale;
		Temp.U     = U     * Scale;
		Temp.V     = V     * Scale;
		return Temp;
	}
};

/*------------------------------------------------------------------------------------
	FSurfaceInfo.
------------------------------------------------------------------------------------*/

//
// Description of a renderable surface.
//
struct FSurfaceInfo
{
	DWORD			PolyFlags;		// Surface flags.
	FColor			FlatColor;		// Flat-shaded color.
	ULevel*			Level;			// Level to render.
	FTextureInfo*	Texture;		// Regular texture mapping info, if any.
	FTextureInfo*	LightMap;		// Light map, if any.
	FTextureInfo*	MacroTexture;	// Macrotexture, if any.
	FTextureInfo*	DetailTexture;	// Detail map, if any.
	FTextureInfo*	FogMap;			// Fog map, if any.
	FTextureInfo*	BumpMap;		// Bump map, if any.
	FTextureInfo*	EnvironmentMap;	// Environment map, if any.
	FTextureInfo*	HeightMap;	    // HeightMap map, if any.
	FPlane			SurfNormal;		// Surface Normals (maybe for hardware lighting) - new for 227j
};

//
// A saved polygon.
//
struct FSavedPoly
{
	FSavedPoly* Next;
	INT			iNode;
	void*       User;
	INT         NumPts;
	FTransform* Pts[ZEROARRAY];
};

//
// Description of a surface facet, represented as either
// a convex polygon or a concave span buffer.
//
struct FSurfaceFacet
{
	FCoords			MapCoords;		// Mapping coordinates.
	FCoords			MapUncoords;	// Inverse mapping coordinates.
	FSpanBuffer*	Span;			// Span buffer, if rendering device wants it.
	FSavedPoly*		Polys;			// Polygon list.
};

/*------------------------------------------------------------------------------------
	FScreenBounds.
------------------------------------------------------------------------------------*/

//
// Screen extents of an axis-aligned bounding box.
//
struct ENGINE_API FScreenBounds
{
	FLOAT MinX, MinY;
	FLOAT MaxX, MaxY;
	FLOAT MinZ;
};

/*------------------------------------------------------------------------------------
	FLightManagerBase.
------------------------------------------------------------------------------------*/

//
// Class encapsulating the dynamic lighting subsystem.
//
class FLightManagerBase
{
public:
	virtual ~FLightManagerBase() noexcept(false) {}

	virtual void SetupSurfsLightmaps(FSceneNode* Frame, UBOOL bForceUnlit) _VF_BASE;
	virtual void SetupSurfsFogmaps(FSceneNode* Frame) _VF_BASE;
	virtual void FinishActor() _VF_BASE;
	virtual DWORD SetupForActor(FSceneNode* Frame, struct FDynamicSprite* Actor, FFogSurf* DistFog, UBOOL bDrawSelected) _VF_BASE_RET(0);
	virtual DWORD Light(FTransSample& Point, DWORD ExtraFlags) _VF_BASE;
	virtual FPlane SpriteLight(const FVector& Point, DWORD ExtraFlags) _VF_BASE_RET(FPlane());
	virtual void SetRenderColor(const FVector& RenderColor, const FVector& UnlitColor) _VF_BASE;
	virtual void SetCustomLighting(UBOOL bDisableEngineLights = FALSE, const FVector* AmbientLight = NULL) _VF_BASE;
	virtual AActor* AddCustomLightSource(const FVector& Location, const FRotator& Rotation, UBOOL bDefault = FALSE) _VF_BASE_RET(NULL);
	virtual void ClearCustomLightSources() _VF_BASE;
	virtual FTextureInfo* BuildStaticMeshLightmap(class UStaticMesh* Mesh, class UStaticLightData* LightData) _VF_BASE_RET(NULL);

	// Push and pop a light frame.
	virtual void PushLightFrame(FSceneNode* Frame) _VF_BASE;
	virtual void PopLightFrame() _VF_BASE;

	// Unused atm!
	//virtual void GetLightData(struct FLightInfo*& LightStart, struct FLightInfo*& LightEnd) {}
	//virtual void GetFogData(struct FLightInfo**& FogStart, struct FLightInfo**& FogEnd) {}

	// For gc
	virtual void Serialize(FArchive& Ar) _VF_BASE;
};

/*------------------------------------------------------------------------------------
	URenderBase.
------------------------------------------------------------------------------------*/

//
// Line drawing flags.
//
enum ELineFlags
{
	LINE_None,
	LINE_Transparent, // Transparent/dotted line.
	LINE_DepthCued, // Honors Z-Ordering.
	LINE_NoPointConvulse, // Don't try to convert into a Point and always render as a line. Used for meshes.
	LINE_Legend // Used for if line is used for inscription, such as UED compass
};

//
// Pure virtual base class of the rendering subsytem.
//
class ENGINE_API URenderBase : public USubsystem
{
	DECLARE_ABSTRACT_CLASS(URenderBase,USubsystem,CLASS_Transient|CLASS_Config,Engine)
	NO_DEFAULT_CONSTRUCTOR(URenderBase);

	// Variables.
	UEngine* Engine;

	// Init/exit functions.
	virtual void Init( UEngine* InEngine ) {Engine=InEngine;}
	virtual UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar=*GLog ) _VF_BASE_RET(0);

	// Prerender/postrender functions.
	virtual void PreRender( FSceneNode* Frame ) _VF_BASE;
	virtual void PostRender( FSceneNode* Frame ) _VF_BASE;

	// Scene frame management.
	virtual FSceneNode* CreateMasterFrame( UViewport* Viewport, FVector Location, FRotator Rotation, FScreenBounds* Bounds ) _VF_BASE_RET(NULL);
	virtual FSceneNode* CreateChildFrame( FSceneNode* Parent, FSpanBuffer* Span, ULevel* Level, INT iSurf, INT iZone, FLOAT Mirror, const FPlane& NearClip, const FCoords& Coords, FScreenBounds* Bounds, INT iClipNode=INDEX_NONE ) _VF_BASE_RET(NULL);
	virtual void FinishMasterFrame() _VF_BASE;
	virtual UBOOL PushRenderToTexture(FRenderToTexture& Tex) _VF_BASE_RET(0);
	virtual void PopRenderToTexture(FRenderToTexture& Tex) _VF_BASE;

	// Major rendering functions.
	virtual void DrawWorld( FSceneNode* Frame ) _VF_BASE;
	virtual void DrawActor( FSceneNode* Frame, AActor* Actor, FDynamicSprite* Sprite=NULL ) _VF_BASE;

	// Other functions.
	virtual UBOOL Project( FSceneNode* Frame, const FVector &V, FLOAT &ScreenX, FLOAT &ScreenY, FLOAT* Scale ) _VF_BASE_RET(0);
	virtual UBOOL Deproject( FSceneNode* Frame, INT ScreenX, INT ScreenY, FVector& V ) _VF_BASE_RET(0);
	virtual UBOOL BoundVisible( FSceneNode* Frame, FBox* Bound, FSpanBuffer* SpanBuffer, FScreenBounds& Results ) _VF_BASE_RET(0);
	virtual void GetVisibleSurfs( UViewport* Viewport, TArray<INT>& iSurfs, FLOAT MaxDistance ) _VF_BASE;
	virtual void GlobalLighting( UBOOL Realtime, AActor* Owner, FLOAT& Brightness, FPlane& Color, UBOOL UseColor=0 ) _VF_BASE;
	virtual void Precache( UViewport* Viewport ) _VF_BASE;
	virtual void NoteDestroyed( AActor* Other ) _VF_BASE;

	// High level primitive drawing.
	virtual void DrawCircle( FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector& Location, FLOAT Radius, UBOOL bScaleRadiusByZoom = 0 ) _VF_BASE;
	virtual void DrawBox( FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector Min, FVector Max ) _VF_BASE;
	virtual void DrawCylinder( FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector& Location, FLOAT Radius, FLOAT Height ) _VF_BASE;
	virtual void DrawCube( FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector Location, FVector Size, FRotator xRotation = FRotator(0,0,0)) _VF_BASE;
	virtual void DrawSphere( FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector Location, FLOAT r, INT n) _VF_BASE;
};

/*-----------------------------------------------------------------------------
	Hit proxies.
-----------------------------------------------------------------------------*/

// Hit a Bsp surface.
struct ENGINE_API HBspSurf : public HHitProxy
{
	DECLARE_HIT_PROXY(HBspSurf,HHitProxy)
	INT iSurf;
	HBspSurf( INT iInSurf ) : iSurf( iInSurf ) {}
};

// Hit an actor.
struct ENGINE_API HActor : public HHitProxy
{
	DECLARE_HIT_PROXY(HActor,HHitProxy)
	AActor* Actor;
	HActor( AActor* InActor ) : Actor( InActor ) {}
};

// Hit ray descriptor.
struct ENGINE_API HCoords : public HHitProxy
{
	DECLARE_HIT_PROXY(HCoords,HHitProxy)
	FCoords Coords, Uncoords;
	FVector Direction;
	HCoords( FSceneNode* InFrame )
	:	Coords  ( InFrame->Coords   )
	,	Uncoords( InFrame->Uncoords )
	{
		FLOAT X = InFrame->Viewport->HitX+InFrame->Viewport->HitXL/2;
		FLOAT Y = InFrame->Viewport->HitY+InFrame->Viewport->HitYL/2;
		Direction
		=	InFrame->Coords.ZAxis
		+	InFrame->Coords.XAxis * (X - InFrame->FX2) * InFrame->RProj.Z
		+	InFrame->Coords.YAxis * (Y - InFrame->FY2) * InFrame->RProj.Z;
	}
};

/*------------------------------------------------------------------------------------
	RenderIterator support.
------------------------------------------------------------------------------------*/

struct FActorBuffer
{
	BYTE Padding[sizeof(AActor)];

	inline AActor* Actor()
	{
		return (AActor*)this;
	}
};

/*------------------------------------------------------------------------------------
	Debug lines.
------------------------------------------------------------------------------------*/

struct ENGINE_API FDebugLineData
{
	struct FLinePair
	{
		FVector A, B;

		FLinePair(){}
		FLinePair(const FVector& InA, const FVector& InB)
			: A(InA), B(InB)
		{}
		inline void Set(const FVector& InA, const FVector& InB)
		{
			A = InA;
			B = InB;
		}
	};

	~FDebugLineData();

	static void DrawLine(const FLinePair& Line, const FPlane& Col, UBOOL bStay = FALSE);
	static void DrawLineList(FLinePair* Lines, const INT Count, const FPlane& Col, UBOOL bStay = FALSE);
	static void DrawLineArray(const TArray<FLinePair>& Lines, const FPlane& Col, UBOOL bStay = FALSE);
	static void DrawLineExtent(const FVector& Start, const FVector& End, const FVector& Extent, const FPlane& Col, UBOOL bStay = FALSE);
	static void DrawBox(const FVector& Start, const FVector& End, const FPlane& Col, UBOOL bStay = FALSE);
	static void DrawTransformedBox(const FVector& A, const FVector& B, const FCoords& Coords, const FPlane& Col, UBOOL bStay = FALSE);
	static void DrawSphere(const FVector& Point, FLOAT Radius, BYTE Segments, const FPlane& Col, UBOOL bStay = FALSE);

	static void Draw(FSceneNode* Frame);
	static void Clear();

private:
	static FDebugLineData*	GFirstLine;
	FDebugLineData*			NextLine;
	FLinePair*				Points;
	INT						NumPoints;
	FPlane					LineColor;
	UBOOL					bShouldStay; // Don't get deleted after first tick.

	FDebugLineData() = delete;
	FDebugLineData(const FLinePair& Line, const FPlane& Col, UBOOL bStay);
	FDebugLineData(FLinePair* Pts, INT ListSize, const FPlane& Col, UBOOL bStay);
};

/*------------------------------------------------------------------------------------
	Engine lighting.
------------------------------------------------------------------------------------*/

#define MAX_LIGHT_SOURCES 32
struct FLightVisibilityCache
{
	struct FInfo
	{
		TSafePointer<AActor> Actor;
		DWORD Factor;
	};
	struct FLightData
	{
		AActor* Actor;			// Light actor owning this source.
		FLightData* Next;		// Next on this local render list.
		FLightData* NextVol;	// Next volumetric light on this render list.
		FPlane FloatColor;		// Light color.
		FLOAT Brightness,		// Actual render brightness.
			LightBrightness,	// Light brightness scale (0-1).
			Radius,				// Light radius in UU.
			RRadius;			// 1.f / Radius
		BYTE IsVolumetric;		// Whether it's volumetric.

		// For volumetric lights.
		FLOAT VolRadius;			// Volumetric radius.
		FLOAT VolRadiusSquared;		// VolRadius*VolRadius.
		FLOAT VolBrightness;		// Volumetric lighting brightness.
		FLOAT LocationSizeSquared;	// Location.SizeSqurated().
		FLOAT RVolRadius;			// 1/Volumetric radius.
		FLOAT RVolRadiusSquared;	// 1/VolRadius*VolRadius.
		FPlane VolumetricColor;		// Volumetric lighting color.

		FLightData()
			:Actor(NULL), Next(NULL), NextVol(NULL)
		{}
		virtual ~FLightData(){}
	};

	FInfo* Sources;				// Cached light list.
	FLightData* LightInfo;		// Cached render time light info.
	FVector AmbLight;
	BYTE NumSources;
	UINT UpdateFrame, LastTeleportTag;

	inline FInfo* GetSources()
	{
		if (!Sources)
		{
			STAT(GStatRender.LightMemActor.Value += (sizeof(FInfo) * MAX_LIGHT_SOURCES));
			Sources = new FInfo[MAX_LIGHT_SOURCES];
		}
		return Sources;
	}

	FLightVisibilityCache()
		: Sources(NULL), LightInfo(NULL), AmbLight(0.f,0.f,0.f), NumSources(0), UpdateFrame(~GFrameNumber), LastTeleportTag(0)
	{
		STAT(GStatRender.LightMemActor.Value += sizeof(FLightVisibilityCache));
	}
	~FLightVisibilityCache()
	{
		STAT(GStatRender.LightMemActor.Value -= sizeof(FLightVisibilityCache));
		if (Sources)
		{
			STAT(GStatRender.LightMemActor.Value -= (sizeof(FInfo) * MAX_LIGHT_SOURCES));
			delete[] Sources;
		}
		if (LightInfo)
			delete LightInfo;
	}
};

/*-----------------------------------------------------------------------------
	URenderIterator inlines.
-----------------------------------------------------------------------------*/

inline AActor* URenderIterator::OnGetActors(FSceneNode* Camera)
{
	guardSlow(URenderIterator::OnGetActors);
	if (bHurtEntry)
		return NULL;
	Frame = Camera;
	Observer = Camera->Viewport->Actor;
	bHurtEntry = TRUE;
	AActor* Result = GetActors();
	bHurtEntry = FALSE;
	Frame = NULL;
	return Result;
	unguardSlow;
}

/*------------------------------------------------------------------------------------
	StaticMesh Buffering.
------------------------------------------------------------------------------------*/
/*
struct ENGINE_API FMeshBufferData
{
	UMesh* Mesh;
	bool IsBuffered;

	FMeshBufferData( UMesh* M );
	virtual ~FMeshBufferData();

	virtual void BeginBuffer( INT Polygons, INT Vertices, INT Index ) _VF_BASE;
	virtual void EndBuffer() _VF_BASE;
	virtual void BufferPolygon( FTransTexture** Pts ) _VF_BASE;
	virtual void* GetData( INT Index ) _VF_BASE;
};
*/
#endif
/*------------------------------------------------------------------------------------
	The End.
------------------------------------------------------------------------------------*/
