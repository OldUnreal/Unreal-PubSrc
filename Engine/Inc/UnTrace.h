/*=============================================================================
	ATrace.h: Trace helper function.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

// Find the coplanar surface corresponding to this intersection point.
static inline INT FindCoplanarSurface( UModel* Model, const INT iNode, const FVector& IntersectionPoint )
{
	guard(FindCoplanarSurface);
	if( iNode == INDEX_NONE )
		return INDEX_NONE;

	INT iTest = iNode;
	const FBspNode* Node;
	while (iTest != INDEX_NONE)
	{
		Node = &Model->Nodes(iTest);
		if (Node->NumVertices > 0)
		{
			// check if this intersection point lies inside this node.
			FVert* Verts = &Model->Verts(Node->iVertPool);
			FVector& SurfNormal = Model->Vectors(Model->Surfs(Node->iSurf).vNormal);

			FVector* PrevVertex = &Model->Points(Verts[Node->NumVertices - 1].pVertex);
			UBOOL Success = 1;
			FLOAT PrevDot = 0;
			for (INT i = 0; i < Node->NumVertices; i++)
			{
				FVector* Vertex = &Model->Points(Verts[i].pVertex);
				FVector ClipNorm = SurfNormal ^ (*Vertex - *PrevVertex);
				FPlane ClipPlane(*Vertex, ClipNorm);

				FLOAT Dot = ClipPlane.PlaneDot(IntersectionPoint);

				if ((Dot < 0 && PrevDot > 0) ||
					(Dot > 0 && PrevDot < 0))
				{
					Success = 0;
					break;
				}
				PrevDot = Dot;
				PrevVertex = Vertex;
			}
			if (Success)
				return Node->iSurf;
		}

		// check next co-planars to see if it contains this intersection point.
		iTest = Node->iPlane;
	}
	
	return Model->Nodes(iNode).iSurf;
	unguardf((TEXT("(Node %i/%i)"), iNode, Model->Nodes.Num()));
}
/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
