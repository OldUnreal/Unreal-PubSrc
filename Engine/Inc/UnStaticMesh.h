/*===============================================================================
	UnStaticMesh.h: Unreal static mesh support.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Marco
===============================================================================*/

#define StaticMeshVersionNum 3
static INT SMSerializeVer=0;

enum EStaticMeshFlags // Own flags definitions for static meshes so they fit in two bytes.
{
	SMFL_Masked			= 1 << 0,
	SMFL_Translucent	= 1 << 1,
	SMFL_AlphaBlend		= 1 << 2,
	SMFL_Modulated		= 1 << 3,
	SMFL_MeshEnvMap		= 1 << 4,
	SMFL_Invisible		= 1 << 5,
	SMFL_NoCollision	= 1 << 6,
	SMFL_TwoSided		= 1 << 7,
	SMFL_Unlit			= 1 << 8
};
struct FStaticMeshTri
{
	_WORD			iVertex[3];		// Vertex indices.
	FMeshFloatUV	Tex[3];			// Texture UV coordinates.
	BYTE			GroupIndex;		// Surface flags.
	FVector			EdgeNormals[3];	// Vert normals (for lighting)
	FMeshFloatUV*	UV2[3];			// Initialized UV2 map for render.

	friend FArchive &operator<<( FArchive& Ar, FStaticMeshTri& T )
	{
		Ar << T.iVertex[0] << T.iVertex[1] << T.iVertex[2];
		Ar << T.Tex[0] << T.Tex[1] << T.Tex[2] << T.GroupIndex;
		return Ar;
	}
	inline void FlipNormal()
	{
		Exchange(iVertex[0], iVertex[2]);
		Exchange(Tex[0], Tex[2]);
		if (UV2[0])
		Exchange(*UV2[0], *UV2[2]);
	}
};
struct FStaticMeshTexGroup
{
	BITFIELD		PolyFlags;		// Surface flags.
	BITFIELD		OldPolyFlags;
	DWORD			RealPolyFlags;	// Setup in game on render.
	INT				Texture;		// Texture index
	INT			SmoothGroup,OldSmoothGroup; // Smoothing group.
	friend FArchive &operator<<( FArchive& Ar, FStaticMeshTexGroup& TexGroup )
	{
		_WORD PolyF = 0x0000FFFF & TexGroup.PolyFlags;
		Ar << PolyF << TexGroup.Texture;
		if( Ar.IsLoading() )
		{
			if( SMSerializeVer==1 )
				TexGroup.SmoothGroup = 0;
			else Ar << TexGroup.SmoothGroup;
			TexGroup.PolyFlags = PolyF;
			TexGroup.SetupPolyFlags();
		}
		else Ar << TexGroup.SmoothGroup;
		return Ar;
	}
	void SetupPolyFlags()
	{
		OldPolyFlags = PolyFlags; // Save for editor use.
		OldSmoothGroup = SmoothGroup;

		// Convert save polyflags to render polyflags.
		RealPolyFlags = ((PolyFlags & SMFL_Masked) ? PF_Masked : 0) | ((PolyFlags & SMFL_Translucent) ? PF_Translucent : 0)
			 | ((PolyFlags & SMFL_AlphaBlend) ? PF_AlphaBlend : 0) | ((PolyFlags & SMFL_Modulated) ? PF_Modulated : 0)
			/* | ((PolyFlags & SMFL_Flat) ? PF_Flat : 0)*/ | ((PolyFlags & SMFL_MeshEnvMap) ? PF_Environment : 0)
			 | ((PolyFlags & SMFL_Invisible) ? PF_Invisible : 0) | ((PolyFlags & SMFL_TwoSided) ? PF_TwoSided : 0)
			 | ((PolyFlags & SMFL_Unlit) ? PF_Unlit : 0);
	}
	inline bool CheckForUpdates()
	{
		if( OldPolyFlags!=PolyFlags )
		{
			UBOOL bHadCollision = (OldPolyFlags & SMFL_NoCollision);
			SetupPolyFlags();
			return (bHadCollision!=((PolyFlags & SMFL_NoCollision) ? 1 : 0) ? true : false);
		}
		else if( OldSmoothGroup!=SmoothGroup )
		{
			OldSmoothGroup = SmoothGroup;
			return true;
		}
		return false;
	}
};
struct FStaticMeshLOD
{
	FLOAT Bias;
	class UStaticMesh* Mesh;
	friend FArchive& operator<<(FArchive& Ar, FStaticMeshLOD& T);
};
struct FVertexNormalPair
{
	FVector Vertex;
	INT Normal;
};

class ENGINE_API UStaticMesh : public UMesh
{
	DECLARE_CLASS(UStaticMesh,UMesh,0,Engine)

	enum EStaticMeshUV2Mode : BYTE
	{
		UV2_TextureOverlay,
		UV2_StaticLightMap,
		UV2_DynamicLightMap,
	};

	TLazyArray<FVector>				SMVerts;
	TLazyArray<FStaticMeshTri>		SMTris;
	TArray<FStaticMeshTexGroup>		SMGroups;
	TArray<FStaticMeshLOD>			SMLodLevels;
	TArray<FVector>					SMNormals; // Calc on render frame.
	TLazyArray<FMeshFloatUV>		SMUV2Map;
	UTexture*						UV2MapTexture;

	INT								MeshVersion; // Mesh Version this was saved on.
	INT								LightmapDetail; // Lightmap detail resolution.
	UBOOL							bUseSimpleLineCheck,bUseSimpleBoxLineCheck,bHasUV2Map,bUV2MapInit,bUV2PreLightmap;
	BYTE							UV2Mode, UV2Style;

	TArray<FVertexNormalPair>		LightmapVerts; // Computed lightmap positioning.

	// Object interface
	UStaticMesh();
	void Serialize( FArchive& Ar );
	void StaticConstructor();
	void onPropertyChange(UProperty* Property, UProperty* OuterProperty);

	// Primitive interface
	FBox GetRenderBoundingBox( const AActor* Owner, UBOOL Exact );
	FBox GetCollisionBoundingBox(const AActor* Owner);
	FSphere GetRenderBoundingSphere( const AActor* Owner, UBOOL Exact );
	UBOOL PointCheck( FCheckResult &Result, AActor *Owner, FVector Location, FVector Extent, DWORD ExtraNodeFlags );
	UBOOL LineCheck( FCheckResult &Result, AActor *Owner, FVector End, FVector Start, FVector Extent, DWORD ExtraNodeFlags );
	BYTE CheckLightrayBlock( const FVector& Pos, const FVector** Clips, const FVector** Pts, INT iSurf, BYTE bBacksideHit, AActor* Owner );
	PX_MeshShape* GetConvexVolume();
	PX_MeshShape* GetTriangleMesh();

	// Mesh interface
	const FMeshAnimSeq* GetAnimSeq( FName SeqName ) const;
	FMeshAnimSeq* GetAnimSeq( FName SeqName );
	void GetFrame( FVector* Verts, INT Size, FCoords Coords, AActor* Owner, INT* LODRequest = NULL);
	void SetupCollision();
	void MapAnimations() {}
	FMeshInstanceBase* GetInstance(AActor* Owner) { return NULL; }
	EActorRenderPass InitMeshRenderPass();
	FFlatShadedModel* GetFlatModel(AActor* Owner) { return NULL; }

	// Static mesh interface
	inline UStaticMesh* GetDisplayMesh(FLOAT Z)
	{
		if (SMLodLevels.Num() == 1)
			return (SMLodLevels(0).Bias > Z && SMLodLevels(0).Mesh) ? SMLodLevels(0).Mesh : this;
		if (SMLodLevels(0).Bias < Z)
			return this; // None LOD drop.
		INT i = 1;
		for (; i < SMLodLevels.Num(); ++i)
			if (SMLodLevels(i).Bias < Z)
				return (SMLodLevels(i - 1).Mesh ? SMLodLevels(i - 1).Mesh : this);
		i = SMLodLevels.Num() - 1;
		return (SMLodLevels(i).Mesh ? SMLodLevels(i).Mesh : this);
	}
	void CalcSMNormals();
	void CalcSMBounds();
	void InitUV2Mapping();
	void InitLightmapVerts();

	// Editor support.
	void MakeMeshOfBrush( const TArray<AActor*>& BRA, INT Cuts );
	void SetupConnects();
	void SmoothCorners( bool bNoMessage=false );
	void GetAnimationInfo(FSceneNode* Frame, AActor* Owner, FName SeqName, DWORD DrawFlags, TCHAR* Info);

	inline UBOOL UseUV2Lightmap() const
	{
		return (bHasUV2Map && UV2Mode >= UV2_StaticLightMap);
	}
};
