/*=============================================================================
	UnMesh.h: Unreal mesh objects.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

/*-----------------------------------------------------------------------------
	U3D Collision data
-----------------------------------------------------------------------------*/

struct FRealCollision
{
	FCollisionModel* Collision;
	FBox CollisionBounds;

	FRealCollision()
		: Collision(NULL)
	{}
	~FRealCollision()
	{
		if( Collision )
		{
			delete Collision;
			Collision = NULL;
		}
	}
};
/*-----------------------------------------------------------------------------
	FMeshVert.
-----------------------------------------------------------------------------*/

// Packed mesh vertex point for skinned meshes.
#define GET_MESHVERT_DWORD(mv) (*(DWORD*)&(mv))
struct FMeshVert
{
	// Variables.
/* !!! WRONG!  --ryan.
#if __INTEL_BYTE_ORDER__
	INT X:11; INT Y:11; INT Z:10;
#else
	INT Z:10; INT Y:11; INT X:11;
#endif
*/

	INT X:11; INT Y:11; INT Z:10;

	// Constructor.
	FMeshVert()
	{}
	FMeshVert( const FVector& In )
	: X(appRound(In.X)), Y(appRound(In.Y)), Z(appRound(In.Z))
	{}

	// Functions.
	FVector Vector() const
	{
		return FVector( X, Y, Z );
	}

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, FMeshVert& V )
	{
		#if __INTEL_BYTE_ORDER__
		return Ar << GET_MESHVERT_DWORD(V);
		#else
		DWORD val = (V.X & 0x7FF) | ((V.Y & 0x7FF) << 11) | ((V.Z & 0x3FF) << 22);
		FArchive &retval = Ar << val;
		V.X = (val >> 00) & 0x7FF;
		V.Y = (val >> 11) & 0x7FF;
		V.Z = (val >> 22) & 0x3FF;
		return retval;
		#endif

	}
};

/*-----------------------------------------------------------------------------
	FMeshTri.
-----------------------------------------------------------------------------*/

// Texture coordinates associated with a vertex and one or more mesh triangles.
// All triangles sharing a vertex do not necessarily have the same texture
// coordinates at the vertex.
struct FMeshUV
{
	BYTE U;
	BYTE V;

	inline void Set(BYTE InU, BYTE InV)
	{
		U = InU;
		V = InV;
	}
	friend FArchive &operator<<( FArchive& Ar, FMeshUV& M )
		{return Ar << M.U << M.V;}
};

struct FMeshFloatUV
{
	FLOAT U;
	FLOAT V;

	FMeshFloatUV()
	{}
	FMeshFloatUV(FLOAT InU, FLOAT InV)
		: U(InU), V(InV)
	{}
	FMeshFloatUV(const FMeshUV& UV)
		: U(UV.U/256.f), V(UV.V/256.f)
	{}
	inline void Set(FLOAT InU, FLOAT InV)
	{
		U = InU;
		V = InV;
	}
	inline void Lerp(const FMeshFloatUV& Target, FLOAT Alpha)
	{
		U += ((Target.U - U) * Alpha);
		V += ((Target.V - V) * Alpha);
	}
	inline FVector2D GetVector() const
	{
		return FVector2D(U, V);
	}

	friend FArchive &operator<<( FArchive& Ar, FMeshFloatUV& M )
		{return Ar << M.U << M.V;}

	inline FMeshFloatUV& operator=(const FMeshFloatUV& Other)
	{
		*((QWORD*)this) = *((QWORD*)&Other);
		return *this;
	}
	inline UBOOL operator==(const FMeshFloatUV& Other)
	{
		return *((QWORD*)this) == *((QWORD*)&Other);
	}
	inline UBOOL operator!=(const FMeshFloatUV& Other)
	{
		return *((QWORD*)this) != *((QWORD*)&Other);
	}
};

// One triangular polygon in a mesh, which references three vertices,
// and various drawing/texturing information.
struct FMeshTri
{
	_WORD		iVertex[3];		// Vertex indices.
	FMeshUV		Tex[3];			// Texture UV coordinates.
	DWORD		PolyFlags;		// Surface flags.
	INT			TextureIndex;	// Source texture index.
	friend FArchive &operator<<( FArchive& Ar, FMeshTri& T )
	{
		Ar << T.iVertex[0] << T.iVertex[1] << T.iVertex[2];
		Ar << T.Tex[0] << T.Tex[1] << T.Tex[2];
		Ar << T.PolyFlags << T.TextureIndex;
		return Ar;
	}
};

/*-----------------------------------------------------------------------------
	LOD Mesh structs.
-----------------------------------------------------------------------------*/

// Compact structure to send processing info to mesh digestion at import time.
struct ULODProcessInfo
{
	UBOOL LevelOfDetail;
	UBOOL NoUVData;
	UBOOL OldAnimFormat;
	UBOOL bExtendedUV;
	INT   SampleFrame;
	INT   MinVerts;
	INT   Style;
};

// LOD-style Textured vertex struct. references one vertex, and
// contains texture U,V information. 4 bytes.
// One triangular polygon in a mesh, which references three vertices,
// and various drawing/texturing information.
struct FMeshWedge
{
	_WORD			iVertex;	// Vertex index.
	FMeshUV			TexUV;		// Texture UV coordinates. ( 2 bytes)
	FMeshFloatUV	ExtTexUV;	// 227j: Run-Time texture coordinates.

	FMeshWedge()
	{}
	friend FArchive &operator<<( FArchive& Ar, FMeshWedge& T )
	{
		Ar << T.iVertex << T.TexUV;
		if(Ar.IsLoading())
			T.ExtTexUV = T.TexUV;
		return Ar;
	}
	inline FMeshWedge& operator=(const FMeshWedge& Other)
	{
		guardSlow(FMeshWedge::operator=);
		iVertex = Other.iVertex;
		*((QWORD*)&ExtTexUV) = *((QWORD*)&Other.ExtTexUV);
		return *this;
		unguardSlow;
	}

	// Render tricks with morphing.
	inline UBOOL IsUninitialized() const
	{
		return (TexUV.U == 0);
	}
	inline void Initialize()
	{
		TexUV.U = 1;
	}
};

// Extended wedge - floating-point UV coordinates.
struct FMeshExtWedge
{
	_WORD		    iVertex;		// Vertex index.
	_WORD           Flags;          // Reserved 16 bits of flags
	FMeshFloatUV	TexUV;			// Texture UV coordinates. ( 2 DWORDS)
	friend FArchive& operator<<(FArchive& Ar, FMeshExtWedge& T)
	{
		Ar << T.iVertex << T.Flags << T.TexUV;
		return Ar;
	}
};

// LOD-style triangular polygon in a mesh, which references three textured vertices.  8 bytes.
struct FMeshFace
{
	_WORD		iWedge[3];		// Textured Vertex indices.
	_WORD		MaterialIndex;	// Source Material (= texture plus unique flags) index.

	friend FArchive &operator<<( FArchive& Ar, FMeshFace& F )
	{
		Ar << F.iWedge[0] << F.iWedge[1] << F.iWedge[2];
		Ar << F.MaterialIndex;
		return Ar;
	}

	FMeshFace& operator=( const FMeshFace& Other )
	{
		guardSlow(FMeshFace::operator=);
		this->iWedge[0] = Other.iWedge[0];
		this->iWedge[1] = Other.iWedge[1];
		this->iWedge[2] = Other.iWedge[2];
		this->MaterialIndex = Other.MaterialIndex;
		return *this;
		unguardSlow;
	}
};

// LOD-style mesh material.
struct FMeshMaterial
{
	DWORD		PolyFlags;		// Surface flags.
	INT			TextureIndex;	// Source texture index.
	friend FArchive &operator<<( FArchive& Ar, FMeshMaterial& M )
	{
		return Ar << M.PolyFlags << M.TextureIndex;
	}
};

struct FEdPreviewSocket
{
	class UMesh* Mesh;
	FVector Offset, Scale;
	FRotator RotOffset;
	FName BoneName;
};

/*-----------------------------------------------------------------------------
	FMeshVertConnect.
-----------------------------------------------------------------------------*/

// Says which triangles a particular mesh vertex is associated with.
// Precomputed so that mesh triangles can be shaded with Gouraud-style
// shared, interpolated normal shading.
struct FMeshVertConnect
{
	INT	NumVertTriangles;
	INT	TriangleListOffset;
	friend FArchive &operator<<( FArchive& Ar, FMeshVertConnect& C )
		{return Ar << C.NumVertTriangles << C.TriangleListOffset;}
};

/*-----------------------------------------------------------------------------
	Index/Vertex buffers for rendering.
-----------------------------------------------------------------------------*/

struct FPolyIndices
{
	_WORD Poly[3];
};
struct FMeshIndexBuffer
{
	FPolyIndices* IndexList;

	FMeshIndexBuffer(INT Size)
		: IndexList(new FPolyIndices[Size])
	{}
	~FMeshIndexBuffer()
	{
		delete[] IndexList;
	}
	inline _WORD* GetIndices()
	{
		return (_WORD*)&IndexList;
	}
};

struct FVertexBufferVertex
{
	FVector* Position,* Normal;
	FLOAT U, V;
	FPlane* Light,* Fog;
};
struct FVertexBuffer
{
	class UMesh* Mesh;
	FVertexBufferVertex* VertList;
	_WORD TexIndex;
	DWORD PolyFlags,VertCount;

	FVertexBuffer(UMesh* M, _WORD Tex, DWORD PF)
		: Mesh(M), VertList(NULL), TexIndex(Tex), PolyFlags(PF), VertCount(0)
	{}
	~FVertexBuffer()
	{
		if(VertList)
			delete[] VertList;
	}
};
struct FVertexList
{
	TArray<FVertexBuffer> PolyLists;

	void Sort();
};

struct FRenderSurface
{
	DWORD iSurface;
	INT SortKey;
};

/*-----------------------------------------------------------------------------
	FMeshInstance/Base.
-----------------------------------------------------------------------------*/

struct FSceneNode;

enum EInstanceType
{
	MI_MeshInstance,
	MI_LodMeshInstance,
	MI_SkelMeshInstance,
	MI_StaticMeshInstance,
	MI_Custom,
	MI_MaxType=0xff,
};

struct ENGINE_API FMeshInstanceBase
{
protected:
	BYTE Type;
	class UMesh* Mesh;
	AActor* Actor;

public:
	UBOOL bCoordsDirty; // Actor has moved.
	FLOAT MirrorNormals;

	FMeshInstanceBase() = delete;
	FMeshInstanceBase(BYTE T, UMesh* M, AActor* A)
		: Type(T), Mesh(M), Actor(A), bCoordsDirty(1)
	{}
	virtual ~FMeshInstanceBase() noexcept(false)
	{}

	inline BYTE GetType()
	{
		return Type;
	}
	inline AActor* GetActor()
	{
		return Actor;
	}
	inline UMesh* GetMesh()
	{
		return Mesh;
	}
	inline BYTE Matches(UMesh* M, AActor* A)
	{
		return (Mesh == M && Actor == A);
	}
	virtual void Serialize(FArchive& Ar) {}
	virtual void SerializeGC(FArchive& Ar) {}
	virtual void PostLoad() {}
	virtual void UpdateFrame() {}

	// API
	virtual FVector GetBoneLocation(INT BoneIndex, UBOOL bOriginal = FALSE) = 0;
	virtual FCoords GetBoneCoords(INT BoneIndex, UBOOL bOriginal = FALSE) = 0;
	virtual void UpdateAttachments(UBOOL bOnlyHard) {}

	// Render
	virtual void GetFrame(FVector* Verts, FVector* Norms, INT Size, FCoords Coords, INT* LODRequest = NULL) {}
	virtual void Draw(FSceneNode* Frame) {}
	virtual void DrawParticles(FSceneNode* Frame, FSpanBuffer* Buffer=NULL);
	virtual void DrawWireframe(FSceneNode* Frame) {}
	virtual void DrawWireParticles(FSceneNode* Frame);
	virtual void DrawEdPreview(FSceneNode* Frame) {}
	virtual FPlane GetWireframeColor();

	static void GetVertAlloc(FVector** Verts, FVector** Norms, INT Size);
	static void GetLightAlloc(FPlane** Light, FPlane** Fog, INT Size);

	inline DWORD GetRenderFlags()
	{
		return (Actor->Style == STY_Translucent ? PF_Translucent : Actor->Style == STY_Masked ? PF_Masked : Actor->Style == STY_Modulated ? PF_Modulated : Actor->Style == STY_AlphaBlend ? PF_AlphaBlend : 0)
			| (Actor->bUnlit ? PF_Unlit : 0) | (Actor->bNoSmooth ? PF_NoSmooth : 0);
	}
};

struct ENGINE_API FMeshInstance : public FMeshInstanceBase
{
	FVector* Verts{}, * Norms{};
	FName Sequence, OldBlendAnim;
	FLOAT Frame, TweenStartTime;
	const FMeshAnimSeq* SeqRef{};
	struct FMeshAnimSeq* TweenSeq{};
	UBOOL bIsValid;
	DWORD LastUpdateFrame;
	FVector* CachePose{};

	FMeshInstance(UMesh* M, AActor* A);
	~FMeshInstance() noexcept(false)
	{
		guardSlow(FMeshInstance::~FMeshInstance);
		if (Verts)
		{
			appFree(Verts);
			appFree(Norms);
		}
		if (CachePose)
			appFree(CachePose);
		unguardSlow;
	}
	void GetFrame(FVector* Verts, FVector* Norms, INT Size, FCoords Coords, INT* LODRequest = NULL);
	void Draw(FSceneNode* Frame);
	void DrawWireframe(FSceneNode* Frame);
	void PostLoad();

	// API
	FVector GetBoneLocation(INT BoneIndex, UBOOL bOriginal = FALSE);
	FCoords GetBoneCoords(INT BoneIndex, UBOOL bOriginal = FALSE);
	void UpdateFrame();
	void UpdateAttachments(UBOOL bOnlyHard);

	static inline FCoords CalcVertCoords(const FVector& A, const FVector& B, const FVector& C)
	{
		FCoords Result;
		Result.Origin = (A + B + C) / 3.f;
		Result.XAxis = (A - Result.Origin).SafeNormal();
		Result.ZAxis = (Result.XAxis ^ (B - Result.Origin)).SafeNormal();
		Result.YAxis = Result.ZAxis ^ Result.XAxis;
		return Result;
	}

protected:
	FMeshInstance(BYTE T, UMesh* M, AActor* A);
};

struct ENGINE_API FLodMeshInstance : public FMeshInstance
{
	inline FLodMeshInstance(UMesh* M, AActor* A);

	void GetFrame(FVector* Verts, FVector* Norms, INT Size, FCoords Coords, INT* LODRequest = NULL);
	//void Draw(FSceneNode* Frame);
	void DrawWireframe(FSceneNode* Frame);

	// API
	FCoords GetBoneCoords(INT BoneIndex, UBOOL bOriginal = FALSE);

protected:
	inline FLodMeshInstance(BYTE T, UMesh* M, AActor* A);
};

#define IMPLEMENT_INSTACE(insType) \
	virtual FMeshInstanceBase* GetInstance(AActor* Owner) \
	{ \
		FScopeThread<FThreadLock> Scope(GMeshMutex); \
		FMeshInstanceBase* M = Owner->MeshInstance; \
		if (!M || !M->Matches(this, Owner)) \
		{ \
			if (M && M->GetActor() == Owner) delete M; \
			M = new insType(this, Owner); \
			Owner->MeshInstance = M; \
		} \
		return M; \
	}

// A flat shaded mesh representation.
struct ENGINE_API FFlatShadedModel
{
	UBOOL	bSingleFrame;
	UMesh*	SourceMesh;
	INT		NumVerts, NumCalcVerts, NumCalcNormals;
	FLOAT	MirrorNormals;
	TArray<FVector> FVerts, FNormals, CalcVerts, CalcNormals;
	TArray<DWORD> VertIndices, NormalIndices;
	TArray<FMeshTri> Tris;

	FFlatShadedModel(UMesh* M);
	void GetFrame(FVector* Verts, FVector* Norms, INT Size, FCoords Coords, AActor* Owner);

	inline void Shrink()
	{
		FNormals.Shrink();
		CalcVerts.Shrink();
		CalcNormals.Shrink();
		VertIndices.Shrink();
		NormalIndices.Shrink();
		Tris.Shrink();
	}
};

/*-----------------------------------------------------------------------------
	UMesh.
-----------------------------------------------------------------------------*/

//
// A mesh, completely describing a 3D object (creature, weapon, etc) and
// its animation sequences.  Does not reference textures.
//
class ENGINE_API UMesh : public UPrimitive
{
	DECLARE_CLASS(UMesh,UPrimitive,(CLASS_SafeReplace | CLASS_RuntimeStatic),Engine)

	// Objects.
	TLazyArray<FMeshVert>			Verts;
	TArray<FVector>					FVerts, Normals;
	TLazyArray<FMeshTri>			Tris;
	TArray<FMeshAnimSeq>			AnimSeqs;
	TLazyArray<FMeshVertConnect>	Connects;
	TArray<FBox>					BoundingBoxes;
	TArray<FSphere>					BoundingSpheres;
	TLazyArray<INT>					VertLinks;
	TArray<UTexture*>				Textures;
	TArray<FLOAT>					TextureLOD;
	TMap<FName, FMeshAnimSeq*>		AnimMap;

	// Counts.
	INT						FrameVerts;
	INT						AnimFrames;

	// Render info.
	DWORD					AndFlags;
	DWORD					OrFlags;
	UBOOL					bCurvyMesh, bUseFlatShading;
	FVertexList*			VertexBuffer;
	//FMeshIndexBuffer*		IndexBuffer;
	FFlatShadedModel*		FlatModel; // 227j: Simplified flat mesh model.

	// Scaling.
	FVector					Scale;		// Mesh scaling.
	FVector 				Origin;		// Origin in original coordinate system.
	FRotator				RotOrigin;	// Amount to rotate when importing (mostly for yawing).

	// Editing info.
	INT						CurPoly;	// Index of selected polygon.
	INT						CurVertex;	// Index of selected vertex.

	// 227g: 3D collision stuff
	INT CollisionState; // 0 - Uninitialized, 1 - Initialized and ok, 2 - Initialized and not ok.
	FRealCollision RealCollision;

	// 227j Physics collision.
	UModel* CollisionShape;
	PX_MeshShape* RbTriMesh, * RbConvexMesh;

	TArray<FMeshSocketInfo>	MeshSockets, InGameSockets;
	TMap<FName, INT>		SocketMap;
	TArray<FEdPreviewSocket> PreviewSockets;
	TArray<AActor*>			PreviewActors;
	UBOOL					bSocketsDirty;

	EActorRenderPass		InitRenderPass; // 227j: Initialized render pass.

	// 227j UnrealEd
	enum EPreviewDrawFlags : DWORD
	{
		PRFLAGS_DrawSkeleton		= (1 << 0),
		PRFLAGS_DrawSkeletonNames	= (1 << 1),
	};

	// 227j 'OrFlags' flags mesh extra flags
	enum EMeshOrFlags : DWORD
	{
		MESHFLAGS_SingleFrame		= (1 << 0),
		MESHFLAGS_FullFlatShaded	= (1 << 1),
	};

	// UObject interface.
	UMesh();
	void Serialize( FArchive& Ar );
	void StaticConstructor();
	void PostLoad();
	void PostEditChange();
	void onPackageChanged(UObject* OldPackage);

	// UPrimitive interface.
	virtual FBox GetRenderBoundingBox( const AActor* Owner, UBOOL Exact );
	virtual FSphere GetRenderBoundingSphere( const AActor* Owner, UBOOL Exact );
	PX_MeshShape* GetConvexVolume();
	PX_MeshShape* GetTriangleMesh();
	void DeletePhysicsMesh();

	// UMesh interface.
	UMesh( INT NumPolys, INT NumVerts, INT NumFrames, INT NumConnects );
	virtual const FMeshAnimSeq* GetAnimSeq( FName SeqName ) const
	{
		guardSlow(UMesh::GetAnimSeq);
		return AnimMap.FindRef(SeqName);
		/*for( INT i=0; i<AnimSeqs.Num(); i++ )
			if( SeqName == AnimSeqs(i).Name )
				return &AnimSeqs(i);
		return NULL;*/
		unguardSlow;
	}
	virtual FMeshAnimSeq* GetAnimSeq( FName SeqName )
	{
		guardSlow(UMesh::GetAnimSeq);
		return AnimMap.FindRef(SeqName);
		/*for( INT i=0; i<AnimSeqs.Num(); i++ )
			if( SeqName == AnimSeqs(i).Name )
				return &AnimSeqs(i);
		return NULL;*/
		unguardSlow;
	}
	virtual void GetFrame( FVector* Verts, INT Size, FCoords Coords, AActor* Owner, INT* LODRequest=NULL);

	// Texture interface.
	inline INT GetNumTextures() const
	{
		return Textures.Num();
	}
	virtual UTexture* GetTexture(INT Count, AActor* Owner) const;
	virtual void GetTextures(UTexture** TexArray, AActor* Owner) const;

	virtual void SetScale( FVector NewScale );
	BYTE FastLineCheck( const FVector& End, const FVector& Start, AActor* Owner = NULL );
	BYTE FastLineCheckLight(const FVector& End, const FVector& Start, AActor* Owner = NULL);
	BYTE CheckLightrayBlock( const FVector& Pos, const FVector** Clips, const FVector** Pts, INT iSurf, BYTE bBacksideHit, AActor* Owner );
	virtual void MapAnimations();

	IMPLEMENT_INSTACE(FMeshInstance)

	virtual BYTE ShouldSerializeInstance(AActor* Other)
	{
		return 0;
	}
	virtual void ResetNormals();
	virtual void CalcFrameNormals();
	virtual void PreDrawMesh();

	inline EActorRenderPass GetRenderPass()
	{
		if (InitRenderPass == RENDPASS_None)
			InitRenderPass = InitMeshRenderPass();
		return InitRenderPass;
	}
	virtual EActorRenderPass InitMeshRenderPass();
	virtual FFlatShadedModel* GetFlatModel(AActor* Owner);

	inline INT FindSocket(const TCHAR* SocketName) const
	{
		guardSlow(USkeletalMesh::FindBone);
		const INT* Result = SocketMap.Find(FName(SocketName, FNAME_Find));
		return (Result ? *Result : INDEX_NONE);
		unguardSlow;
	}
	inline INT FindSocket(FName SocketName) const
	{
		guardSlow(USkeletalMesh::FindBone);
		const INT* Result = SocketMap.Find(SocketName);
		return (Result ? *Result : INDEX_NONE);
		unguardSlow;
	}

	// 3D collision interface
	virtual void SetupCollision();
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
	FBox GetCollisionBoundingBox( const AActor* Owner );

	// 227j skeletal socket support:
	virtual INT GetBoneIndex(FName BoneName);
	virtual FName GetBoneName(INT Index);
	virtual void NotifyLevelChange(); // Flush temporary data of mesh.
	virtual void ResetSockets();
	virtual UBOOL HasWeaponBone() const;
	virtual UBOOL SetSocket(const FMeshSocketInfo& Socket);

	// 227j UnrealEd:
	void onPropertyChange(UProperty* Property, UProperty* OuterProperty);
	virtual void SetDrawMesh(AActor* Owner) {} // Begin to draw this mesh for first frame.
	virtual void DrawPreview(FSceneNode* Frame, AActor* Owner, DWORD DrawFlags); // Draw this mesh preview info in editor.
	virtual void GetAnimationInfo(FSceneNode* Frame, AActor* Owner, FName SeqName, DWORD DrawFlags, TCHAR* Info); // Get editor info.
};

/*-----------------------------------------------------------------------------
	ULodMesh.
-----------------------------------------------------------------------------*/

//
// A LodMesh, completely describing a 3D object (creature, weapon, etc) and
// its animation sequences.  Does not reference textures.
//
class ENGINE_API ULodMesh : public UMesh
{
	DECLARE_CLASS(ULodMesh,UMesh,CLASS_RuntimeStatic,Engine)

	// LOD-specific objects.
	// Make lazy arrays where useful.
	TArray<_WORD>			CollapsePointThus;  // Lod-collapse single-linked list for points.
	TArray<_WORD>           FaceLevel;          // Minimum lod-level indicator for each face.
	TArray<FMeshFace>       Faces;              // Faces
	TArray<_WORD>			CollapseWedgeThus;  // Lod-collapse single-linked list for the wedges.
	TArray<FMeshWedge>		Wedges;             // 'Hoppe-style' textured vertices.
	TArray<FMeshMaterial>   Materials;          // Materials
	TArray<FMeshFace>       SpecialFaces;       // Invisible special-coordinate faces.

	// Misc Internal.
	INT    ModelVerts;     // Number of 'visible' vertices.
	INT	   SpecialVerts;   // Number of 'invisible' (special attachment) vertices.

	// Max of x/y/z mesh scale for LOD gauging (works on top of drawscale).
	FLOAT  MeshScaleMax;

	// Script-settable LOD controlling parameters.
	FLOAT  LODStrength;    // Scales the (not necessarily linear) falloff of vertices with distance.
	INT    LODMinVerts;    // Minimum number of vertices with which to draw a model.
	FLOAT  LODMorph;       // >0.0 = allow morphing ; 0.0-1.0 = range of vertices to morph.
	FLOAT  LODZDisplace;   // Z displacement for LOD distance-dependency tweaking.
	FLOAT  LODHysteresis;  // Controls LOD-level change delay and morphing.

	// Remapping of animation vertices.
	TArray<_WORD> RemapAnimVerts;
	INT    OldFrameVerts;  // Possibly different old per-frame vertex count.

	// 227j: Save extended U/V map.
	UBOOL bExtendedUVMap;

	//  UObject interface.
	ULodMesh();
	void Serialize( FArchive& Ar );
	void StaticConstructor();

	//  UMesh interface.
	void SetScale( FVector NewScale );
	ULodMesh( INT NumPolys, INT NumVerts, INT NumFrames );
	void CalcFrameNormals();
	IMPLEMENT_INSTACE(FLodMeshInstance);
	EActorRenderPass InitMeshRenderPass();

	PX_MeshShape* GetTriangleMesh();

	// 3D collision interface
	virtual void SetupCollision();
	BYTE CheckLightrayBlock( const FVector& Pos, const FVector** Clips, const FVector** Pts, INT iSurf, BYTE bBacksideHit, AActor* Owner );

	// 227j skeletal socket support:
	UBOOL HasWeaponBone() const;

	// 227j UnrealEd:
	void GetAnimationInfo(FSceneNode* Frame, AActor* Owner, FName SeqName, DWORD DrawFlags, TCHAR* Info);
};

struct FMeshVertCacheData
{
	UMesh* Model;
	FName Sequence;
	FLOAT Frame;
	FVector* Verts;

	// Buffering for modern renderer
	QWORD					CacheID;		// Unique cache ID.
	UINT					VAO[64];		// VertexArrayObject
	UINT					VertBuffer[64];
	UINT					TexBuffer[64];
	UINT					ColorBuffer[64];

	bool					UpdateBuffer;

	FMeshVertCacheData( INT VertCount, UMesh* M )
	 : Model(M)
	 , Sequence(NAME_None)
	 , Frame(0.f)
	 , CacheID(0)
	{
		//debugf(TEXT("Init vertex stream for %ls"),Model->GetName());
		Verts = new FVector[VertCount];
	}
	~FMeshVertCacheData()
	{
		//debugf(TEXT("Cleanup vertex stream for %ls"),Model->GetName());
		if( Verts )
			delete[] Verts;
		Verts = NULL;
	}
};

ENGINE_API void FindPointUVMap( FMeshFloatUV& Out, const FVector& Pos, const FMeshFloatUV* EdgeUV, const FVector** Clips, const FVector** Pts );

FLodMeshInstance::FLodMeshInstance(UMesh* M, AActor* A)
	: FMeshInstance(MI_LodMeshInstance,M,A)
{}
FLodMeshInstance::FLodMeshInstance(BYTE T, UMesh* M, AActor* A)
	: FMeshInstance(T, M, A)
{}
#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
