/*=============================================================================
	UnVcWin32.h: Unreal definitions for Visual C++ SP2 running under Win32.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

/*----------------------------------------------------------------------------
	Platform compiler definitions.
----------------------------------------------------------------------------*/

#define __WIN32__				1
#define __INTEL__				1
#define __INTEL_BYTE_ORDER__	1

/*----------------------------------------------------------------------------
	Platform specifics types and defines.
----------------------------------------------------------------------------*/


// Undo any Windows defines.
#undef BYTE
#undef WORD
#undef DWORD
#undef INT
#undef FLOAT
#undef MAXBYTE
#undef MAXWORD
#undef MAXDWORD
#undef MAXINT
#undef CDECL

#include <stdio.h>
#include <intrin.h>
#pragma warning(disable:4996)

// Make sure HANDLE is defined.
#ifndef _WINDOWS_
	#define HANDLE void*
	#define HINSTANCE void*
#endif

// Sizes.
enum {DEFAULT_ALIGNMENT = 8 }; // Default boundary to align memory allocations on.
enum {CACHE_LINE_SIZE   = 32}; // Cache line size.

// Optimization macros (preceeded by #pragma).
#define DISABLE_OPTIMIZATION optimize("",off)
#define ENABLE_OPTIMIZATION  optimize("",on)

// Function type macros.
#ifdef __STATIC_LINK
#define DLL_IMPORT
#define DLL_EXPORT
#define DLL_EXPORT_CLASS
#else
#define DLL_IMPORT	__declspec(dllimport)	/* Import function from DLL */
#define DLL_EXPORT  __declspec(dllexport)	/* Export function to DLL */
#define DLL_EXPORT_CLASS	__declspec(dllexport)	/* Export class to DLL */
#endif

#define VARARGS     __cdecl					/* Functions with variable arguments */
#define CDECL	    __cdecl					/* Standard C function */
#define STDCALL		__stdcall				/* Standard calling convention */
#define FASTCALL	__fastcall
#define FORCEINLINE __forceinline			/* Force code to be inline */
#define ZEROARRAY                           /* Zero-length arrays in structs */

// Variable arguments.
#define GET_VARARGS(msg,len,Fmt)	\
{	\
	va_list ArgPtr; \
	va_start( ArgPtr, Fmt ); \
	_vsnwprintf( msg, len, Fmt, ArgPtr ); \
	va_end( ArgPtr ); \
}
#define GET_VARARGS_RESULT(msg, len, Fmt,result)	\
{	\
    va_list ArgPtr;	\
    va_start( ArgPtr, Fmt );	\
    result = _vsnwprintf( msg, len, Fmt, ArgPtr );	\
    va_end( ArgPtr );	\
}

// Compiler name.
#ifdef _DEBUG
	#define COMPILER "Compiled with Visual C++ Debug"
#else
	#define COMPILER "Compiled with Visual C++"
#endif

// Unsigned base types.
typedef unsigned char		BYTE;		// 8-bit  unsigned.
typedef unsigned short		_WORD;		// 16-bit unsigned.
typedef unsigned long		DWORD;		// 32-bit unsigned.
typedef unsigned int        UINT;		// unsigned int
typedef unsigned __int64	QWORD;		// 64-bit unsigned.

// Signed base types.
typedef	signed char			SBYTE;		// 8-bit  signed.
typedef signed short		SWORD;		// 16-bit signed.
typedef signed int  		INT;		// 32-bit signed.
typedef signed __int64		SQWORD;		// 64-bit signed.

// Character types.
typedef char				ANSICHAR;	// An ANSI character.
//typedef unsigned short      UNICHAR;	// A unicode character. Changed for proper unicode support Smirftsch
typedef wchar_t				UNICHAR;	// A unicode character.
typedef unsigned char		ANSICHARU;	// An ANSI character.
// typedef unsigned short      UNICHARU;	// A unicode character.
typedef __nullterminated	char *NPSTR, *LPSTR, *PSTR;
typedef __nullterminated	wchar_t *NWPSTR, *LPWSTR, *PWSTR;
typedef						LPWSTR PTSTR, LPTSTR;

// Other base types.
typedef signed int			UBOOL;		// Boolean 0 (false) or 1 (true).
typedef float				FLOAT;		// 32-bit IEEE floating point.
typedef double				DOUBLE;		// 64-bit IEEE double.
//typedef unsigned long       SIZE_T;     // Corresponds to C SIZE_T.

// Bitfield type.
// stijn: WARNING: the property offset calculation in UnClass and UnProp assumes that bitfields are 32-bit integers.
typedef unsigned long       BITFIELD;	// For bitfields.

#define FALSE 0
#define TRUE 1

#ifdef _WIN64
typedef unsigned __int64 PTRINT;
#else
typedef unsigned long PTRINT;
#endif

// !! Fixme: This is a workaround.
#define GCC_OPT_INLINE		inline

// Unwanted VC++ level 4 warnings to disable.
#pragma warning(disable : 4305) /* truncation from 'const double' to 'float'                            */
#pragma warning(disable : 4244) /* conversion to float, possible loss of data							*/
#pragma warning(disable : 4699) /* creating precompiled header											*/
#pragma warning(disable : 4200) /* Zero-length array item at end of structure, a VC-specific extension	*/
#pragma warning(disable : 4100) /* unreferenced formal parameter										*/
#pragma warning(disable : 4514) /* unreferenced inline function has been removed						*/
#pragma warning(disable : 4201) /* nonstandard extension used : nameless struct/union					*/
#pragma warning(disable : 4710) /* inline function not expanded											*/
#pragma warning(disable : 4702) /* unreachable code in inline expanded function							*/
#pragma warning(disable : 4711) /* function selected for autmatic inlining								*/
#pragma warning(disable : 4725) /* Pentium fdiv bug														*/
#pragma warning(disable : 4127) /* Conditional expression is constant									*/
#pragma warning(disable : 4512) /* assignment operator could not be generated                           */
#pragma warning(disable : 4530) /* C++ exception handler used, but unwind semantics are not enabled     */
#pragma warning(disable : 4245) /* conversion from 'enum ' to 'unsigned long', signed/unsigned mismatch */
#pragma warning(disable : 4305) /* truncation from 'const double' to 'float'                            */
#pragma warning(disable : 4238) /* nonstandard extension used : class rvalue used as lvalue             */
#pragma warning(disable : 4251) /* needs to have dll-interface to be used by clients of class 'ULinker' */
#pragma warning(disable : 4275) /* non dll-interface class used as base for dll-interface class         */
#pragma warning(disable : 4511) /* copy constructor could not be generated                              */
#pragma warning(disable : 4284) /* return type is not a UDT or reference to a UDT                       */
#pragma warning(disable : 4355) /* this used in base initializer list                                   */
#pragma warning(disable : 4097) /* typedef-name '' used as synonym for class-name ''                    */
#pragma warning(disable : 4291) /* typedef-name '' used as synonym for class-name ''                    */
#pragma warning(disable : 4731) /* frame pointer register 'ebp' modified by inline assembly code        */
#pragma warning(disable : 4458) /* declaration of '' hides class member									*/
#pragma warning(disable : 4297) /* '': function assumed not to throw an exception but does				*/
#pragma warning(disable : 4324) /* structure was padded due to alignment specifier						*/

// If C++ exception handling is disabled, force guarding to be off.
#ifndef _CPPUNWIND
	#error "Bad VCC option: C++ exception handling must be enabled"
#endif

// Make sure characters are unsigned.
#ifdef _CHAR_UNSIGNED
	#error "Bad VC++ option: Characters must be signed"
#endif

// No asm if not compiling for x86.
#ifndef _M_IX86
	#undef ASM
	#define ASM 0
#endif

// Strings.
#define LINE_TERMINATOR TEXT("\r\n")
#define PATH_SEPARATOR TEXT("\\")

// DLL file extension.
#define DLLEXT TEXT(".dll")

// Pathnames.
#define PATH(s) s

// NULL.
#define NULL 0

// Package implementation.
#if !__STATIC_LINK
#define IMPLEMENT_PACKAGE_PLATFORM(pkgname) \
	extern "C" {HINSTANCE hInstance;} \
	INT DLL_EXPORT STDCALL DllMain( HINSTANCE hInInstance, DWORD Reason, void* Reserved ) \
	{ hInstance = hInInstance; return 1; }
#else
#define IMPLEMENT_PACKAGE_PLATFORM(pkgname)
#endif

// Platform support options.
#define PLATFORM_NEEDS_ARRAY_NEW 1
#define FORCE_UTF8_LOG           1

// OS unicode function calling.
#if defined(NO_UNICODE_OS_SUPPORT) || !defined(UNICODE)
	#define TCHAR_CALL_OS(funcW,funcA) (funcA)
	#define TCHAR_TO_ANSI(str) str
	#define ANSI_TO_TCHAR(str) str
#elif defined(NO_ANSI_OS_SUPPORT)
	#define TCHAR_CALL_OS(funcW,funcA) (funcW)
	#define TCHAR_TO_ANSI(str) str
	#define ANSI_TO_TCHAR(str) str
#else
	CORE_API ANSICHAR* winToANSI( ANSICHAR* ACh, const UNICHAR* InUCh, INT Count );
	CORE_API INT winGetSizeANSI( const UNICHAR* InUCh );
	CORE_API UNICHAR* winToUNICODE( UNICHAR* Ch, const ANSICHAR* InUCh, INT Count );
	CORE_API INT winGetSizeUNICODE( const ANSICHAR* InACh );
	CORE_API LPSTR UnicodeToAscii(LPTSTR szString);
	CORE_API INT GetStringLengthW(LPTSTR szString);
	#define TCHAR_CALL_OS(funcW,funcA) (GUnicodeOS ? (funcW) : (funcA))
	#define TCHAR_TO_ANSI(str) winToANSI((ANSICHAR*)appAlloca(winGetSizeANSI(str)),str,winGetSizeANSI(str))
	#define TCHAR_TO_OEM(str) winToOEM((ANSICHAR*)appAlloca(winGetSizeANSI(str)),str,winGetSizeANSI(str))
	#define ANSI_TO_TCHAR(str) winToUNICODE((TCHAR*)appAlloca(winGetSizeUNICODE(str)*sizeof(TCHAR)),str,winGetSizeUNICODE(str))
#endif

// Bitfield alignment.
#define GCC_PACK(n)
#define GCC_ALIGN(n)

/*----------------------------------------------------------------------------
	Globals.
----------------------------------------------------------------------------*/

// System identification.
extern "C"
{
	extern HINSTANCE      hInstance;
    extern CORE_API UBOOL GIsAltivec;
	extern CORE_API UBOOL GIsMMX;
	extern CORE_API UBOOL GIsSSE;
	extern CORE_API UBOOL GIsSSE2;
	extern CORE_API UBOOL GIsSSE3;
	extern CORE_API UBOOL GIsSSSE3;
	extern CORE_API UBOOL GIsSSE41;
	extern CORE_API UBOOL GIsSSE42;
	extern CORE_API UBOOL GIsSSE5;
	extern CORE_API UBOOL GIsSSE4A;
	extern CORE_API UBOOL GIsA3DNOW;
	extern CORE_API UBOOL GIsMMXPLUS;
	extern CORE_API UBOOL GIsA3DNOWEXT;
	extern CORE_API UBOOL GTimestamp;			// stijn: is the TSC usable on this system?
	extern CORE_API UBOOL GTimestampPreferred;	// stijn: is TSC the preferred timing source?
}

/*----------------------------------------------------------------------------
	Functions.
----------------------------------------------------------------------------*/

//
// Round a floating point number to an integer.
// Note that (int+.5) is rounded to (int+1).
//
#if 0
#define DEFINED_appRound 1
inline INT appRound( FLOAT F )
{
	INT I;
	__asm fld [F]
	__asm fistp [I]
	return I;
}
#endif

//
// Converts to integer equal to or less than.
//
#if 0
#define DEFINED_appFloor 1
inline INT appFloor( FLOAT F )
{
	static FLOAT Half=0.5;
	INT I;
	__asm fld [F]
	__asm fsub [Half]
	__asm fistp [I]
	return I;
}
#endif

//
// Memory copy.
//
#if 0
#define DEFINED_appMemcpy
inline void appMemcpy( void* Dest, const void* Src, size_t Count )
{
	memcpy_optimized( Dest, Src, Count );
	/*
	__asm
	{
		mov		ecx, Count
		mov		esi, Src
		mov		edi, Dest
		mov     ebx, ecx
		shr     ecx, 2
		and     ebx, 3
		rep     movsd
		mov     ecx, ebx
		rep     movsb
	}
	*/
}
#endif

//
// Memory zero.
//
#if 0
#define DEFINED_appMemzero
inline void appMemzero( void* Dest, INT Count )
{
	__asm
	{
		mov		ecx, [Count]
		mov		edi, [Dest]
		xor     eax, eax
		mov		ebx, ecx
		shr		ecx, 2
		and		ebx, 3
		rep     stosd
		mov     ecx, ebx
		rep     stosb
	}
}
#endif

#if ASM3DNOW
inline void DoFemms()
{
	__asm _emit 0x0f
	__asm _emit 0x0e
}
#endif

#if ASM
#define DEFINED_appDebugBreak
inline void appDebugBreak()
{
	__asm
	{
		int 3
	}
}
#endif

//extern "C" void* __cdecl _alloca(size_t);
#define appAlloca(size) _alloca((size+7)&~7)
#define appAllocaType(type,arsize) reinterpret_cast<type*>(appAlloca(arsize*sizeof(type)))
#define appAllocaTypeZero(varname,type,arsize) appAllocaType(type,arsize); appMemzero(varname, arsize*sizeof(type))

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
