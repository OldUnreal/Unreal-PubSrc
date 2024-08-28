/*=============================================================================
	UnObjVer.h: Unreal object version.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

/*-----------------------------------------------------------------------------
	Version coding.
-----------------------------------------------------------------------------*/

// Earliest engine build that is network compatible with this one.
#define ENGINE_MIN_NET_VERSION 224

// Engine build number, for displaying to end users.
#define ENGINE_VERSION 227

// Base protocol version to negotiate in network play.
#define ENGINE_NEGOTIATION_VERSION 227
#define ENGINE_SUBVERSION 11
#define ENGINE_MIN_SUBVERSION 9
#define ENGINE_BUILT_TIME __TIMESTAMP__

// Prevents incorrect files from being loaded.
#define PACKAGE_FILE_TAG 0x9E2A83C1

// The current Unrealfile version.
#define PACKAGE_FILE_VERSION 69

// New network protocol version (227j)
#define NETWORK_VERSION_INDEX 2

#if 1 //LVer added by Legend on 4/12/2000
//
// Licensee Version Number
// by Paul Du Bois, Infinite Machine
//
// Uses the upper two words of the VersionNum DWORD to encode a licensee-specific
// version WORD.
//
#define PACKAGE_FILE_VERSION_LICENSEE	227

#define VERSION_ENGINE_ONLY				0x00
#endif

// The earliest file version which we can load with complete
// backwards compatibility. Must be at least PACKAGE_FILE_VERSION.
#define PACKAGE_MIN_VERSION 60

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
