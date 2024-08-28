/*=============================================================================
	FractalPrivate.h: Fractal texture effects private header file.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	By Erik de Neve.
=============================================================================*/

/*----------------------------------------------------------------------------
	Dependencies.
----------------------------------------------------------------------------*/

#include "Engine.h"

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

/*----------------------------------------------------------------------------
	General structures.
----------------------------------------------------------------------------*/

// Lightning.
struct LineSeg
{
	BYTE Xpos,Ypos,Xlen,Ylen;
};

#if 0
#define FF_HALF(Val) ((Val)>>1)
#else
#define FF_HALF(Val) (Val)
#endif

/*----------------------------------------------------------------------------
	UFireTexture.
----------------------------------------------------------------------------*/

// Fire constants.
#define MAXSPARKSINIT     1024 /* _Initial_ maximum sparks per fire texture. */
#define MAXSPARKSLIMIT    8192 /* Total allowed ceiling...*/
#define MINSPARKSLIMIT    4    /* Minimum required number */

/*----------------------------------------------------------------------------
	UIceTexture.
----------------------------------------------------------------------------*/

// Ice constants.
#define MaxGuideKeys 64

// A keypoint for movement.
// warning: Mirrored in IceTexture.uc.
#if 0 // Not in use!
struct DLL_EXPORT_CLASS KeyPoint
{
	BYTE    X GCC_PACK(INT_ALIGNMENT);       // Keypoint X - fix
	BYTE    Y;       // Keypoint Y - fix
	BYTE    SpeedX;
	BYTE    SpeedY;

	BYTE    Pause1;
	BYTE    Pause2;
	BYTE    Pause3;
	BYTE    Pause4;
};
#endif

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

#include "FireClasses.h"

/*-----------------------------------------------------------------------------
    The End.
-----------------------------------------------------------------------------*/
