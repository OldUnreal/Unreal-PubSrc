/*===============================================================================
	UnStaticLight.h: Unreal static lighting support.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Marco
===============================================================================*/

#pragma once
#define StaticLightVer 2

struct ENGINE_API FLazyColorData : private FLazyLoader
{
	TArray<FPlane> StaticLights;

	FLazyColorData()
	: FLazyLoader()
	, StaticLights(0)
	{}
	~FLazyColorData() noexcept(false)
	{
		guard(FLazyColorData::~FLazyColorData);
		if( SavedAr )
			SavedAr->DetachLazyLoader( this );
		unguard;
	}
	void Load();
	void Unload();
	friend FArchive& operator<<( FArchive& Ar, FLazyColorData& This );
};

struct ENGINE_API FMeshLightmap
{
	INT DetailLevel;
	TArray<AActor*> Lights;
	TArray<BYTE> ShadowMask;
	TArray<BYTE> DecompLightmap;

	FMeshLightmap();
	friend FArchive& operator<<(FArchive& Ar, FMeshLightmap& LightMap);

	// Compression for static lightmap.
	void Compress(); // Compress from DecompLightmap -> ShadowMask
	void Decompress(); // Decompress from ShadowMask -> DecompLightmap
};

struct ENGINE_API FRenderLightmapBase
{
	FRenderLightmapBase()
	{}
	virtual ~FRenderLightmapBase() noexcept(false)
	{}
	virtual void Flush() _VF_BASE;
};

class ENGINE_API UStaticLightData : public UObject
{
	DECLARE_CLASS(UStaticLightData,UObject,CLASS_RuntimeStatic,Engine)

	BYTE SavedLightVer,bSingleLightmap;
	INT BuiltFrameVerts; // Used for validating if this data still valid.
	FLazyColorData ColorData;
	TArray<BYTE> DynLightSources; // Dynamic light source flags.
	TArray<AActor*> DynamicLightActors; // For changing light sources (MAX 8).
	struct FStaticMeshLight* DataRef;

	UBOOL bHasLightmap;
	FMeshLightmap* Lightmap{};
	FRenderLightmapBase* RenderLightmap{};

	TArray<UStaticLightData*> LODLevels; // Static light data for the different mesh LOD levels.

	// Editor API:
	struct ENGINE_API FSLightInfo
	{
		AActor* LightActor;
		FPlane LightColor;
		FLOAT LightRadii, LightRadiiSq;
		FVector LightDir;
		UBOOL bIsDynamicSource, bIsSpecialLit;
		FSLightInfo* NextSrc;

		FSLightInfo(AActor* A, FSLightInfo* N);
		FLOAT LightBrightness(const FVector& Vertex, const FVector* VertexNormal, ULevel* Level, UBOOL bTwoSided) const;
	};

	static FSLightInfo* LightList;

	// Editor stuff
	FVector SetupPosition;

	// Object interface
	UStaticLightData() {}
	void StaticConstructor();
	void Serialize( FArchive& Ar );
	void PostLoad();
	void Destroy();

	// Static lighting functions
	bool LightDataValid( UMesh* M );
	FVector GetStaticLightSurf( INT SurfIndex );
	FStaticLightActor* GetLightData();

	// Editor support
	bool BuildStaticMeshLighting(AActor* SM);
	void PrepareMesh(AActor* SM);
	static void PreBuildLights(class ULevel* Map);
	static void PostBuildLights(class ULevel* Map);
	static UBOOL LitVertex(FPlane& VertexColor, INT& SpatialBits, const INT maxSpatialBits, TArray<AActor*>& SpatialSrc, FSLightInfo** RelevantLights, const INT NumRelLights, ULevel* Map, UBOOL bTwoSided, const FVector& Vertex, const FVector& Normal);
};
struct ENGINE_API FStaticMeshLight : public FStaticLightActor
{
	UStaticLightData* Data;

	FStaticMeshLight(UStaticLightData* Light);
	AActor** GetDynamicLights(INT* NumLights);
	void Flush();
	UStaticLightData* GetStaticMeshData() const
	{
		return Data;
	}
};

inline FStaticLightActor* UStaticLightData::GetLightData()
{
	guardSlow(UStaticLightData::GetLightData);
	if (!DataRef)
		DataRef = new FStaticMeshLight(this);
	return DataRef;
	unguardSlow;
}
