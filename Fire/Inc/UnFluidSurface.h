/*=============================================================================
	UnFluidSurface.h: Simple fluid surface.

	Revision history:
		* Created by Marco
=============================================================================*/

class AFluidSurfaceInfo;

class FIRE_API UFluidPrimitive : public UStaticMesh
{
public:
	DECLARE_CLASS(UFluidPrimitive,UStaticMesh,CLASS_Transient,Fire);

	FLOAT* BufferA, * BufferB, * RenderBuffer;
	FVector* VertsPos;
	INT RealX,RealY,XSize,YSize,FullSize,OldSize;
	FLOAT SpaceX,SpaceY,Diffuse,WaveSizeLimit[2];
	TArray<INT> LookupTable;
	UBOOL bSquaredModel;
	BYTE* ClampedPoints;

	UFluidPrimitive()
		: BufferA(NULL), OldSize(0)
	{}

	void Destroy();
	void TickFluid( FLOAT Delta );
	void PockWaves( INT X, INT Y, FLOAT Depth, FLOAT Radius );
	void PockWaves( const FVector& Pos, AFluidSurfaceInfo* Info, FLOAT Depth, FLOAT Radius, BYTE bForce );
	void GetFrame( FVector* Verts, INT Size, FCoords Coords, AActor* Owner, INT* LODFactor=NULL );
	void InitFluid( AFluidSurfaceInfo* Info );
	UTexture* GetTexture(INT Count, AActor* Owner) const;
	void GetTextures(UTexture** TexArray, AActor* Owner) const;
	void SetupCollision();
	void BuildConnections( AFluidSurfaceInfo* Info );
	void ClampPoints( AFluidSurfaceInfo* Info );

	inline FCoords GetWaveCoords( const FCoords& Base, AActor* Info )
	{
		return Base * (Info->Location+Info->PrePivot) * Info->Rotation * FScale(Info->DrawScale3D*Info->DrawScale,0.0,SHEER_None);
	}

	BYTE CheckLightrayBlock( const FVector& Pos, const FVector** Clips, const FVector** Pts, INT iSurf, BYTE bBacksideHit, AActor* Owner )
	{
		return 1;
	}
	void Serialize( FArchive& Ar )
	{
		UObject::Serialize(Ar);
	}
private:
	void ProcessWater( FLOAT dt );
};
