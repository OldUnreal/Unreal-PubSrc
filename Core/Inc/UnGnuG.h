/*=============================================================================
	UnGnuG.h: Unreal definitions for Gnu G++. Unfinished. Unsupported.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	* Fast appRound, appFloor on GNUC/x86 added by Michael Vance.
=============================================================================*/

/*----------------------------------------------------------------------------
	Platform compiler definitions.
----------------------------------------------------------------------------*/
//__LP64__ || __x86_64__ || __powerpc64__ || __aarch64__ || __ARM_ARCH_8__ || _M_X64


#ifdef __LINUX_X86__
	#define __LINUX__ 1
	#define __INTEL__ 1
	#define __INTEL_BYTE_ORDER__ 1
	#undef ASM
	#undef ASM3DNOW
	#undef ASMKNI
    #undef ASMLINUX
    #ifdef __clang__
        #define COMPILER "Compiled with Linux LLVM clang++(" __VERSION__ ")"
    #else
        #define COMPILER "Compiled with GNU g++ (" __VERSION__ ")"
    #endif
#elif __LINUX__X86_64__
    #define BUILD_64 1
	#define __LINUX__ 1
	#define __INTEL__ 1
	#define __INTEL_BYTE_ORDER__ 1
	#undef ASM
	#undef ASM3DNOW
	#undef ASMKNI
    #undef ASMLINUX
    #ifdef __clang__
        #define COMPILER "Compiled with Linux LLVM clang++(" __VERSION__ ")"
    #else
        #define COMPILER "Compiled with GNU g++ (" __VERSION__ ")"
    #endif
#elif __LINUX_ARM__
	#define __LINUX__ 1
	#define __INTEL_BYTE_ORDER__ 1
	#undef ASM
	#undef ASM3DNOW
	#undef ASMKNI
    #undef ASMLINUX
    #ifdef __clang__
        #define COMPILER "Compiled with Linux LLVM clang++(" __VERSION__ ")"
    #else
        #define COMPILER "Compiled with GNU g++ (" __VERSION__ ")"
    #endif
#elif __LINUX_ARM_64__
    #define BUILD_64
	#define __LINUX__ 1
	#define __INTEL_BYTE_ORDER__ 1
	#undef ASM
	#undef ASM3DNOW
	#undef ASMKNI
    #undef ASMLINUX
    #ifdef __clang__
        #define COMPILER "Compiled with Linux LLVM clang++(" __VERSION__ ")"
    #else
        #define COMPILER "Compiled with GNU g++ (" __VERSION__ ")"
    #endif
#elif __APPLE__
	#define __UNIX__  1
	#define __INTEL__ 1
	#define __INTEL_BYTE_ORDER__ 1
# if !__APPLE_ARM64__
	#define __UNREAL_X86__ 1
# endif
    #undef ASM
	#define COMPILER "Compiled with macOS Clang (" __VERSION__ ")"

    #if __LITTLE_ENDIAN__
    #define WCHAR_T_ENCODING_STRING "UCS-4LE"
    #else
    #define WCHAR_T_ENCODING_STRING "UCS-4BE"
    #endif

#elif __PSX2_EE__
	#define __LINUX__ 1
	#define __INTEL__ 1
	#define __INTEL_BYTE_ORDER__ 1
	#undef ASM
	#undef ASM3DNOW
	#undef ASMKNI
	#undef ASMLINUX
	#define ASMPSX2 1
	#define COMPILER "Compiled with PSX2-EE g++ ("__VERSION__")"
#elif __EMSCRIPTEN__
	#define __UNIX__ 1
	#define __LINUX__ 1
	#define __INTEL__ 1
	#define __INTEL_BYTE_ORDER__ 1
	#undef ASM
	#undef ASMLINUX
	#define ASMPSX2 1
	#define COMPILER "Compiled with Emscripten (" __VERSION__ ")"
#else
	#error Unsupported platform.
#endif

#include <math.h>    // floor
#include <stdlib.h>  // alloca
#include <errno.h>
#include <pthread.h> // needed for pthread_create()
#include <map>
#include <inttypes.h>

#ifndef WCHAR_T_ENCODING_STRING
#define WCHAR_T_ENCODING_STRING "WCHAR_T"
#endif


#if UNICODE
#include <wchar.h>
#include <wctype.h>
#endif

// Work around gcc/clang differences.
#ifndef __clang__
    #define _THROW_BAD_ALLOC _GLIBCXX_THROW (std::bad_alloc)
    #define _NOEXCEPT _GLIBCXX_USE_NOEXCEPT
#endif
#define __forceinline inline __attribute__((always_inline))


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
#undef VOID
#undef CDECL

// Make sure HANDLE is defined.
#define HANDLE DWORD
#define HINSTANCE DWORD

// Sizes.
enum {DEFAULT_ALIGNMENT = 16}; // Default boundary to align memory allocations on.
enum {CACHE_LINE_SIZE   = 32}; // Cache line size.
#define GCC_PACK(n) __attribute__((aligned(n)))
#define GCC_REALLY_PACK(n) __attribute__((packed,aligned(n)))
//#define GCC_MOVE_ALIGN(n) __attribute__((aligned(n))) __attribute__((section (".bss")))
#define GCC_ALIGN(n) __attribute__((aligned(n)))
#define GCC_MOVE_ALIGN(n)

// Optimization macros
//#pragma GCC optimize ("O0")
#define DISABLE_OPTIMIZATION
#define ENABLE_OPTIMIZATION

// Function type macros.
#define DLL_IMPORT
#define DLL_EXPORT			extern "C"
#define DLL_EXPORT_CLASS
#define VARARGS     __cdecl
#define CDECL	    __cdecl
#define STDCALL
#define FORCEINLINE __attribute__((always_inline))/* Force code to be inline */
//#define inline   inline __attribute__((always_inline))
#define ZEROARRAY 0 /* Zero-length arrays in structs */
#define __cdecl

// Variable arguments.
#if UNICODE

// stijn: This is risky! vsprintf is _NOT_ compatible with the _vsnwprintf we use on Windows.
// Specifically, they differ in their treatment of wide-character and wide-string modifiers.

/*
#define GET_VARARGS(msg,len,fmt)				\
{	\
    va_list ArgPtr;	\
    va_start( ArgPtr, fmt );	\
    vswprintf( msg, len, fmt, ArgPtr );	\
    va_end( ArgPtr );	\
}
#define GET_VARARGS_RESULT(msg, len, fmt,result)	\
{	\
    va_list ArgPtr;	\
    va_start( ArgPtr, fmt );	\
    result = vswprintf( msg, len, fmt, ArgPtr );	\
    va_end( ArgPtr );	\
}
*/

extern "C" int appVswprintf(wchar_t* Out, size_t Count, const wchar_t* Fmt, va_list ArgPtr);
#define GET_VARARGS(msg,len,fmt)			\
  {							\
    va_list ArgPtr;					\
    va_start( ArgPtr, fmt );				\
    (void) appVswprintf( msg, len, fmt, ArgPtr );	\
    va_end( ArgPtr );					\
  }
#define GET_VARARGS_RESULT(msg,len,fmt,result)		\
  {							\
    va_list ArgPtr;					\
    va_start( ArgPtr, fmt );				\
    result = appVswprintf( msg, len, fmt, ArgPtr );	\
    va_end( ArgPtr );					\
  }

#else
#define GET_VARARGS(msg,len,fmt)	\
  {					\
    va_list ArgPtr;			\
    va_start( ArgPtr, fmt );		\
    vsprintf( msg, fmt, ArgPtr );	\
    va_end( ArgPtr );			\
  }
#define GET_VARARGS_RESULT(msg, len, fmt,result)	\
  {							\
    va_list ArgPtr;					\
    va_start( ArgPtr, fmt );				\
    result = vsprintf( msg, fmt, ArgPtr );		\
    va_end( ArgPtr );					\
  }
#endif

// Unsigned base types.
typedef bool BOOL;
typedef uint8_t		        BYTE;		// 8-bit  unsigned.
typedef uint16_t		    _WORD;		// 16-bit unsigned.
typedef uint32_t		    DWORD;		// 32-bit unsigned.
typedef uint32_t            UINT;		// 32-bit unsigned.
typedef uint64_t	        QWORD;		// 64-bit unsigned.

// Signed base types.
typedef	int8_t              SBYTE;		// 8-bit  signed.
typedef int16_t             SWORD;		// 16-bit signed.
typedef int16_t		        WORD;		// 16-bit signed.
typedef int32_t		        INT;	    // 32-bit signed.
typedef int64_t	            SQWORD;     // 64-bit signed.

#if UNICODE
// Character types.
typedef char				ANSICHAR;	// An ANSI character.
typedef unsigned short      UNICHAR;	// stijn: THIS IS A NETWORK UNICODE CHARACTER!!! We communicate in UTF-16LE so this needs to be unsigned short (2 bytes)
typedef unsigned char	    ANSICHARU;	// An ANSI character.
typedef unsigned short      UNICHARU;	// An unicode character.
typedef                 	char *NPSTR, *LPSTR, *PSTR;
typedef                 	wchar_t *NWPSTR, *LPWSTR, *PWSTR;
typedef					    LPWSTR PTSTR, LPTSTR;
#else
// Character types.
typedef char			    ANSICHAR;	// An ANSI character.
typedef unsigned short	    UNICHAR;	// A unicode character.
typedef unsigned char	    ANSICHARU;	// An ANSI character.
typedef unsigned short	    UNICHARU;	// A unicode character.
#endif

#ifdef UNICODE
typedef wchar_t TCHAR;
#define _TCHAR_DEFINED 1
#else
typedef char TCHAR;
#define _TCHAR_DEFINED 1
#endif

// Other base types.
typedef signed int		    UBOOL;		// Boolean 0 (false) or 1 (true).
typedef float			    FLOAT;		// 32-bit IEEE floating point.
typedef double			    DOUBLE;     // 64-bit IEEE double.
typedef uintptr_t		    PTRINT;
typedef size_t	            SIZE_T;     // Corresponds to C SIZE_T.
typedef ssize_t		        SSIZE_T;

// Bitfield type.
typedef unsigned int		BITFIELD;	// For bitfields.

#define FALSE 0
#define TRUE 1

// Make sure characters are unsigned.
#ifdef __CHAR_UNSIGNED__
	#error "Bad compiler option: Characters must be signed"
#endif

// Strings.
#if __GNUG__
    #define LINE_TERMINATOR TEXT("\n")
    #define PATH_SEPARATOR TEXT("/")
    #if MACOSX || MACOSXPPC
    #define DLLEXT TEXT(".dylib")
    #else
    #define DLLEXT TEXT(".so")
    #endif
#else
    #define LINE_TERMINATOR TEXT("\r\n")
    #define PATH_SEPARATOR TEXT("\\")
    #define DLLEXT TEXT(".dll")
#endif

#undef PATH_MAX
#define PATH_MAX 4096

// NULL.
#undef NULL
#define NULL 0

// Package implementation.
#define IMPLEMENT_PACKAGE_PLATFORM(pkgname) \
	BYTE GLoaded##pkgname;

// Platform support options.
#define PLATFORM_NEEDS_ARRAY_NEW 1
#define FORCE_ANSI_LOG           0
#define FORCE_UTF8_LOG           1

// OS unicode function calling.
CORE_API ANSICHAR* nixToANSI( ANSICHAR* ACh, const TCHAR* InUCh, INT Count );
CORE_API INT nixGetSizeANSI( const TCHAR* InUCh );
CORE_API TCHAR* nixToUNICODE( TCHAR* Ch, const ANSICHAR* InUCh, INT Count );
CORE_API INT nixGetSizeUNICODE( const ANSICHAR* InACh );
#define TCHAR_CALL_OS(funcW,funcA) (GUnicodeOS ? (funcW) : (funcA))
#define TCHAR_TO_ANSI(str) nixToANSI((ANSICHAR*)appAlloca(nixGetSizeANSI(str)),str,nixGetSizeANSI(str))
#define TCHAR_TO_OEM(str) nixToOEM((ANSICHAR*)appAlloca(nixGetSizeANSI(str)),str,nixGetSizeANSI(str))
#define ANSI_TO_TCHAR(str) nixToUNICODE((TCHAR*)appAlloca(nixGetSizeUNICODE(str)*sizeof(TCHAR)),str,nixGetSizeUNICODE(str))

// !! Fixme: This is a workaround.
#define GCC_OPT_INLINE

// Memory
#define appAlloca(size) alloca((size+7)&~7)
#define appAllocaType(type,arsize) reinterpret_cast<type*>(appAlloca(arsize*sizeof(type)))
#define appAllocaTypeZero(varname,type,arsize) appAllocaType(type,arsize); appMemzero(varname, arsize*sizeof(type))

/*----------------------------------------------------------------------------
	Globals.
----------------------------------------------------------------------------*/

// System identification.
extern "C"
{
	extern HINSTANCE      hInstance;
	extern CORE_API UBOOL GTimestamp;
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
}

// Module name
extern TCHAR GModule[256];

#if 0 //!DO_RYANS_HACKY_GUARD_BLOCKS
#include <sys/wait.h>
#include <signal.h>
#include <setjmp.h>

class __Context
{
public:

	__Context()
	{
	Last[0] = Env[0];
	Last[1] = Env[1];
	Last[2] = Env[2];
	Last[3] = Env[3];
	Last[4] = Env[4];
	Last[5] = Env[5];
	Last[6] = Env[6];
	memcpy(Last, Env, sizeof(jmp_buf));
	}

	~__Context()
	{
	Env[0] = Last [0];
	Env[1] = Last [1];
	Env[2] = Last [2];
	Env[3] = Last [3];
	Env[4] = Last [4];
	Env[5] = Last [5];
	Env[6] = Last [6];
	memcpy(Env, Last, sizeof(jmp_buf));
	}

	static void StaticInit();
	static jmp_buf Env;

protected:
	static void HandleSignal( int Sig );
	static struct sigaction Act_SIGHUP;
	static struct sigaction Act_SIGQUIT;
	static struct sigaction Act_SIGILL;
	static struct sigaction Act_SIGTRAP;
	static struct sigaction Act_SIGIOT;
	static struct sigaction Act_SIGBUS;
	static struct sigaction Act_SIGFPE;
	static struct sigaction Act_SIGSEGV;
	static struct sigaction Act_SIGTERM;
	jmp_buf Last;
};
#endif

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/

