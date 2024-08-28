/*===============================================================================
	UnTerrainInfo.h: Unreal terrain support.

	Revision history:
		* Created by Marco
===============================================================================*/
#pragma once

#define TERRAIN_LIGHT_VER 1
#define ALPHA_TEX_MASK 256

struct FTerrainPart;

class ENGINE_API UTerrainMesh : public UMesh
{
	DECLARE_CLASS(UTerrainMesh, UMesh, (CLASS_Transient | CLASS_NoUserCreate), Engine);
	NO_DEFAULT_CONSTRUCTOR(UTerrainMesh);

	struct FTerrainQuad
	{
		INT NumTris, TrisOffset;
		FBox Bounds;
		FSphere Sphere;
		TArray<INT> Verts;
	};
	struct FTerrainTris
	{
		FPlane TriNormal;
		INT Verts[3], RenderVerts[3];
		FMeshFloatUV UV[3];
		FLOAT EdgeAlpha[3];
		INT MaterialIndex, Quad, Key, Index;

		FTerrainTris(INT q, INT i)
			: MaterialIndex(0), Quad(q), Index(i)
		{}
		inline void Flip()
		{
			Exchange(Verts[0], Verts[2]);
		}
		inline void EnableAlpha()
		{
			EdgeAlpha[0] = EdgeAlpha[1] = EdgeAlpha[2] = 0.f;
			MaterialIndex |= ALPHA_TEX_MASK;
		}
		inline void SetKey() // Set sorting priority.
		{
			Key = MaterialIndex | (Quad << 10);
		}
		inline UBOOL IsAlpha() const
		{
			return (MaterialIndex & ALPHA_TEX_MASK) != 0;
		}
		inline INT GetTexIndex() const
		{
			return (MaterialIndex & (ALPHA_TEX_MASK - 1));
		}
	};
	struct FTerrainVert
	{
		FVector Vert, Normal;
		INT Index;

		FTerrainVert(const FVector& InV, INT OrgVert)
			: Vert(InV), Normal(0.f,0.f,0.f), Index(OrgVert)
		{}
	};
	ATerrainInfo* Terrain;
	INT SizeX, SizeY;

	TArray<FTerrainTris>	TerrainTris;
	TArray<FTerrainVert>	TerrainVerts;
	TArray<FTerrainQuad>	TerrainQuads;
	TArray<FVector>			OrgVerts; // Untransformed raw verts.

	TArray<FTerrainPart*> RenderQuads;
	TArray<FTerrainPart*> RenderDecos;

	// Editor only data:
	TArray<FVector> EdTerrainVerts, EdTerrainSurfs;

	// Object interface
	UTerrainMesh(ATerrainInfo* InTerrain);
	void Serialize(FArchive& Ar);
	void Destroy();

	// Primitive interface
	FBox GetCollisionBoundingBox(const AActor* Owner);
	FBox GetRenderBoundingBox(const AActor* Owner, UBOOL Exact);
	FSphere GetRenderBoundingSphere(const AActor* Owner, UBOOL Exact);
	void SetupCollision();
	UTexture* GetTexture(INT Count, AActor* Owner) const;
	UBOOL PointCheck(FCheckResult& Result, AActor* Owner, FVector Location, FVector Extent, DWORD ExtraNodeFlags);
	UBOOL LineCheck(FCheckResult& Result, AActor* Owner, FVector End, FVector Start, FVector Extent, DWORD ExtraNodeFlags);
	BYTE FastLineCheck(const FVector& End, const FVector& Start, AActor* Owner);
	BYTE FastLineCheckLight(const FVector& End, const FVector& Start, AActor* Owner);
	PX_MeshShape* GetConvexVolume();
	PX_MeshShape* GetTriangleMesh();

	void Rebuild();
	void Cleanup();
	void CalcVertOffsets();
	void RebuildHeightMap(INT StartX = INDEX_NONE, INT EndX = INDEX_NONE, INT StartY = INDEX_NONE, INT EndY = INDEX_NONE);
	void RebuildTris();
	void RebuildMaterials();
	void UpdateUVMap();

	// Editor:
	void SetSurfHidden(INT iSurf, UBOOL bHide);
	void SetEdgeTurn(INT iSurf, UBOOL bTurn);

	// Abstract 2D positions.
	inline FLOAT GetVertexRadius(INT A, INT B) const
	{
		INT y = A / SizeX;
		INT x = A - (y * SizeX);
		const FVector2D vA(x, y);
		y = B / SizeX;
		x = B - (y * SizeX);
		const FVector2D vB(x, y);
		return Square(vB.X - vA.X) + Square(vB.Y - vA.Y);
	}
	inline FLOAT GetSurfaceRadius(INT A, INT B) const
	{
		const INT SurfX = (SizeX - 1);
		INT y = A / SurfX;
		INT x = A - (y * SurfX);
		const FVector2D vA(x, y);
		y = B / SurfX;
		x = B - (y * SurfX);
		const FVector2D vB(x, y);
		return Square(vB.X - vA.X) + Square(vB.Y - vA.Y);
	}
	inline FVector GetVertexPos(INT A) const
	{
		INT y = A / SizeX;
		INT x = A - (y * SizeX);
		return FVector(x, y, 0.f);
	}
	inline FVector GetSurfacePos(INT A) const
	{
		const INT SurfX = (SizeX - 1);
		INT y = A / SurfX;
		INT x = A - (y * SurfX);
		return FVector(x, y, 0.f);
	}
	inline FVector GetVertexMax() const
	{
		return FVector(SizeX, SizeY, 0.f);
	}
	inline FVector GetSurfaceMax() const
	{
		return FVector(SizeX - 1, SizeY - 1, 0.f);
	}
	inline FCoords GetTerrainCoords() const
	{
		return GMath.UnitCoords * Terrain->Rotation * Terrain->Location;
	}
};

struct ENGINE_API FTerrainLight : public FStaticLightActor
{
	INT Version, SerVertexCount;
	TArray<AActor*> SpatialSources;
	TArray<INT> SpatialBits;
	FLazyColorData VertexColor;

	FTerrainLight(INT InVer);

	void Serialize(FArchive& Ar);
	AActor** GetDynamicLights(INT* NumLights);

	void Reset();
	void Build(ATerrainInfo* Terrain);

	inline UBOOL LightDataMatches(UTerrainMesh* TerrainPrim)
	{
		return (SerVertexCount == TerrainPrim->TerrainVerts.Num());
	}
};
