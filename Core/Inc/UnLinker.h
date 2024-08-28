/*=============================================================================
	UnLinker.h: Unreal object linker.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
    * Added MD5 checksum by Michiel Hendriks
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

 #ifndef MD5CHUNK
 #define MD5CHUNK 1024
 #endif

 #ifndef SHABUFLEN
 #define SHABUFLEN 16384
 #endif

/*-----------------------------------------------------------------------------
	FObjectExport.
-----------------------------------------------------------------------------*/

//
// Information about an exported object.
//
struct CORE_API FObjectExport
{
	// Variables.
	INT         ClassIndex;		// Persistent.
	INT         SuperIndex;		// Persistent (for UStruct-derived objects only).
	INT			PackageIndex;	// Persistent.
	INT			ArchetypeIndex;	// Persistent.
	FName		ObjectName;		// Persistent.
	DWORD		ObjectFlags;	// Persistent.
	INT         SerialSize;		// Persistent.
	INT         SerialOffset;	// Persistent (for checking only).
	UObject*	_Object;		// Internal.
	INT			_iHashNext;		// Internal.

	// Functions.
	FObjectExport();
	FObjectExport( UObject* InObject );

	CORE_API friend FArchive& operator<<(FArchive& Ar, FObjectExport& E);
};

/*-----------------------------------------------------------------------------
	FObjectImport.
-----------------------------------------------------------------------------*/

//
// Information about an imported object.
//
struct CORE_API FObjectImport
{
	// Variables.
	FName			ClassPackage;	// Persistent.
	FName			ClassName;		// Persistent.
	INT				PackageIndex;	// Persistent.
	FName			ObjectName;		// Persistent.
	UObject*		XObject;		// Internal (only really needed for saving, can easily be gotten rid of for loading).
	ULinkerLoad*	SourceLinker;	// Internal.
	INT             SourceIndex;	// Internal.

	// Functions.
	FObjectImport();
	FObjectImport( UObject* InObject );

	CORE_API friend FArchive& operator<<(FArchive& Ar, FObjectImport& I);
};

/*----------------------------------------------------------------------------
	Items stored in Unrealfiles.
----------------------------------------------------------------------------*/

//
// Unrealfile summary, stored at top of file.
//
struct FGenerationInfo
{
	INT ExportCount, NameCount;

	FGenerationInfo( INT InExportCount, INT InNameCount )
		: ExportCount(InExportCount), NameCount(InNameCount)
	{}
	friend FArchive& operator<<( FArchive& Ar, FGenerationInfo& Info )
	{
		guard(FGenerationInfo<<);
		return Ar << Info.ExportCount << Info.NameCount;
		unguard;
	}
};

struct FPackageFileSummary
{
	// Variables.
	INT		Tag;
#if 1 //LVer added by Legend on 4/12/2000
protected:
	INT		FileVersion;
public:
#else
	INT		FileVersion;
#endif
	DWORD	PackageFlags;
	INT		NameCount,		NameOffset;
	INT		ExportCount,	ExportOffset;
	INT     ImportCount,	ImportOffset;
	FGuid	Guid;
	TArray<FGenerationInfo> Generations;

	#ifdef UTPG_MD5
	FString ZQ5Purpxfhz; //elmuerte: MD5 checksum
	UBOOL		TableVerified; // md5 has been checked
	float		LoadedTime;
	#endif

	// Constructor.
	FPackageFileSummary()
		: Tag(0), FileVersion(0), PackageFlags(0), NameCount(0), NameOffset(0), ExportCount(0), ExportOffset(0), ImportCount(0), ImportOffset(0)
#ifdef UTPG_MD5
		, TableVerified(0), LoadedTime(0)
#endif
	{}

#if 1 //LVer added by Legend on 4/12/2000
	INT GetFileVersion() const { return (FileVersion & 0xffff); }
	INT GetFileVersionLicensee() const { return ((FileVersion >> 16) & 0xffff); }
	void SetFileVersions(INT Epic, INT Licensee) { FileVersion = ((Licensee << 16) | Epic); }
#endif

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, FPackageFileSummary& Sum )
	{
		guard(FUnrealfileSummary<<);

		Ar << Sum.Tag;
		Ar << Sum.FileVersion;
		Ar << Sum.PackageFlags;
		Ar << Sum.NameCount     << Sum.NameOffset;
		Ar << Sum.ExportCount   << Sum.ExportOffset;
		Ar << Sum.ImportCount   << Sum.ImportOffset;
		if( Sum.GetFileVersion()>=68 ) //LVer added by Legend on 4/12/2000
		{
			INT GenerationCount = Sum.Generations.Num();
			Ar << Sum.Guid << GenerationCount;
			//!!67 had: return
			if( Ar.IsLoading() )
			{
				GenerationCount = Clamp<INT>(GenerationCount,0,64);
				Sum.Generations = TArray<FGenerationInfo>( GenerationCount );
			}
			for( INT i=0; i<GenerationCount; i++ )
				Ar << Sum.Generations(i);
		}
		else //oldver
		{
			INT HeritageCount, HeritageOffset;
			Ar << HeritageCount << HeritageOffset;
			INT Saved = Ar.Tell();
			if( HeritageCount )
			{
				Ar.Seek( HeritageOffset );
				for( INT i=0; i<HeritageCount; i++ )
					Ar << Sum.Guid;
			}
			Ar.Seek( Saved );
			if( Ar.IsLoading() )
			{
				Sum.Generations.Empty( 1 );
				new(Sum.Generations)FGenerationInfo(Sum.ExportCount,Sum.NameCount);
			}
		}

		return Ar;
		unguard;
	}
};

struct FSavedGameSummary
{
	// Set by engine:
	FString OrgFile;

	// Variables.
	FString MapTitle, Timestamp, ExtraInfo;
	TArray<BYTE> Screenshot;

	// Constructor.
	FSavedGameSummary()
	{}

	// Serializer.
	friend FArchive& operator<<(FArchive& Ar, FSavedGameSummary& Sum)
	{
		guard(FSavedGameSummary << );

		Ar << Sum.OrgFile << Sum.MapTitle << Sum.Timestamp << Sum.ExtraInfo << Sum.Screenshot;

		return Ar;
		unguard;
	}
};

/*----------------------------------------------------------------------------
	FPackageRemap.
----------------------------------------------------------------------------*/

class CORE_API FPackageRemap
{
public:
	struct FRemapEntry
	{
		FString PckName,ObjName;
		FRemapEntry( const TCHAR* Pck, const TCHAR* Obj )
			: PckName(Pck), ObjName(Obj)
		{}
		UBOOL operator==( const FRemapEntry& Other ) const
		{
			return PckName==Other.PckName && ObjName==Other.ObjName;
		}
	};

	static TMap<FRemapEntry,FRemapEntry>* RemapList;
	static void InitializeRemap();
	static void ExitRemap();
};
inline DWORD GetTypeHash( const FPackageRemap::FRemapEntry& E )
{
	return appStrihash(*E.PckName) ^ appStrihash(*E.ObjName);
}

/*----------------------------------------------------------------------------
	ULinker.
----------------------------------------------------------------------------*/

//
// A file linker.
//
class CORE_API ULinker : public UObject
{
	DECLARE_CLASS(ULinker,UObject,CLASS_Transient,Core)
	NO_DEFAULT_CONSTRUCTOR(ULinker)

	// Variables.
	UObject*				LinkerRoot;			// The linker's root object.
	FPackageFileSummary		Summary;			// File summary.
	TArray<FName>			NameMap;			// Maps file name indices to name table indices.
	TArray<FObjectImport>	ImportMap;			// Maps file object indices >=0 to external object names.
	TArray<FObjectExport>	ExportMap;			// Maps file object indices >=0 to external object names.
	INT						Success;			// Whether the object was constructed successfully.
	FString					Filename;			// Filename.
	DWORD					_ContextFlags;		// Load flag mask.

	// Constructors.
	ULinker( UObject* InRoot, const TCHAR* InFilename );
	void Serialize( FArchive& Ar );
	FString GetImportFullName( INT i );
	FString GetExportFullName( INT i, const TCHAR* FakeRoot=NULL );
};

/*----------------------------------------------------------------------------
	ULinkerLoad.
----------------------------------------------------------------------------*/

//
// A file loader.
//
class CORE_API ULinkerLoad : public ULinker, public FArchive
{
	DECLARE_CLASS(ULinkerLoad,ULinker,CLASS_Transient,Core)
	NO_DEFAULT_CONSTRUCTOR(ULinkerLoad)

	// Friends.
	friend class UObject;
	friend class UPackageMap;

	// Variables.
	DWORD					LoadFlags;
	UBOOL					Verified;
	INT						ExportHash[256];
	TArray<FLazyLoader*>	LazyLoaders;
	FArchive*				Loader;
	ULinkerLoad*			SavedGameLoader, *ChildLoader;

	#ifdef UTPG_MD5
	FString       ShortFilename;
	FString       PartialHash; // temp storage
	FString				FileLang;
	#endif

	ULinkerLoad( UObject* InParent, const TCHAR* InFilename, DWORD InLoadFlags );

	void Verify();
	FName GetExportClassPackage( INT i );
	FName GetExportClassName( INT i );
	void VerifyImport( INT i );
	void LoadAllObjects(FName LoadType = NAME_None);
	INT FindExportIndex( FName ClassName, FName ClassPackage, FName ObjectName, INT PackageIndex );
	UObject* Create( UClass* ObjectClass, FName ObjectName, DWORD LoadFlags, UBOOL Checked );
	void Preload( UObject* Object );

	#ifdef UTPG_MD5
	FString GenFullSum();
	FString GenPartialSum();
	#endif

private:
	#ifdef UTPG_MD5
	FString DoRealFullMD5();
	#endif

	UObject* CreateExport( INT Index );
	UObject* CreateImport( INT Index );

	UObject* IndexToObject( INT Index );
	void DetachExport( INT i );

	// UObject interface.
	void Serialize( FArchive& Ar );
	void Destroy();

public:
	// FArchive interface.
	void AttachLazyLoader( FLazyLoader* LazyLoader );
	void DetachLazyLoader( FLazyLoader* LazyLoader );
	void DetachAllLazyLoaders( UBOOL Load );
	void Seek( INT InPos );
	INT Tell();
	INT TotalSize();
	void Serialize( void* V, INT Length );
	FArchive& operator<<( UObject*& Object )
	{
		guard(ULinkerLoad<<UObject);
		INT Index = 0;
		*Loader << AR_INDEX(Index);
		UObject* Temporary = IndexToObject( Index );
		appMemcpy(&Object, &Temporary, sizeof(UObject*));

		return *this;
		unguardf(( TEXT("(%ls %i))"), GetFullName(), Tell() ));
	}

	FArchive& operator<<( FName& Name )
	{
		guard(ULinkerLoad<<FName);

		NAME_INDEX NameIndex = 0;
		*Loader << AR_INDEX(NameIndex);

		if( !NameMap.IsValidIndex(NameIndex) )
			appErrorf( TEXT("Bad name index %i/%i"), NameIndex, NameMap.Num() );
		FName Temporary = NameMap( NameIndex );
		appMemcpy(&Name, &Temporary, sizeof(FName));

		return *this;
		unguardf(( TEXT("(%ls %i))"), GetFullName(), Tell() ));
	}
};

/*----------------------------------------------------------------------------
	ULinkerSave.
----------------------------------------------------------------------------*/

//
// A file saver.
//
class ULinkerSave : public ULinker, public FArchive
{
	DECLARE_CLASS(ULinkerSave,ULinker,CLASS_Transient,Core);
	NO_DEFAULT_CONSTRUCTOR(ULinkerSave);

	// Variables.
	FArchive* Saver;
	TArray<INT> ObjectIndices;
	TArray<INT> NameIndices;

	// Constructor.
	ULinkerSave( UObject* InParent, const TCHAR* InFilename );
	void Destroy();

	// FArchive interface.
	INT MapName( FName* Name );
	INT MapObject( UObject* Object );
	FArchive& operator<<( FName& Name )
	{
		guardSlow(ULinkerSave<<FName);
		INT Save = NameIndices(Name.GetIndex());
		return *this << AR_INDEX(Save);
		unguardobjSlow;
	}
	FArchive& operator<<( UObject*& Obj )
	{
		guardSlow(ULinkerSave<<UObject);
		INT Save = Obj ? ObjectIndices(Obj->GetIndex()) : 0;
		return *this << AR_INDEX(Save);
		unguardobjSlow;
	}
	void Seek( INT InPos );
	INT Tell();
	void Serialize( void* V, INT Length );
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
