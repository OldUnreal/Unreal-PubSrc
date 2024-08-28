/*=============================================================================
	UnCoreNative.h: Native function lookup table for static libraries.
	Copyright 2000 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Brandon Reinhart
=============================================================================*/

#ifndef UNCORENATIVE_H
#define UNCORENATIVE_H

#define MAP_NATIVE(cls,name) \
	{"int"#cls#name, &cls::name},

#define DECLARE_NATIVE_TYPE(pkg,type) \
	typedef void (type::*type##Native)( FFrame& TheStack, RESULT_DECL ); \
	struct type##NativeInfo \
	{ const ANSICHAR* Name; type##Native Pointer; }; \
	extern void* Find##pkg##type##Native( const char* NativeName );

#define IMPLEMENT_NATIVE_HANDLER(pkg,type) \
	void* Find##pkg##type##Native( const char* NativeName ) \
	{ INT i=0; while (G##pkg##type##Natives[i].Name) \
		{ if (strcmp(NativeName, G##pkg##type##Natives[i].Name) == 0) \
			return &G##pkg##type##Natives[i].Pointer; \
		i++; } return NULL; }

typedef void* (*NativeLookup)(const char*);
extern NativeLookup GNativeLookupFuncs[128];

/*-----------------------------------------------------------------------------
  Custom Network Serialization
-----------------------------------------------------------------------------*/
void SerializeVector(FArchive& Ar, UPackageMap* Map, FVector* Data);
void SerializeRotator(FArchive& Ar, UPackageMap* Map, FRotator* Data);
void SerializePlane(FArchive& Ar, UPackageMap* Map, FPlane* Data);
void SerializeVector2D(FArchive& Ar, UPackageMap* Map, FVector2D* Data);
void SerializeVectorNormal(FArchive& Ar, UPackageMap* Map, FVectorNormal* Data);
void SerializePackedInt(FArchive& Ar, UPackageMap* Map, FPackedInt* Data);

#define MAP_NETSERIALIZE_NATIVE(strname,func)	\
	{"netserialize"#strname, reinterpret_cast<void*>(&func)},

void* FindNetSerializeNative(const char*);

static inline void* FindNative( const char* NativeName )
{
	for (INT i=0; i<ARRAY_COUNT(GNativeLookupFuncs); i++)
	{
		if (GNativeLookupFuncs[i])
		{
			void* Result = GNativeLookupFuncs[i]( NativeName );
			if (Result)
				return Result;
		}
	}
	return NULL;
}

DECLARE_NATIVE_TYPE(Core,UObject);
DECLARE_NATIVE_TYPE(Core,UCommandlet);
#ifdef UTPG_MD5
DECLARE_NATIVE_TYPE(Core,FZQ5Erpbeq);
#endif
DECLARE_NATIVE_TYPE(Core,ULogHandler);
DECLARE_NATIVE_TYPE(Core,ULocale);
DECLARE_NATIVE_TYPE(Core,UScriptHook);
	
#endif
