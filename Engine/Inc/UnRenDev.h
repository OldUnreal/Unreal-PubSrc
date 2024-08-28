/*=============================================================================
	UnRenDev.h: 3D rendering device class.

	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _UNRENDEV_H_
#define _UNRENDEV_H_

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

/*------------------------------------------------------------------------------------
	URenderDevice.
------------------------------------------------------------------------------------*/

// Flags for locking a rendering device.
enum ELockRenderFlags
{
	LOCKR_ClearScreen	    = 1,
	LOCKR_LightDiminish     = 2,
};
enum EDescriptionFlags
{
	RDDESCF_Certified       = 1,
	RDDESCF_Incompatible    = 2,
	RDDESCF_LowDetailWorld  = 4,
	RDDESCF_LowDetailSkins  = 8,
	RDDESCF_LowDetailActors = 16,
};

//
// A low-level 3D rendering device.
//
class ENGINE_API URenderDevice : public USubsystem
{
	DECLARE_ABSTRACT_CLASS(URenderDevice,USubsystem,CLASS_Config,Engine)

	// Variables.
	BYTE			DecompFormat;
	INT				RecommendedLOD;
    INT				MaxTextureSize;
	UViewport*		Viewport;
	FString			Description;
	DWORD			DescFlags;
	BITFIELD		SpanBased;
	BITFIELD		FullscreenOnly;
	BITFIELD		SupportsNoClipRender;
	BITFIELD		SupportsFogMaps;
	BITFIELD		SupportsDistanceFog;
	BITFIELD		LegacyRenderingOnly;
	BITFIELD		UseLightmapAtlas;
	BITFIELD		VolumetricLighting;
	BITFIELD		ShinySurfaces;
	BITFIELD		Coronas;
	BITFIELD		HighDetailActors;
	BITFIELD		SupportsTC; // Supports BTC1-3 (DXT1,3,5)
	BITFIELD		SupportsNewBTC; // Supports BTC4+
	BITFIELD		PrecacheOnFlip;
	BITFIELD		SupportsLazyTextures;
	BITFIELD		PrefersDeferredLoad;
	BITFIELD		DetailTextures, MeshDetailTextures;
	BITFIELD		SupportsHDLightmaps;
	BITFIELD		SupportsAlphaBlend;
	BITFIELD		NeedsMaskedFonts;
	DWORD			UnsupportHDLightFlags;

	// Constructors.
	void StaticConstructor();

	// URenderDevice low-level functions that drivers must implement.
	virtual void Placeholder() {}
	virtual UBOOL Init( UViewport* InViewport, INT NewX, INT NewY, INT NewColorBytes, UBOOL Fullscreen ) _VF_BASE_RET(0);
	virtual UBOOL SetRes( INT NewX, INT NewY, INT NewColorBytes, UBOOL Fullscreen ) _VF_BASE_RET(0);
	virtual UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar );

	virtual void Exit() _VF_BASE;
	virtual void Flush( UBOOL AllowPrecache ) _VF_BASE;
	virtual void Lock( FPlane FlashScale, FPlane FlashFog, FPlane ScreenClear, DWORD RenderLockFlags, BYTE* HitData, INT* HitSize ) _VF_BASE;
	virtual void Unlock( UBOOL Blit ) _VF_BASE;
	virtual void DrawComplexSurface( FSceneNode* Frame, FSurfaceInfo& Surface, FSurfaceFacet& Facet ) _VF_BASE;
	virtual void DrawGouraudPolygon( FSceneNode* Frame, FTextureInfo& Info, FTransTexture** Pts, int NumPts, DWORD PolyFlags, FSpanBuffer* Span );
	virtual void DrawTile( FSceneNode* Frame, FTextureInfo& Info, FLOAT X, FLOAT Y, FLOAT XL, FLOAT YL, FLOAT U, FLOAT V, FLOAT UL, FLOAT VL, class FSpanBuffer*Span, FLOAT Z, FPlane Color, FPlane Fog, DWORD PolyFlags ) _VF_BASE;
	virtual void Draw2DLine( FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector P1, FVector P2 ) _VF_BASE;
	virtual void Draw2DPoint( FSceneNode* Frame, FPlane Color, DWORD LineFlags, FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2, FLOAT Z ) _VF_BASE;
	virtual void ClearZ( FSceneNode* Frame ) _VF_BASE;
	virtual void PushHit( const BYTE* Data, INT Count ) _VF_BASE;
	virtual void PopHit( INT Count, UBOOL bForce ) _VF_BASE;
	virtual void GetStats( TCHAR* Result ) _VF_BASE;
	virtual void ReadPixels( FColor* Pixels ) _VF_BASE;

	virtual void Draw3DLine( FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector OrigP, FVector OrigQ );
	virtual void Draw2DClippedLine( FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector P1, FVector P2 );
	virtual void EndFlash() {}
	virtual void DrawStats( FSceneNode* Frame ){}
	virtual void SetSceneNode( FSceneNode* Frame ){}
	virtual void PrecacheTexture( FTextureInfo& Info, DWORD PolyFlags ){}
	virtual void PreDrawGouraud(FSceneNode* Frame, FFogSurf &FogSurf){}
	virtual void PostDrawGouraud(FSceneNode* Frame, FFogSurf &FogSurf){}
	virtual void DrawPass(FSceneNode* Frame, INT Pass){}
	virtual void ResetGamma() {}

	// 227g: No Clipping render
	virtual void BeginMeshDraw(FSceneNode* Frame, AActor* Owner) {}
	virtual void DrawGouraudPolyList(FSceneNode* Frame, FTextureInfo& Info, FTransTexture* Pts, int NumPts, DWORD PolyFlags, FSpanBuffer* Span=NULL);
	virtual void EndMeshDraw(FSceneNode* Frame, AActor* Owner) {}

	// 227j clipping planes (return 1 to let hardware handle it, 2 to indicate hardware overflow).
	virtual BYTE PushClipPlane(const FPlane& Plane) { return 0; }
	virtual BYTE PopClipPlane() { return 0; }

	// Change Z-testing mode while returning previous state.
	virtual BYTE SetZTestMode(BYTE Mode)
	{
		return ZTEST_LessEqual;
	}

	// 227k: Render to texture states.
	virtual UBOOL PushRenderToTexture(struct FRenderToTexture* Tex) { return FALSE; }
	virtual void PopRenderToTexture() {}

	// 227i buffered StaticMesh rendering
	/*
	virtual FMeshBufferData* FindMeshBuffer( UMesh* Mesh ) _VF_BASE_RET(NULL);
	virtual void InitMeshBuffer( FMeshBufferData& B ) {}
	virtual void ExitMeshBuffer( FMeshBufferData& B ) {}
	virtual void PrepareBufferedMesh( AActor* Actor, const FCoords& Coords, bool bWireframe ) {}
	virtual void EndBufferedMesh() {}
	virtual void DrawMeshBuffer( FMeshBufferData& B, INT Num, UTexture* Tex, DWORD Flags ) {}
	virtual void DrawMeshWireframe( FMeshBufferData& B, INT Num, FPlane Color ) {}
	*/
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

#endif
/*------------------------------------------------------------------------------------
	The End.
------------------------------------------------------------------------------------*/
