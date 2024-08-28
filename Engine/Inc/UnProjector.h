/*===============================================================================
	UnProjector.cpp: Unreal projector support.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Marco
===============================================================================*/

#pragma once

/*===============================================================================
 Major maths functions:
===============================================================================*/

#define GET_DIAG_SIZE appSqrt(ProjectorScale * ProjectorScale * ProjectTexture->USize * ProjectTexture->USize * 0.5f)
#define GET_FOV_ANG_VALUE GMath.SinTab(appRound(FLOAT(Min<BYTE>(FOV,180))/180.f*16384.f))
#define GET_PROJ_COLOR(P) \
	((P->ActorRenderColor.R || P->ActorRenderColor.G || P->ActorRenderColor.B) \
	 ? (P->ProjectStyle==STY_AlphaBlend ? FPlane(FLOAT(P->ActorRenderColor.R)/255.f,FLOAT(P->ActorRenderColor.G)/255.f,FLOAT(P->ActorRenderColor.B)/255.f,P->ScaleGlow) \
	 : FPlane(FLOAT(P->ActorRenderColor.R)/255.f*P->ScaleGlow,FLOAT(P->ActorRenderColor.G)/255.f*P->ScaleGlow,FLOAT(P->ActorRenderColor.B)/255.f*P->ScaleGlow,1.f)) \
	 : (P->ProjectStyle==STY_AlphaBlend ? FPlane(1.f,1.f,1.f,P->ScaleGlow) : FPlane(P->ScaleGlow,P->ScaleGlow,P->ScaleGlow,1.f)))
#define MAX_ProjAllocSize 80

void SetupPrimitive( AProjector* Decal );

#define MAX_PROJ_DRAW_PTS 512

struct FProjectorClipper
{
private:
	FLOAT* Dots;
	FTransTexture* TTex;
	FTransTexture** TTexPtr;
	FVector* TestPts;
	BYTE PtrIndex, NumPoints;

	// Render:
	struct FSceneNode* DrawFrame;
	struct FTextureInfo* TexInfo;
	INT NumDrawPts;
	class URenderDevice* RenDev;
	FTransTexture* DTTex,*TTTx;
	class FSpanBuffer* DrawSpan;
	DWORD DrawFlags;

	inline void ScaleValue(FTransTexture& Goal, const FTransTexture& A, const FTransTexture& B, const FLOAT Scale) const
	{
		Goal.U = Lerp(A.U, B.U, Scale);
		Goal.V = Lerp(A.V, B.V, Scale);
		Goal.Light = A.Light.Lerp(B.Light, Scale);
		Goal.Point = A.Point.Lerp(B.Point, Scale);
	}
public:
	FProjectorClipper();
	~FProjectorClipper();

	// Test clipping polies:
	inline FVector* InitTestPolies(BYTE Count)
	{
		NumPoints = Count;
		return TestPts;
	}
	inline BYTE CanClipPoints(const FPlane* Clips, BYTE NumPlanes)
	{
		BYTE TotalPts = NumPoints;
		for (BYTE x = 0; x < NumPlanes; ++x)
		{
			for (BYTE i = 0; i < TotalPts; i++)
			{
				INT j = 0;
				for (; j < NumPoints; j++)
					Dots[j] = Clips[x].PlaneDot(TestPts[j]);
				for (j = 0; j < NumPoints; j++)
				{
					if ((Dots[j] > 0 && Dots[(j + 1) % NumPoints] < 0)
						|| (Dots[j] < 0 && Dots[(j + 1) % NumPoints] > 0)) // Does clip plane.
					{
						FVector NewPoint = FLinePlaneIntersection(TestPts[j], TestPts[(j + 1) % NumPoints], Clips[x]);
						if (j < NumPoints - 1)
						{
							// move Dots[] and Pts[] arrays along
							appMemmove(&Dots[j + 2], &Dots[j + 1], sizeof(FLOAT) * (NumPoints - j - 1));
							appMemmove(&TestPts[j + 2], &TestPts[j + 1], sizeof(FVector) * (NumPoints - j - 1));
						}
						TestPts[j + 1] = NewPoint;
						Dots[j + 1] = 0;
						++j;
						if (++NumPoints == MAX_ProjAllocSize)
							return 1;
					}
				}
				for (j = 0; j < NumPoints; j++)
				{
					if (Dots[j] < 0)
					{
						appMemmove(&Dots[j], &Dots[j + 1], sizeof(FLOAT) * (NumPoints - j - 1));
						appMemmove(&TestPts[j], &TestPts[j + 1], sizeof(FVector) * (NumPoints - j - 1));
						--j;
						--NumPoints;
					}
				}
				if (NumPoints<3)
					return 0;
			}
		}
		return (NumPoints > 2);
	}

	// Clipping polies:
	inline FTransTexture* InitClipping(BYTE Count)
	{
		for (BYTE i = 0; i < Count; ++i)
			TTexPtr[i] = &TTex[i];
		PtrIndex = Count;
		NumPoints = Count;
		return TTex;
	}
	inline FTransTexture** GetVertPtr()
	{
		return TTexPtr;
	}
	inline BYTE GetVerts()
	{
		return NumPoints;
	}
	inline BYTE IsValidSurf()
	{
		return (NumPoints > 2);
	}

	inline void ClipEdge(const FPlane& Plane)
	{
		if (NumPoints<3 || PtrIndex == MAX_ProjAllocSize)
			return;

		BYTE TotalPts = NumPoints;
		for (BYTE i = 0; i < TotalPts; i++)
		{
			INT j = 0;
			for (; j < NumPoints; j++)
				Dots[j] = Plane.PlaneDot(TTexPtr[j]->Point);
			for (j = 0; j < NumPoints; j++)
			{
				if ((Dots[j] > 0 && Dots[(j + 1) % NumPoints] < 0)
					|| (Dots[j] < 0 && Dots[(j + 1) % NumPoints] > 0)) // Does clip plane.
				{
					FTransTexture* OldPoint = TTexPtr[(j + 1) % NumPoints];
					FLOAT OldDot = Dots[(j + 1) % NumPoints];
					if (j < NumPoints - 1)
					{
						// move Dots[] and Pts[] arrays along
						appMemmove(&Dots[j + 2], &Dots[j + 1], sizeof(FLOAT) * (NumPoints - j - 1));
						appMemmove(&TTexPtr[j + 2], &TTexPtr[j + 1], sizeof(void*) * (NumPoints - j - 1));
					}
					TTexPtr[j + 1] = &TTex[PtrIndex++];
					ScaleValue(*TTexPtr[j + 1], *TTexPtr[j], *OldPoint, (Dots[j] / (Dots[j] - OldDot)));
					Dots[j + 1] = 0;
					++j;
					if (PtrIndex == MAX_ProjAllocSize || ++NumPoints == MAX_ProjAllocSize)
						return;
				}
			}
			for (j = 0; j < NumPoints; j++)
			{
				if (Dots[j] < 0)
				{
					appMemmove(&Dots[j], &Dots[j + 1], sizeof(FLOAT) * (NumPoints - j - 1));
					appMemmove(&TTexPtr[j], &TTexPtr[j + 1], sizeof(void*) * (NumPoints - j - 1));
					j--;
					NumPoints--;
				}
			}
			if (NumPoints<3)
				return;
		}
	}

	inline void PredrawProjector(FSceneNode* Frame, FTextureInfo* TI, URenderDevice* R, DWORD pf, FSpanBuffer* S=NULL )
	{
		DrawFrame = Frame;
		TexInfo = TI;
		RenDev = R;
		NumDrawPts = 0;
		TTTx = DTTex;
		DrawSpan = S;
		DrawFlags = pf;
	}
	inline void DrawPoliesPtr(FTransTexture** P, BYTE NumPts)
	{
		INT count = (NumPts - 2) * 3;
		if ((NumDrawPts + count) >= MAX_PROJ_DRAW_PTS)
		{
			RenDev->DrawGouraudPolyList(DrawFrame, *TexInfo, DTTex, NumDrawPts, DrawFlags, DrawSpan);
			NumDrawPts = count;
			TTTx = DTTex;
		}
		else NumDrawPts += count;

		for (BYTE j = 2; j < NumPts; ++j)
		{
			*TTTx++ = *P[0];
			*TTTx++ = *P[j - 1];
			*TTTx++ = *P[j];
		}
	}
	inline void DrawPolies(FTransTexture* P, BYTE NumPts)
	{
		INT count = (NumPts - 2) * 3;
		if ((NumDrawPts + count) >= MAX_PROJ_DRAW_PTS)
		{
			RenDev->DrawGouraudPolyList(DrawFrame, *TexInfo, DTTex, NumDrawPts, DrawFlags, DrawSpan);
			NumDrawPts = count;
			TTTx = DTTex;
		}
		else NumDrawPts += count;

		for (BYTE j = 2; j < NumPts; ++j)
		{
			*TTTx++ = P[0];
			*TTTx++ = P[j - 1];
			*TTTx++ = P[j];
		}
	}
	inline void PostdrawProjector()
	{
		if(NumDrawPts>=3)
			RenDev->DrawGouraudPolyList(DrawFrame, *TexInfo, DTTex, NumDrawPts, DrawFlags, DrawSpan);
	}
};
ENGINE_API extern FProjectorClipper GProjClipper;

inline bool IsNotBackface( const FVector& Normal, AProjector* Decal )
{
	FLOAT DotVal = (Decal->FrustrumPlanes[4] | Normal);
	return ((Decal->bProjectBSPBackfaces || (DotVal<0.f)) && Abs(DotVal)>DELTA);
}
inline bool IsNotBackfaceMesh( const FVector& Normal, AProjector* Decal )
{
	FLOAT DotVal = (Decal->FrustrumPlanes[4] | Normal);
	return ((Decal->bProjectMeshBackfaces || (DotVal<0.f)) && Abs(DotVal)>DELTA);
}

inline bool WithinBoundingBox( const FBox& A, const FBox& B )
{
	return (((A.Min.X<B.Min.X && A.Max.X>B.Min.X) || (A.Min.X>B.Min.X && A.Min.X<B.Max.X))
		 && ((A.Min.Y<B.Min.Y && A.Max.Y>B.Min.Y) || (A.Min.Y>B.Min.Y && A.Min.Y<B.Max.Y))
		 && ((A.Min.Z<B.Min.Z && A.Max.Z>B.Min.Z) || (A.Min.Z>B.Min.Z && A.Min.Z<B.Max.Z)));
}
struct FBoxBoundsCheck
{
	AProjector* D;
	UModel& M;
	FVector Extent,MidPoint;
	struct FStaticProjector* OutMap;
	FMovingBrushTrackerBase* Tracker;

	FBoxBoundsCheck(AProjector* Decal, UModel* Model, const FBox& Bounds, FMovingBrushTrackerBase* T);
	void TestFace(INT iNode);
	void CheckCollisionBox(INT iNode, INT iParent);
	struct FProjectorSurf* BakeNode(INT iNode);
};

/*-----------------------------------------------------------------------------
	Static projector render instance.
-----------------------------------------------------------------------------*/

struct FProjSurfData
{
	BYTE NumVerts;
	FVector* Pts;
	FPlane* Color;
	FLOAT* U, * V;

	FProjSurfData(BYTE Verts);
	~FProjSurfData();
};
struct ENGINE_API FProjectorSurf
{
	AProjector* Proj;
	TArray<FProjSurfData> Surfs;

	FProjectorSurf(AProjector* P);
	void Draw(struct FSceneNode* Frame, INT Displacement, class FSpanBuffer* Span=NULL); // Really fast draw method without checks.
};
struct FProjectorSurfMesh
{
	TArray<FProjectorSurf*> SurfLOD; // Must have one for each static mesh LOD level.

	FProjectorSurfMesh(INT c)
	{
		SurfLOD.AddZeroed(c);
	}
	~FProjectorSurfMesh();
};
struct FStaticProjector
{
	TMap<AActor*, FProjectorSurfMesh*> StaticMap;
	TArray<FProjectorSurf*> BSP;
	AProjector* Proj;

	FStaticProjector(AProjector* P);
	~FStaticProjector();

	BYTE BuildStaticMesh(AActor* A);
	void Empty();
};
class UProjectorPrimitive : public UPrimitive
{
	DECLARE_CLASS(UProjectorPrimitive, UPrimitive, CLASS_Transient, Engine)

	// UPrimitive collision interface.
	UBOOL PointCheck(FCheckResult& Result, AActor* Owner, FVector	Location, FVector Extent, DWORD ExtraNodeFlags);
	UBOOL LineCheck(FCheckResult& Result, AActor* Owner, FVector End, FVector Start, FVector Extent, DWORD ExtraNodeFlags);
	FBox GetCollisionBoundingBox(const AActor* Owner);
	void Destroy();
	static UPrimitive* GetCollision();
};
