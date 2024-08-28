/*=============================================================================
	UnFile.h: General-purpose file utilities.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

#include <stdlib.h>
#include <math.h>

/*-----------------------------------------------------------------------------
	Global variables.
-----------------------------------------------------------------------------*/

// Global variables.
CORE_API extern DWORD GCRCTable[];

/*----------------------------------------------------------------------------
	Byte order conversion.
----------------------------------------------------------------------------*/

// Bitfields.
#ifndef NEXT_BITFIELD
	#if __INTEL_BYTE_ORDER__
		#define NEXT_BITFIELD(b) ((b)<<1)
		#define FIRST_BITFIELD   (1)
	#else
		#define NEXT_BITFIELD(b) ((b)>>1)
		#define FIRST_BITFIELD   (0x80000000)
	#endif
#endif

#if __INTEL_BYTE_ORDER__
	#define INTEL_ORDER16(x)   (x)
	#define INTEL_ORDER32(x)   (x)
	#define INTEL_ORDER64(x)   (x)
#else

    // These macros are not safe to use unless data is UNSIGNED!
	#define INTEL_ORDER16_unsigned(x)   ((((x)>>8)&0xff)+ (((x)<<8)&0xff00))
	#define INTEL_ORDER32_unsigned(x)   (((x)>>24) + (((x)>>8)&0xff00) + (((x)<<8)&0xff0000) + ((x)<<24))

    static inline _WORD INTEL_ORDER16(_WORD val)
    {
        #if MACOSXPPC
        register _WORD retval;
        __asm__ volatile("lhbrx %0,0,%1"
                : "=r" (retval)
                : "r" (&val)
                );
        return retval;
        #else
        return(INTEL_ORDER16_unsigned(val));
        #endif
    }

    static inline SWORD INTEL_ORDER16(SWORD val)
    {
        _WORD uval = *((_WORD *) &val);
        uval = INTEL_ORDER16(uval);
        return( *((SWORD *) &uval) );
    }

    static inline DWORD INTEL_ORDER32(DWORD val)
    {
        #if MACOSXPPC
        register DWORD retval;
        __asm__ __volatile__ (
            "lwbrx %0,0,%1"
                : "=r" (retval)
                : "r" (&val)
        );
        return retval;
        #else
        return(INTEL_ORDER32_unsigned(val));
        #endif
    }

    static inline INT INTEL_ORDER32(INT val)
    {
        DWORD uval = *((DWORD *) &val);
        uval = INTEL_ORDER32(uval);
        return( *((INT *) &uval) );
    }

	static inline QWORD INTEL_ORDER64(QWORD x)
	{
		/* Separate into high and low 32-bit values and swap them */
		DWORD l = (DWORD) (x & 0xFFFFFFFF);
		DWORD h = (DWORD) ((x >> 32) & 0xFFFFFFFF);
	    return( (((QWORD) (INTEL_ORDER32(l))) << 32) |
		         ((QWORD) (INTEL_ORDER32(h))) );
	}
#endif

/*-----------------------------------------------------------------------------
	Stats.
-----------------------------------------------------------------------------*/

#if STATS
	#define STAT(x) x
#else
	#define STAT(x)
#endif

/*-----------------------------------------------------------------------------
	Global init and exit.
-----------------------------------------------------------------------------*/

CORE_API void appInit( const TCHAR* InPackage, const TCHAR* InCmdLine, FMalloc* InMalloc, FOutputDevice* InLog, FOutputDeviceError* InError, FFeedbackContext* InWarn, FFileManager* InFileManager, FConfigCache*(*ConfigFactory)(), UBOOL RequireConfig );
CORE_API void appPreExit();
CORE_API void appExit();

/*-----------------------------------------------------------------------------
	Logging and critical errors.
-----------------------------------------------------------------------------*/

CORE_API void appRequestExit( UBOOL Force, FString Msg );

CORE_API void appFailAssert( const ANSICHAR* Expr, const ANSICHAR* File, INT Line, UBOOL bFatal );
CORE_API void VARARGS appFailAssertf(const ANSICHAR* Expr, const ANSICHAR* File, INT Line, UBOOL bFatal, const TCHAR* Fmt, ...);
CORE_API void VARARGS appUnwindf( const TCHAR* Fmt, ... );
CORE_API const TCHAR* appGetSystemErrorMessage( INT Error=0 );
CORE_API const void appDebugMessagef( const TCHAR* Fmt, ... );
CORE_API const void appMsgf( const TCHAR* Fmt, ... );
CORE_API const void appGetLastError( void );

#define debugf				GLog->Logf
#define appErrorf			GError->Logf
#define warnf				GWarn->Logf

#if DO_GUARD_SLOW
	#define debugfSlowCond(cond, ...) if(cond) GLog->Logf(__VA_ARGS__)
	#define debugfSlow(...)		GLog->Logf(__VA_ARGS__)
	#define appErrorfSlow(...)	GError->Logf(__VA_ARGS__)
#else
	#define debugfSlowCond(cond, ...)
	#define debugfSlow(...)
	#define appErrorfSlow(...)
#endif

/*-----------------------------------------------------------------------------
	Misc.
-----------------------------------------------------------------------------*/

CORE_API void* appGetDllHandle( const TCHAR* DllName );
CORE_API void appFreeDllHandle( void* DllHandle );
CORE_API void* appGetDllExport( void* DllHandle, const TCHAR* ExportName );
CORE_API void appLaunchURL( const TCHAR* URL, const TCHAR* Parms=NULL, FString* Error=NULL );
CORE_API void* appCreateProc( const TCHAR* URL, const TCHAR* Parms , UBOOL bRealTime );
CORE_API UBOOL appGetProcReturnCode( void* ProcHandle, INT* ReturnCode );
CORE_API void appEnableFastMath( UBOOL Enable );
CORE_API class FGuid appCreateGuid();
CORE_API void appCreateTempFilename( const TCHAR* Path, TCHAR* Result256 );
CORE_API UBOOL appCheckValidCmd( const TCHAR* URL );
CORE_API void appCleanFileCache();
CORE_API UBOOL appFindPackageFile(const TCHAR* In, const FGuid* Guid, FString& Out, UBOOL bNoCacheSearch = FALSE);
CORE_API void * memcpy_optimized(void *dest, const void *src, size_t n);
CORE_API void* memset_optimized(void *dest, int c, size_t n);
CORE_API void memzero_optimized(void *dest, size_t n);

/*-----------------------------------------------------------------------------
	Clipboard.
-----------------------------------------------------------------------------*/

CORE_API void appClipboardCopy( const TCHAR* Str );
CORE_API FString appClipboardPaste();

/*-----------------------------------------------------------------------------
	Timing functions.
-----------------------------------------------------------------------------*/

CORE_API FTime appSeconds();
CORE_API FTime appSecondsSlow();
CORE_API QWORD appCycles();
CORE_API QWORD appGetTime( const TCHAR* Filename );
CORE_API void appSystemTime( INT& Year, INT& Month, INT& DayOfWeek, INT& Day, INT& Hour, INT& Min, INT& Sec, INT& MSec );
CORE_API FString appTimestamp();
CORE_API void appSleep( FLOAT Seconds );
CORE_API QWORD CurrentTime();
class FHungChecker;
#include "UnDebugCPUClock.h"

/*-----------------------------------------------------------------------------
	Guard macros for call stack display.
-----------------------------------------------------------------------------*/

//
// guard/unguardf/unguard macros.
// For showing calling stack when errors occur in major functions.
// Meant to be enabled in release builds.
//
#ifndef _MSC_VER
#define MAX_RYANS_HACKY_GUARD_BLOCKS 8192

class FString;

// stijn: old version. Was not thread-safe.
class UnGuardBlock
{
public:
	static void GetBackTrace(FString& str);
private:
	static INT GuardIndex;
	static const TCHAR* GuardTexts[MAX_RYANS_HACKY_GUARD_BLOCKS];
};

// stijn: new version. thread-safe.
class UnGuardBlockTLSData
{
public:
	INT GuardIndex;
	const TCHAR* GuardTexts[MAX_RYANS_HACKY_GUARD_BLOCKS];
};

class UnGuardBlockTLS
{
public:
	UnGuardBlockTLS(const TCHAR* text)
	{
		if (++Data.GuardIndex < MAX_RYANS_HACKY_GUARD_BLOCKS)
			Data.GuardTexts[Data.GuardIndex] = text;
	}

	~UnGuardBlockTLS(void)
	{
		Data.GuardIndex--;
	}

	static const TCHAR* GetFuncName()
	{
		return (Data.GuardIndex >= 0 && Data.GuardIndex < MAX_RYANS_HACKY_GUARD_BLOCKS) ?
			Data.GuardTexts[Data.GuardIndex] : TEXT("");
	}

	static void GetBackTrace(FString& str);

private:
	static thread_local UnGuardBlockTLSData Data;
};

#endif

#if !DO_GUARD
# define guard(func)			{
# define unguard				}
# define unguardf(msg)		    }
# define guard_nofunc			{
# define unguardf_nofunc(msg)	}
#elif WIN32
# define guard(func)			{static const TCHAR __FUNC_NAME__[]=TEXT(#func); try{
# define unguard				}catch(TCHAR*Err){throw Err;}catch(...){appUnwindf(TEXT("%ls"),__FUNC_NAME__); throw;}}
# define unguardf(msg)		    }catch(TCHAR*Err){throw Err;}catch(...){appUnwindf(TEXT("%ls"),__FUNC_NAME__); appUnwindf msg; throw;}}
# define guard_nofunc			{try{
# define unguardf_nofunc(msg)	}catch(TCHAR*Err){throw Err;}catch(...){appUnwindf msg; throw;}}
#else
// stijn: C++ catch blocks cannot catch segfaults on nix. Ryan's guard blocks
// are probably the cleanest and most portable way to create a backtrace...
# define guard(func)			{UnGuardBlockTLS __FUNC_NAME__(TEXT(#func)); try{
# define unguard				}catch(TCHAR*Err){throw Err;}catch(...){appUnwindf(TEXT("%ls"),UnGuardBlockTLS::GetFuncName()); throw;}}
# define unguardf(msg)		    }catch(TCHAR*Err){throw Err;}catch(...){appUnwindf(TEXT("%ls"),UnGuardBlockTLS::GetFuncName()); appUnwindf msg; throw;}}
# define guard_nofunc			{try{
# define unguardf_nofunc(msg)	}catch(TCHAR*Err){throw Err;}catch(...){appUnwindf msg; throw;}}
#endif

//
// guardSlow/unguardfSlow/unguardSlow macros.
// For showing calling stack when errors occur in performance-critical functions.
// Meant to be disabled in release builds.
//
#if defined(_DEBUG) || !DO_GUARD || !DO_GUARD_SLOW
	#define guardSlow(func)		{
	#define unguardfSlow(msg)	}
	#define unguardSlow			}
	#define unguardfSlow(msg)	}
#else
	#define guardSlow(func)		guard(func)
	#define unguardSlow			unguard
	#define unguardfSlow(msg)	unguardf(msg)
#endif

//
// For throwing string-exceptions which safely propagate through guard/unguard.
//
CORE_API void VARARGS appThrowf( const TCHAR* Fmt, ... );

/*-----------------------------------------------------------------------------
	Check macros for assertions.
-----------------------------------------------------------------------------*/

//
// "check" expressions are only evaluated if enabled.
// "verify" expressions are always evaluated, but only cause an error if enabled.
//
#if DO_CHECK
	#define check(expr)			{if(!(expr)) appFailAssert( #expr, __FILE__, __LINE__, FALSE );}
	#define verify(expr)		{if(!(expr)) appFailAssert( #expr, __FILE__, __LINE__, TRUE );}
	#define checkf(expr, ...)	{if(!(expr)) appFailAssertf( #expr, __FILE__, __LINE__, FALSE, __VA_ARGS__);}
	#define verifyf(expr, ...)	{if(!(expr)) appFailAssertf( #expr, __FILE__, __LINE__, TRUE, __VA_ARGS__);}
#else
	#define check(expr)
	#define verify(expr) expr
	#define checkf(expr, ...)
	#define verifyf(expr, ...) expr
#endif

//
// Check for development only.
//
#if DO_GUARD_SLOW
	#define checkSlow(expr)  {if(!(expr)) appFailAssert( #expr, __FILE__, __LINE__, FALSE );}
	#define verifySlow(expr) {if(!(expr)) appFailAssert( #expr, __FILE__, __LINE__, TRUE );}
	#define checkfSlow(expr, ...)	{if(!(expr)) appFailAssertf( #expr, __FILE__, __LINE__, FALSE, __VA_ARGS__);}
	#define verifyfSlow(expr, ...)	{if(!(expr)) appFailAssertf( #expr, __FILE__, __LINE__, TRUE, __VA_ARGS__);}
#else
	#define checkSlow(expr)
	#define verifySlow(expr)
	#define checkfSlow(...)
	#define verifyfSlow(...)
#endif

/*-----------------------------------------------------------------------------
	Timing macros.
-----------------------------------------------------------------------------*/

//
// Normal timing.
//

// stijn: someone originally gave this the same name as a standard C89/99/POSIX
// function.  This was a BAD idea and caused a _LOT_ of problems.
#define clockFast(Timer)   {Timer -= appCycles();}
#define unclockFast(Timer) {Timer += appCycles()-34;}

//
// Performance critical timing.
//
#if DO_CLOCK_SLOW
	#define clockSlow(Timer)   {Timer-=appCycles();}
	#define unclockSlow(Timer) {Timer+=appCycles();}
#else
	#define clockSlow(Timer)
	#define unclockSlow(Timer)
#endif

/*-----------------------------------------------------------------------------
	Text format.
-----------------------------------------------------------------------------*/

CORE_API FString appFormat( FString Src, const TMultiMap<FString,FString>& Map );

/*-----------------------------------------------------------------------------
	Localization.
-----------------------------------------------------------------------------*/

class CORE_API FLocalizeFileCache
{
public:
	FLocalizeFileCache();
	~FLocalizeFileCache();

	// Find an actual language file by package name, returns false if failed.
	static UBOOL FindLocaleFile(FString& Result, const TCHAR* PackageName, const TCHAR* LangExt = NULL);

	// Grab list of all language files.
	static void GetAllLocaleFiles(TArray<FString>& Result, const TCHAR* LangExt = NULL);

	// Find list of all language Core files.
	static void FindAllLanguages(TArray<FString>& Result);

	// Check if valid language.
	static UBOOL ValidLanguage(const TCHAR* LangExt);

	// Create new language file if doesn't excist, and return filepath of it.
	static FString CreateLangFile(const TCHAR* PackageName, const TCHAR* LangExt = NULL);

	// Lang path parsing.
	static FString GetLangPathWildcard(const TCHAR* InPath, const TCHAR* LangID); // Turn i.e: ../SystemLocalized/<lang>/*.<lang> -> ../SystemLocalized/int/*.int
	static FString GetLangPath(const TCHAR* InPath, const TCHAR* LangID, const TCHAR* Filename = NULL); // Turn i.e: ../SystemLocalized/<lang>/*.<lang> -> ../SystemLocalized/int/ (OR with Filename specified: ../SystemLocalized/int/Core.int)
};

CORE_API FString Localize( const TCHAR* Section, const TCHAR* Key, const TCHAR* Package=GPackage, const TCHAR* LangExt=NULL, UBOOL Optional=0 );
CORE_API FString LocalizeError( const TCHAR* Key, const TCHAR* Package=GPackage, const TCHAR* LangExt=NULL );
CORE_API FString LocalizeProgress( const TCHAR* Key, const TCHAR* Package=GPackage, const TCHAR* LangExt=NULL );
CORE_API FString LocalizeQuery( const TCHAR* Key, const TCHAR* Package=GPackage, const TCHAR* LangExt=NULL );
CORE_API FString LocalizeGeneral( const TCHAR* Key, const TCHAR* Package=GPackage, const TCHAR* LangExt=NULL );

#if UNICODE
	CORE_API FString Localize( const ANSICHAR* Section, const ANSICHAR* Key, const TCHAR* Package=GPackage, const TCHAR* LangExt=NULL, UBOOL Optional=0 );
	CORE_API FString LocalizeError( const ANSICHAR* Key, const TCHAR* Package=GPackage, const TCHAR* LangExt=NULL );
	CORE_API FString LocalizeProgress( const ANSICHAR* Key, const TCHAR* Package=GPackage, const TCHAR* LangExt=NULL );
	CORE_API FString LocalizeQuery( const ANSICHAR* Key, const TCHAR* Package=GPackage, const TCHAR* LangExt=NULL );
	CORE_API FString LocalizeGeneral( const ANSICHAR* Key, const TCHAR* Package=GPackage, const TCHAR* LangExt=NULL );
#endif

/*-----------------------------------------------------------------------------
	File functions.
-----------------------------------------------------------------------------*/

// File utilities.
CORE_API const TCHAR* appFExt( const TCHAR* Filename );
CORE_API FString appFilePathName( const TCHAR* InName );
CORE_API FString appFileBaseName( const TCHAR* InName );
CORE_API UBOOL appUpdateFileModTime( const TCHAR* Filename );
CORE_API FString appGetGMTRef();
CORE_API UBOOL appChdirSystem(); // stijn: chdir into the System directory

/*-----------------------------------------------------------------------------
	OS functions.
-----------------------------------------------------------------------------*/

CORE_API const TCHAR* appCmdLine();
CORE_API const TCHAR* appBaseDir();
CORE_API const TCHAR* appPackage();
CORE_API const TCHAR* appPackageExe();
CORE_API const TCHAR* appComputerName();
CORE_API const TCHAR* GetMACAddress();
CORE_API const TCHAR* appUserName();


/*-----------------------------------------------------------------------------
	Character type functions.
-----------------------------------------------------------------------------*/

inline TCHAR appToUpper( TCHAR c )
{
	return (c<'a' || c>'z') ? (c) : (TCHAR)(c+'A'-'a');
}
inline TCHAR appToLower( TCHAR c )
{
	return (c<'A' || c>'Z') ? (c) : (TCHAR)(c+'a'-'A');
}
inline UBOOL appIsAlpha( TCHAR c )
{
	return (c>='a' && c<='z') || (c>='A' && c<='Z');
}
inline UBOOL appIsDigit( TCHAR c )
{
	return c>='0' && c<='9';
}
inline UBOOL appIsAlnum( TCHAR c )
{
	return (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9');
}
inline UBOOL appIsHexDigit(TCHAR c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

/*-----------------------------------------------------------------------------
	String functions.
-----------------------------------------------------------------------------*/

CORE_API ANSICHAR* appToAnsi( const TCHAR* Str );
CORE_API UNICHAR* appToUnicode( const TCHAR* Str );
CORE_API TCHAR* appFromAnsi( const ANSICHAR* Str );
CORE_API TCHAR* appFromUnicode( const UNICHAR* Str );
CORE_API size_t appToAnsiInPlace(ANSICHAR* Dst, const TCHAR* Src, size_t MaxCount);
CORE_API size_t appToUnicodeInPlace(UNICHAR* Dst, const TCHAR* Src, size_t MaxCount);
CORE_API size_t appFromAnsiInPlace(TCHAR* Dst, const ANSICHAR* Src, size_t MaxCount);
CORE_API size_t appFromUnicodeInPlace(TCHAR* Dst, const UNICHAR* Src, size_t MaxCount);
CORE_API size_t appToUtf8InPlace(ANSICHAR* Dst, const TCHAR* Src, size_t MaxCount);
CORE_API size_t appFromUtf8InPlace(TCHAR* Dst, const ANSICHAR* Src, size_t MaxCount);
CORE_API UBOOL appIsPureAnsi( const TCHAR* Str );
CORE_API UBOOL appIsPureAnsiOld( const TCHAR* Str );

inline TCHAR* appStrcpy( TCHAR* Dest, const TCHAR* Src )
{
#if UNICODE
	if (!Src)
		Src = TEXT("Invalid Input");
	return wcscpy(Dest, Src);
#else
	if (!Src)
		Src = TEXT("Invalid Input");
	return strcpy(Dest, Src);
#endif
}
inline INT appStrlen( const TCHAR* String )
{
	return static_cast<INT>(wcslen(String));
}

// Changes by Smirftsch for Unicode
inline const TCHAR* appStrstr(const TCHAR* String, const TCHAR* Find)
{
	return wcsstr(String, Find);
}
inline TCHAR* appStrstr(TCHAR* String, const TCHAR* Find)
{
	return wcsstr(String, Find);
}
inline const TCHAR* appStrchr(const TCHAR* String, INT c)
{
	return wcschr(String, c);
}
inline TCHAR* appStrchr(TCHAR* String, INT c)
{
	return wcschr(String, c);
}

inline TCHAR* appStrcat( TCHAR* Dest, const TCHAR* Src )
{
#if UNICODE
	return wcscat(Dest, Src);
#else
	return strcat(Dest, Src);
#endif
}
inline INT appStrcmp( const TCHAR* String1, const TCHAR* String2 )
{
#if UNICODE
	return wcscmp(String1, String2);
#else
	return strcmp(String1, String2);
#endif
}
inline INT appStricmp( const TCHAR* String1, const TCHAR* String2 )
{
#if _WIN32
#if UNICODE
	return _wcsicmp(String1, String2);
#else
	return stricmp(String1, String2);
#endif
#else
#if UNICODE
	return wcscasecmp(String1, String2);
#else
	return stricmp(String1, String2);
#endif
#endif
}
inline INT appStrncmp( const TCHAR* String1, const TCHAR* String2, INT Count )
{
#if UNICODE
	return wcsncmp(String1, String2, Count);
#else
	return strncmp(String1, String2, Count);
#endif
}
CORE_API TCHAR* appStaticString1024();
CORE_API TCHAR* appStaticString4096();
CORE_API TCHAR* appStaticString8192();
CORE_API ANSICHAR* appAnsiStaticString1024();
CORE_API ANSICHAR* appAnsiStaticString4096();
CORE_API ANSICHAR* appAnsiStaticString8192();

// stijn: added for 227. Dynamically allocates a string of the requested size
// in a string memory pool. The caller should _NOT_ free the string.
CORE_API ANSICHAR* appAnsiStaticString(size_t Len);
CORE_API TCHAR* appStaticString(size_t Len);

CORE_API const TCHAR* appSpc( INT Num );
CORE_API TCHAR* appStrncpy( TCHAR* Dest, const TCHAR* Src, INT Max);
inline TCHAR* appStrncat(TCHAR* Dest, const TCHAR* Src, INT MaxLen)
{
	return wcsncat(Dest, Src, MaxLen);
}

// uppercase.
CORE_API void appStrupr( TCHAR* String );
// lowercase
CORE_API void appStrlwr(TCHAR* String);

CORE_API const TCHAR* appStrfind(const TCHAR* Str, const TCHAR* Find);
CORE_API DWORD appStrCrc( const TCHAR* Data );
CORE_API DWORD appStrCrcCaps( const TCHAR* Data );
inline INT appAtoi( const TCHAR* Str )
{
#if _MSC_VER
#if UNICODE
	return _wtoi(Str);
#else
	return atoi(Str);
#endif
#else
#if UNICODE
	return wcstol(Str, NULL, 10);
	//return atoi( TCHAR_TO_ANSI(Str) );
#else
	return strtol(Str, NULL, 10);
	//return atoi( Str );
#endif
#endif
}
inline FLOAT appAtof( const TCHAR* Str )
{
#if _MSC_VER
#if UNICODE
	return _wtof(Str);
#else
	return atof(Str);
#endif
#else
	return atof(appToAnsi(Str));
#endif
}
CORE_API INT appStrtoi( const TCHAR* Start, TCHAR** End, INT Base );
CORE_API INT appStrnicmp( const TCHAR* A, const TCHAR* B, INT Count );
CORE_API INT appSprintf( TCHAR* Dest, const TCHAR* Fmt, ... );
CORE_API INT appSnprintf(TCHAR* Dest, INT Size, const TCHAR* Fmt, ...);
CORE_API void appTrimSpaces( ANSICHAR* String);
CORE_API void appStripSpaces( TCHAR* T );
CORE_API void appStripChars( TCHAR* T, const TCHAR* ClearChars );
CORE_API void appStripBadChrs( TCHAR* T );

#if _MSC_VER
	// stijn: later visual studios do not like it when you pass the format string argument by reference...
	// I changed this so the pointer is now passed by value instead.
	CORE_API INT appGetVarArgs( TCHAR* Dest, INT Count, const TCHAR* Fmt, ... );
#else
	#include <stdio.h>
	#include <stdarg.h>
	#if UNICODE
	#include <wchar.h>
	#include <wctype.h>
	#endif
#endif

typedef int QSORT_RETURN;
typedef QSORT_RETURN(CDECL* QSORT_COMPARE)( const void* A, const void* B );
CORE_API void appQsort( void* Base, INT Num, INT Width, QSORT_COMPARE Compare );

//
// Case insensitive string hash function.
//
inline DWORD appStrihash( const TCHAR* Data )
{
	DWORD Hash=0;
	while( *Data )
	{
		TCHAR Ch = appToUpper(*Data++);
		BYTE  B  = (BYTE)Ch;
		Hash     = ((Hash >> 8) & 0x00FFFFFF) ^ GCRCTable[(Hash ^ B) & 0x000000FF];
		B        = (BYTE)(Ch>>8);
		Hash     = ((Hash >> 8) & 0x00FFFFFF) ^ GCRCTable[(Hash ^ B) & 0x000000FF];
	}
	return Hash;
}

/*-----------------------------------------------------------------------------
	Parsing functions.
-----------------------------------------------------------------------------*/

CORE_API UBOOL ParseCommand( const TCHAR** Stream, const TCHAR* Match );
CORE_API UBOOL Parse( const TCHAR* Stream, const TCHAR* Match, class FName& Name );
CORE_API UBOOL Parse( const TCHAR* Stream, const TCHAR* Match, DWORD& Value );
CORE_API UBOOL Parse( const TCHAR* Stream, const TCHAR* Match, class FGuid& Guid );
CORE_API UBOOL Parse( const TCHAR* Stream, const TCHAR* Match, TCHAR* Value, INT MaxLen );
CORE_API UBOOL Parse( const TCHAR* Stream, const TCHAR* Match, BYTE& Value );
CORE_API UBOOL Parse( const TCHAR* Stream, const TCHAR* Match, SBYTE& Value );
CORE_API UBOOL Parse( const TCHAR* Stream, const TCHAR* Match, _WORD& Value );
CORE_API UBOOL Parse( const TCHAR* Stream, const TCHAR* Match, SWORD& Value );
CORE_API UBOOL Parse( const TCHAR* Stream, const TCHAR* Match, FLOAT& Value );
CORE_API UBOOL Parse( const TCHAR* Stream, const TCHAR* Match, INT& Value );
CORE_API UBOOL Parse( const TCHAR* Stream, const TCHAR* Match, FString& Value );
CORE_API UBOOL Parse( const TCHAR* Stream, const TCHAR* Match, QWORD& Value );
CORE_API UBOOL Parse( const TCHAR* Stream, const TCHAR* Match, SQWORD& Value );
CORE_API UBOOL ParseUBOOL( const TCHAR* Stream, const TCHAR* Match, UBOOL& OnOff );
CORE_API UBOOL ParseObject( const TCHAR* Stream, const TCHAR* Match, class UClass* Type, class UObject*& DestRes, class UObject* InParent );
CORE_API UBOOL ParseLine( const TCHAR** Stream, TCHAR* Result, INT MaxLen, UBOOL Exact=0 );
CORE_API UBOOL ParseLine( const TCHAR** Stream, FString& Resultd, UBOOL Exact=0 );
CORE_API UBOOL ParseToken( const TCHAR*& Str, TCHAR* Result, INT MaxLen, UBOOL UseEscape );
CORE_API UBOOL ParseToken( const TCHAR*& Str, FString& Arg, UBOOL UseEscape );
CORE_API FString ParseToken( const TCHAR*& Str, UBOOL UseEscape );
CORE_API void ParseNext( const TCHAR** Stream );
CORE_API UBOOL ParseParam( const TCHAR* Stream, const TCHAR* Param );
CORE_API UBOOL ParseNextProperty(const TCHAR** Stream, FString& PropName, INT& ArIndex, FString& PropValue);

/*-----------------------------------------------------------------------------
	Array functions.
-----------------------------------------------------------------------------*/

enum EStringEncoding
{
	STRENC_Auto, // Auto-detect best encoding for this string.
	STRENC_ANSI,
	STRENC_UTF8,
	STRENC_UTF16,
};

// Core functions depending on TArray and FString.
CORE_API UBOOL appLoadFileToArray( TArray<BYTE>& Result, const TCHAR* Filename, FFileManager* FileManager=GFileManager );
CORE_API UBOOL appLoadFileToString( FString& Result, const TCHAR* Filename, FFileManager* FileManager=GFileManager );
CORE_API UBOOL appSaveArrayToFile( const TArray<BYTE>& Array, const TCHAR* Filename, FFileManager* FileManager=GFileManager );
CORE_API UBOOL appSaveStringToFile(const FString& String, const TCHAR* Filename, FFileManager* FileManager = GFileManager, EStringEncoding Encoding = STRENC_Auto);

/*-----------------------------------------------------------------------------
	Memory functions.
-----------------------------------------------------------------------------*/

inline void* appMemmove(void* Dest, const void* Src, size_t Count)
{
	return memmove(Dest, Src, Count);
}
inline INT appMemcmp(const void* Buf1, const void* Buf2, size_t Count)
{
	return memcmp(Buf1, Buf2, Count);
}
inline UBOOL appMemIsZero(const void* V, size_t Count)
{
	guardSlow(appMemIsZero);
	BYTE* B = (BYTE*)V;
	while (Count-- > 0)
		if (*B++ != 0)
			return 0;
	return 1;
	unguardSlow;
}
CORE_API DWORD appMemCrc( const void* Data, INT Length, DWORD CRC=0 );

inline void appMemset(void* Dest, INT C, size_t Count)
{
	//memset_optimized( Dest, C, Count );
	memset(Dest, C, Count);
}

#ifndef DEFINED_appMemcpy
inline void appMemcpy(void* Dest, const void* Src, size_t Count)
{
	//memcpy_optimized( Dest, Src, Count );
	memcpy(Dest, Src, Count);
}
#endif

#ifndef DEFINED_appMemzero
inline void appMemzero(void* Dest, size_t Count)
{
	//memzero_optimized(Dest,Count);
	memset(Dest, 0, Count);
}
#endif

inline void appMemswap(void* Ptr1, void* Ptr2, DWORD Size)
{
	void* Temp = appAlloca(Size);
	appMemcpy(Temp, Ptr1, Size);
	appMemcpy(Ptr1, Ptr2, Size);
	appMemcpy(Ptr2, Temp, Size);
}

//
// C style memory allocation stubs.
//
#define appMalloc     GMalloc->Malloc
#define appFree       GMalloc->Free
#define appRealloc    GMalloc->Realloc

#ifdef _MSC_VER  // turn off "operator new may not be declared inline"
#pragma warning( disable : 4595 )
#endif

//
// C++ style memory allocation.
//
#if ASAN
#define NewTagged(Tag) new
#else
#define NewTagged(Tag) new(Tag)
#endif

#if !defined(_DEBUG)
__forceinline void* operator new(size_t Size, const TCHAR* Tag)
{
	guardSlow(new);
	return appMalloc(Size, Tag);
	unguardSlow;
}
__forceinline void* operator new(size_t Size)
{
	guardSlow(new);
	return appMalloc(Size, TEXT("new"));
	unguardSlow;
}
__forceinline void operator delete(void* Ptr) throw()
{
	guardSlow(delete);
	appFree(Ptr);
	unguardSlow;
}
#endif

#if !defined(_DEBUG)
# if PLATFORM_NEEDS_ARRAY_NEW
__forceinline void* operator new[](size_t Size, const TCHAR* Tag)
{
	guardSlow(new);
	return appMalloc(Size, Tag);
	unguardSlow;
}
__forceinline void* operator new[](size_t Size)
{
	guardSlow(new);
	return appMalloc(Size, TEXT("new"));
	unguardSlow;
}
__forceinline void operator delete[](void* Ptr) throw()
{
	guardSlow(delete);
	appFree(Ptr);
	unguardSlow;
}
# endif
#endif

/*-----------------------------------------------------------------------------
	Math.
-----------------------------------------------------------------------------*/

CORE_API BYTE appCeilLogTwo( DWORD Arg );

#ifndef DEFINED_appFloorLogTwo
inline BYTE appFloorLogTwo(DWORD Arg)
{
	INT Bit = 32;
	while (--Bit >= 0)
		if (Arg & (1u << DWORD(Bit)))
			return Bit;

	// lg(0) is undefined, so we can't solve.
	return 0;
}
#endif

/*-----------------------------------------------------------------------------
	MD5 functions.
-----------------------------------------------------------------------------*/

//
// MD5 Context.
//
struct FMD5Context
{
	DWORD state[4];
	DWORD count[2];
	BYTE buffer[64];
};

//
// MD5 functions.
//!!it would be cool if these were implemented as subclasses of
// FArchive.
//
CORE_API void appMD5Init( FMD5Context* context );
CORE_API void appMD5Update( FMD5Context* context, BYTE* input, INT inputLen );
CORE_API void appMD5Final( BYTE* digest, FMD5Context* context );
CORE_API void appMD5Transform( DWORD* state, BYTE* block );
CORE_API void appMD5Encode( BYTE* output, DWORD* input, INT len );
CORE_API void appMD5Decode( DWORD* output, BYTE* input, INT len );
CORE_API FString appMD5String( FString InString );

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
