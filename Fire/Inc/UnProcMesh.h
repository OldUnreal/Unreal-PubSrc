/*=============================================================================
	UnProcMesh.h: Fluid mesh.

	Revision history:
		* Created by Marco
=============================================================================*/

class FProcProcess : public FThreadingBase
{
public:
	class UProcPrimitive* Primitive;
	FLOAT DeltaTime;

	FProcProcess( class UProcPrimitive* P );
	void Main();
};
class FIRE_API UProcPrimitive : public UStaticMesh
{
	friend class FProcProcess;
public:
	DECLARE_CLASS(UProcPrimitive,UStaticMesh,CLASS_Transient,Fire);

	FVector *BufferA,*BufferB,*BaseVerts,*VertNormals;
	FLOAT *BufferFA,*BufferFB,*VertDeltaBuffer;
	UMesh* SourceMesh;
	BYTE bLodMesh,bStaticMesh,bValidMesh;
	AProceduralMesh* Info;
	TArray<INT> VertexLink;
	TArray<FMeshVertConnect> VertexCon;
	FProcProcess* WaveProcess;

	UProcPrimitive()
		: BufferA(NULL), BufferB(NULL), BaseVerts(NULL), VertNormals(NULL), BufferFA(NULL), BufferFB(NULL)
		, VertDeltaBuffer(NULL), SourceMesh(NULL), bValidMesh(0), WaveProcess(NULL)
	{}
	void SetSourceMesh( UMesh* M );
	void Destroy();
	void GetFrame( FVector* Verts, INT Size, FCoords Coords, AActor* Owner, INT* LODFactor=NULL );
	void ProcessFluid( FLOAT dt );
	void ApplyForce( INT iVertex, FLOAT Force );
	void Impulse( const FVector& Pos, const FVector& Force, FLOAT Radius );

	inline FCoords GetInversedCoords()
	{
		FLOAT DrawScale = Info->bParticles ? 1.0f : Info->DrawScale;
		return GMath.UnitCoords / FScale(Info->DrawScale3D*DrawScale,0.0,SHEER_None) / Info->Rotation / (Info->Location + Info->PrePivot);
	}

	FBox GetRenderBoundingBox( const AActor* Owner, UBOOL Exact )
	{
		if( SourceMesh )
			return SourceMesh->GetRenderBoundingBox(Owner,Exact);
		return FBox();
	}
	FSphere GetRenderBoundingSphere( const AActor* Owner, UBOOL Exact )
	{
		if( SourceMesh )
			return SourceMesh->GetRenderBoundingSphere(Owner,Exact);
		return FSphere();
	}
	FBox GetCollisionBoundingBox( const AActor* Owner ) const
	{
		if( SourceMesh )
			return SourceMesh->GetCollisionBoundingBox(Owner);
		return FBox();
	}

	void SetupCollision()
	{
		if (SourceMesh && !SourceMesh->CollisionState)
			SourceMesh->SetupCollision();
	}
	UBOOL PointCheck( FCheckResult &Result, AActor *Owner, FVector Location, FVector Extent, DWORD ExtraNodeFlags )
	{
		if( SourceMesh )
			return SourceMesh->PointCheck(Result,Owner,Location,Extent,ExtraNodeFlags);
		return Super::PointCheck(Result,Owner,Location,Extent,ExtraNodeFlags);
	}
	UBOOL LineCheck( FCheckResult &Result, AActor *Owner, FVector End, FVector Start, FVector Extent, DWORD ExtraNodeFlags )
	{
		if( SourceMesh )
			return SourceMesh->LineCheck(Result,Owner,End,Start,Extent,ExtraNodeFlags);
		return Super::LineCheck(Result,Owner,End,Start,Extent,ExtraNodeFlags);
	}
	BYTE FastLineCheck( const FVector& End, const FVector& Start, AActor* Owner=NULL )
	{
		if( SourceMesh )
			return SourceMesh->FastLineCheck(End,Start,Owner);
		return Super::FastLineCheck(End,Start,Owner);
	}
	void Serialize( FArchive& Ar )
	{
		UObject::Serialize(Ar);
	}
private:
	void TickFluid( FLOAT dt );
};
