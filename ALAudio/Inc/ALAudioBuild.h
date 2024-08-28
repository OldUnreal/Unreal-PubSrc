/*=============================================================================
	ALAudioBuild.h: ALAudio multiple game support.

	Revision history:
	* Created by Sebastian Kaufel
=============================================================================*/

#define HRTF 1

//
// Rule of thumb for figuring out ratio between UnrealUnits and meters:
//  * 1.80m ~ (CollisionHeight of average human)*2.
//

// Rune.
#if ENGINE_VERSION==005 || ENGINE_VERSION==107
	#define RUNE 1
	#define LEGACY_TIME
	#define DEFAULT_UNREALUNITS_PER_METER 47

// Rune Classic. Has major changes compared to Rune.
#elif ENGINE_VERSION==110
	#define RUNE_CLASSIC 1
	#define LEGACY_TIME
	#define DEFAULT_UNREALUNITS_PER_METER 47

// The Next Generation: Klingon Honor Guard.
#elif ENGINE_VERSION==219
	#define LEGACY_TIME
	#define LEGACY_DECLARE_CLASS
	#define LEGACY_STATIC_CONSTRUCTOR
	#define LEGACY_YELL_IF_UNICODE
	#define LEGACY_NEED_APPSLEEP
	#define LEGACY_NEED_FASTLINECHECK
	#define DEFAULT_UNREALUNITS_PER_METER 54
	#define USE_UNEXT

// Unreal 224.
#elif ENGINE_VERSION==224
	#define LEGACY_TIME
	#define LEGACY_DECLARE_CLASS
	#define LEGACY_NEED_FASTLINECHECK
	#define DEFAULT_UNREALUNITS_PER_METER 43
	#define USE_UNEXT
	#define DLL_EXPORT_CLASS	__declspec(dllexport)	/* Export class to DLL */

// Unreal 226.
#elif ENGINE_VERSION==226
	#define LEGACY_TIME
	#define DEFAULT_UNREALUNITS_PER_METER 43
	#define USE_UNEXT

// Unreal 227.
#elif ENGINE_VERSION==227
	#define DEFAULT_UNREALUNITS_PER_METER 43
	#define INCLUDE_AUDIOSUBSYSTEM_SETINSTANT
	//#define USE_LIPSYNCH - Marco: Currently implemented in Engine.

// Nerf Arena Blast.
#elif ENGINE_VERSION==300
	#define LEGACY_TIME
	#define LEGACY_DECLARE_CLASS
	#define DEFAULT_UNREALUNITS_PER_METER 55
	#define USE_UNEXT

// X-Com: Enforcer.
#elif defined(XCOM_ENFORCER)
	#pragma comment(lib,"..\\..\\Engine\\Lib\\binkw32.lib")
	#define DEFAULT_UNREALUNITS_PER_METER 43
	#define USE_UNEXT

// Cliff Barker's Undying.
#elif ENGINE_VERSION==420
	#define USE_UNEXT

// Unreal Tournament.
#elif ENGINE_VERSION>=430 && ENGINE_VERSION <= 500
	#define DEFAULT_UNREALUNITS_PER_METER 43
# if defined(_MSC_VER) && !defined(UNREAL_TOURNAMENT_UTPG) 
	#define LEGACY_TIME
# endif

// Brother Bear.
#elif ENGINE_VERSION==433
	#define USE_UNEXT

// Deus Ex.
#elif ENGINE_VERSION==1100
	#define LEGACY_TIME
	#define LEGACY_DECLARE_CLASS
	#define DEFAULT_UNREALUNITS_PER_METER 53
	#define INCLUDE_AUDIOSUBSYSTEM_SETINSTANT
	#define USE_UNEXT
	#define USE_LIPSYNCH
#endif

//
// Possible Unicode/Non-Unicode overview: (Galaxy/ALAudio/Miles export table)
//
// PlaySoundW Rune, Nerf, DeusEx, WOT, VRND, UT
// PlaySoundA KHG, Undying, DS9
// PlaySound  BB
//

// Include timeGetTime() support for anything but 227 (and for now which doesn't have FTime)
#if ENGINE_VERSION!=227 && defined(LEGACY_TIME) && !defined(UNREAL_TOURNAMENT)
	typedef DOUBLE FTIME;
	inline DOUBLE appSecondsAudio()
	{
		static UBOOL UseTimeGetTime = ParseParam(appCmdLine(),TEXT("FORCETGT"));
		if ( UseTimeGetTime )
			return timeGetTime() * 0.001;
		else if( GTimestamp )
		{
			DWORD L,H;
			__asm
			{
				xor   eax,eax	// Required so that VC++ realizes EAX is modified.
				xor   edx,edx	// Required so that VC++ realizes EDX is modified.
				_emit 0x0F		// RDTSC  -  Pentium+ time stamp register to EDX:EAX.
				_emit 0x31		// Use only 32 bits in EAX - even a Ghz cpu would have a 4+ sec period.
				mov   [L],eax   // Save low value.
				mov   [H],edx   // Save high value.
			}
			return ((DOUBLE)L +  4294967296.0 * (DOUBLE)H) * GSecondsPerCycle;
		}
		else
			return appSecondsSlow().GetFloat();
	}
// Wrapper for DOUBLE/FTime.
#elif defined(LEGACY_TIME)
	typedef DOUBLE FTIME;
	#define appSecondsAudio appSeconds
#elif defined(UNREAL_TOURNAMENT_UTPG)
	typedef DOUBLE FTIME;
	#define appSecondsAudio appSecondsNew
#else
	typedef FTime FTIME;
	#define appSecondsAudio appSeconds().GetFloat
#endif

// Wrapper for 3 and 4 parameter versions of DECLARE_CLASS macro.
#ifdef LEGACY_DECLARE_CLASS
	#define DECLARE_CLASS_AUDIO( TClass, TSuperClass, TStaticFlags, TPackage ) \
		DECLARE_CLASS( TClass, TSuperClass, TStaticFlags )
#else
	#define DECLARE_CLASS_AUDIO( TClass, TSuperClass, TStaticFlags, TPackage ) \
		DECLARE_CLASS( TClass, TSuperClass, TStaticFlags, TPackage )
#endif

// StaticConstrutor() in very old engine versions was called InternalClassInitializer().
#ifdef LEGACY_INTERNAL_CLASS_INITIALIZER
	#define LEGACY_STATIC_CONSTRUCTOR
	#define StaticConstrutor InternalClassInitializer
#endif

// Wrapper for static StaticConstrutor.
// void StaticConstructor() vs. static void StaticConstructor( UClass* Class )
#ifdef LEGACY_STATIC_CONSTRUCTOR
	#define SC_PARAM	UClass* Class
	#define SC_MODIFIER	static
	#define SC_GETCLASS	Class
#else
	#define SC_PARAM
	#define SC_MODIFIER
	#define SC_GETCLASS	GetClass()
#endif

#if ENGINE_VERSION!=227
	#ifdef _WIN32
		#define GCC_ALIGN(n)
	#endif
#endif

// Workaround for different FOutputDevice situation for KHG.
#if ENGINE_VERSION==219
	#define AudioLog GOut.Logf
#else
	#define AudioLog GLog->Logf
#endif

// Wrapper around potential missing UModel::FastLineCheck()
#if defined(LEGACY_NEED_FASTLINECHECK)
BYTE LineCheckInner( INT iNode, FVector End, FVector Start, BYTE Outside );
extern FBspNode* GLineCheckNodes;
inline BYTE FastLineCheck( UModel* M, FVector End, FVector Start )
{
	guard(FastLineCheck);
	GLineCheckNodes = &M->Nodes(0);
	return M->Nodes.Num() ? LineCheckInner(0,End,Start,M->RootOutside) : M->RootOutside;
	unguard;
}
#else
inline BYTE FastLineCheck( UModel* M, FVector End, FVector Start )
{
#if ENGINE_VERSION==227
	return M->FastLineCheck( End, Start, NULL );
#else
	return M->FastLineCheck( End, Start );
#endif
}
#endif

//
// 227 additions.
//
#if ENGINE_VERSION!=227
	void appStripBadChrs( TCHAR* T );
#endif

// Keep in mind this is a relative compare, not an absolute one!, thus approximatelyEqualFloat(100,99,0.5) yields true!
#if ENGINE_VERSION!=227 || ENGINE_SUBVERSION<10
	#include <math.h> // Note: MSVC6 does not have a <math> header. Maybe compiler version checks required.
	inline UBOOL approximatelyEqualFloat( FLOAT a, FLOAT b, FLOAT rel )
	{
		// return fabs(a-b)<=Max(fabs(a),fabs(b)*rel;
		return fabs(a-b) <= ((fabs(a)<fabs(b) ? fabs(b) : fabs(a)) * rel);
	}
#endif

// use ALURE or ALUT
#define ALURE

// Whether to use the ALC_EXT_EFX extension.
#define EFX

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
