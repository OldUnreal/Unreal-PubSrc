/*=============================================================================
	UnScrTex.h: Unreal scripted texture class
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Jack Porter
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

class UScriptedTexture;

struct FDelayedPortal
{
	UScriptedTexture* Tex;
	FVector Position;
	FRotator Rotation;
	AActor* Camera;
	ULevel* Map;

	FDelayedPortal( UScriptedTexture* T, const FVector& P, const FRotator& R, AActor* C, ULevel* M )
		: Tex(T), Position(P), Rotation(R), Camera(C), Map(M)
	{}
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif


/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
