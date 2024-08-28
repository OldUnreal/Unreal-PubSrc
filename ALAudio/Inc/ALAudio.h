/*=============================================================================
	ALAudio.h: Unreal audio public header file.
	Copyright 2015 Oldunreal.
=============================================================================*/

#ifndef _INC_ALAUDIO
#define _INC_ALAUDIO

/*-----------------------------------------------------------------------------
	Dependencies.
-----------------------------------------------------------------------------*/
#ifdef _WIN32
	#include <process.h>     // needed for _beginthreadex()
#elif __LINUX__ || MACOSX || MACOSXPPC
	#include <pthread.h> // needed for pthread_create()
#endif
#include "Engine.h"
#include "Core.h"
#include "UnRender.h"

/*-----------------------------------------------------------------------------
	Global variables.
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Audio compiler specific includes.
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Audio public includes.
-----------------------------------------------------------------------------*/

#include "ALAudioBuild.h"

// UnrealEngine Extension.
#if defined(USE_UNEXT)
	#define UNEXT_INCLUDE_AUDIOSUBSYSTEMS
	#include "UnExt.h"
#endif

#include "ALAudioSubsystem.h"
#include "ALAudioNames.h"

#if __STATIC_LINK

/* No native execs. */

#define AUTO_INITIALIZE_REGISTRANTS_ALAUDIO \
	UALAudioSubsystem::StaticClass();

#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/

#endif
