/*=============================================================================
	ALAudio.cpp: Unreal audio package.
	Copyright 2015 Oldunreal.
=============================================================================*/

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
	Globals.
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Package implementation.
-----------------------------------------------------------------------------*/

IMPLEMENT_PACKAGE(ALAudio);

/*-----------------------------------------------------------------------------
	Autogenerate Names.
-----------------------------------------------------------------------------*/

#define NAMES_ONLY
#define AUTOGENERATE_NAME(name) ALAUDIO_API FName ALAUDIO_##name;
#include "ALAudioNames.h"
#undef AUTOGENERATE_NAME
#undef NAMES_ONLY

void UALAudioSubsystem::RegisterNames()
{
	guard(UALAudioSubsystem::RegisterNames);
	static INT Registered=0;
	if(!Registered++)
	{
		#define NAMES_ONLY
		#define AUTOGENERATE_NAME(name) extern ALAUDIO_API FName ALAUDIO_##name; ALAUDIO_##name=FName(TEXT(#name),FNAME_Intrinsic);
		#include "ALAudioNames.h"
		#undef DECLARE_NAME
		#undef NAMES_ONLY
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	UnExt.
-----------------------------------------------------------------------------*/

#if defined(UNEXT_VERSION)
	#define UNEXT_GUID_ONLY
	#define UNEXT_AUTOGENERATE_GUID(name,a,b,c,d) FGuid name = FGuid(a,b,c,d);
	#include "UnExt.h"
	#undef AUTOGENERATE_FUNCTION
	#undef UNEXT_GUID_ONLY
#endif

/*-----------------------------------------------------------------------------
	The end.
-----------------------------------------------------------------------------*/
