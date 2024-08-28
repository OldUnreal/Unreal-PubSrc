/* ============================================================================
	SwFMODGeometry.cpp:
	Copyright 2007 Roman Switch` Dzieciol. All Rights Reserved.
===============================================================================
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
============================================================================ */


// Includes.
#include "SwFMOD.h"

/*-----------------------------------------------------------------------------
	FSwTriangulator
-----------------------------------------------------------------------------*/
struct FSwTriangulator
{
	UModel*						Model;
	FMovingBrushTrackerBase*	BrushTracker;
	TArray<INT>					Triangles;
	FBox						Box;

	FSwTriangulator(UModel* M, FMovingBrushTrackerBase* T)
		: Model(M), BrushTracker(T), Box(0)
	{}

	void RecurseBSP(INT iNode)
	{
		while (iNode != INDEX_NONE)
		{
			const FBspNode& curNode = Model->Nodes(iNode);
			if (!BrushTracker || !BrushTracker->SurfIsDynamic(curNode.iSurf))
			{
				INT iCoplanar = iNode;
				while (iCoplanar != INDEX_NONE)
				{
					const FBspNode& PlaneNode = Model->Nodes(iCoplanar);
					const FBspSurf& Surf = Model->Surfs(PlaneNode.iSurf);

					if ((PlaneNode.NumVertices > 0) && !(Surf.PolyFlags & PF_NotSolid))
					{
						FVert* GVerts = &Model->Verts(PlaneNode.iVertPool);

						for (INT i = 2; i < PlaneNode.NumVertices; i++)
						{
							Triangles.AddItem(GVerts[0].pVertex);
							Triangles.AddItem(GVerts[i - 1].pVertex);
							Triangles.AddItem(GVerts[i].pVertex);
						}
					}
					iCoplanar = PlaneNode.iPlane;
				}
			}
			if (curNode.iBack != INDEX_NONE)
				RecurseBSP(curNode.iBack);
			iNode = curNode.iFront;
		}
	}
	bool Triangulate()
	{
		guard(FSwTriangulator::Triangulate);

		// Triangulate convex polygons

		// Get faces
		RecurseBSP(0);

		if (Triangles.Num() < (4 * 3)) // Not enough polies to even make a cube...
			return false;

		// calc bounding box
		FVector* GPts = &Model->Points(0);
		int numPts = Model->Points.Num();
		for (int i = 0; i < numPts; ++i)
			Box += GPts[i];

		return true;
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	USwFMOD
-----------------------------------------------------------------------------*/

void USwFMOD::LoadGeometry(ULevel* Level)
{
	guard(USwFMOD::LoadGeometry);
	FMOD_RESULT result;
	//SWF_LOG( SWF_NAME, TEXT("%ls -- %ls :: "), SWF_PLOG );

	if( Geometry )
	{
		Geometry->setActive(false);
		Geometry->release();
		Geometry = NULL;
	}

	if (!Level)
		return;

	FSwTriangulator tor(Level->Model, Level->BrushTracker);

	if (!tor.Triangulate())
		return;

	FLOAT worldsize = Max( tor.Box.Min.Size(), tor.Box.Max.Size() );
	//SWF_LOG( SWF_NAME, TEXT("%ls -- %ls :: World size[%ls] Points[%ls] Triangles[%ls]"), SWF_PLOG, *ToStr(worldsize), *ToStr(tor.Points.Num()), *ToStr(tor.Triangles.Num()) );
	SWF_FMOD_RCALL( System->setGeometrySettings(ToFMODFloat(worldsize)) );

	SWF_FMOD_RCALL( System->createGeometry(tor.Triangles.Num()/3, tor.Triangles.Num(), &Geometry) );
	int index = 0;
	FMOD_VECTOR vertices[3];
	INT* iTris = &tor.Triangles(0);
	FVector* gVerts = &tor.Model->Points(0);
	for (INT i = 0; i < tor.Triangles.Num(); i += 3)
	{
		vertices[2] = ToFMODVector(gVerts[iTris[i]]);
		vertices[1] = ToFMODVector(gVerts[iTris[i + 1]]);
		vertices[0] = ToFMODVector(gVerts[iTris[i + 2]]);
		//SWF_LOG( SWF_NAME, TEXT("%ls -- %ls :: Adding [%ls] [%ls] [%ls]"), SWF_PLOG, *ToStr(vertices[0]), *ToStr(vertices[1]), *ToStr(vertices[2]) );
		SWF_FMOD_RCALL( Geometry->addPolygon( 0.75f, 0.0f, false, 3, vertices, &index) );
	}

	Geometry->setActive(true);
	unguard;
}


/*------------------------------------------------------------------------------------
	USwFMOD.
------------------------------------------------------------------------------------*/


UBOOL USwFMOD::IsObstructed( AActor* Actor )
{
	guard(USwFMOD::IsObstructed);
	//SWF_LOG( SWF_NAME, TEXT("%ls -- %ls :: "), SWF_PLOG );
	//FMOD_RESULT result;

	return 0;
	unguard;
}


void USwFMOD::RenderAudioGeometry( FSceneNode* Frame )
{
	guard(USwFMOD::RenderAudioGeometry);
	FMOD_RESULT result;

	if( !GRenderDevice || !Geometry || !StatRender )
		return;

	static TArray<FMOD_VECTOR> poly;
	static FPlane linecolor = FPlane(1,0,0,1);

	int polys;
	SWF_FMOD_RCALL( Geometry->getNumPolygons(&polys) );
	for( int i=0; i<polys; ++i )
	{
		int tris;
		SWF_FMOD_RCALL( Geometry->getPolygonNumVertices(i,&tris) );

		if( poly.Num() < tris )
			poly.Insert(0,tris-poly.Num());

		for( int j=0; j<tris; ++j )
		{
			SWF_FMOD_RCALL( Geometry->getPolygonVertex(i,j,&poly(j)) );
			if( j != 0 )
			{
				// TODO :: offset by normal to prevent z-artifacts
				GRenderDevice->Draw3DLine( Frame, linecolor, 0, ToUEVector(poly(j-1)), ToUEVector(poly(j)) );
			}
		}

		// TODO :: render normal
		// TODO :: don't render distant ones
	}

	unguard;
}


/* ----------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------- */
