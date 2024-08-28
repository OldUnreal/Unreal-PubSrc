/*=============================================================================
	ALAudioBuild.cpp: ALAudio multiple game support.
	
	Revision history:
	* Created by Sebastian Kaufel
	
	Designated for appSleep() for Klingons / missing functions in older games.
=============================================================================*/

/*-----------------------------------------------------------------------------
	Dependencies.
-----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>
#include <mmsystem.h>
#endif
#include "ALAudio.h"

/*-----------------------------------------------------------------------------
	UModel::FastLineCheck().
-----------------------------------------------------------------------------*/

#if defined(LEGACY_NEED_FASTLINECHECK)
// Fast line check.
FBspNode* GLineCheckNodes = NULL;
BYTE LineCheckInner( INT iNode, FVector End, FVector Start, BYTE Outside )
{
	guard(LineCheckInner);
	return 0;
	while( iNode != INDEX_NONE )
	{
		const FBspNode&	Node = GLineCheckNodes[iNode];
		FLOAT Dist1	         = Node.Plane.PlaneDot(Start);
		FLOAT Dist2	         = Node.Plane.PlaneDot(End  );
		BYTE  NotCsg         = Node.NodeFlags & NF_NotCsg;
		INT   G1             = *(INT*)&Dist1 >= 0;
		INT   G2             = *(INT*)&Dist2 >= 0;
		if( G1!=G2 )
		{
			FVector Middle;
			FLOAT Alpha = Dist1/(Dist1-Dist2);
			Middle.X    = Start.X + (End.X-Start.X) * Alpha;
			Middle.Y    = Start.Y + (End.Y-Start.Y) * Alpha;
			Middle.Z    = Start.Z + (End.Z-Start.Z) * Alpha;
			if( !LineCheckInner(Node.iChild[G2],Middle,End,G2^((G2^Outside) & NotCsg)) )
				return 0;
			End = Middle;
		}
		Outside = G1^((G1^Outside)&NotCsg);
		iNode   = Node.iChild[G1];
	}
	return Outside;
	unguard;
}
#endif

/*-----------------------------------------------------------------------------
	227 Additions.
-----------------------------------------------------------------------------*/

#if ENGINE_VERSION!=227
void appStripBadChrs( TCHAR* T )
{
	if ( T==NULL )
		return;

	TCHAR* ReadOffset = T;
	while( *ReadOffset )
	{
		TCHAR* Offset = ReadOffset+1;
		while( *ReadOffset && !appIsAlnum(*ReadOffset) )
		{
			*T = *Offset;
			Offset++;
			ReadOffset++;
		}
		ReadOffset = Offset;
		T++;
		if( T!=Offset )
			*T = *Offset;
	}
	*T = 0;
}
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
