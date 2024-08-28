/*=============================================================================
	UnCoreNet.h: Core networking support.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

//
// Information about a field.
//
enum EFieldNetCacheType
{
	FNC_Property,
	FNC_NativeProperty,
	FNC_Function,
	FNC_Invalid,
};
class CORE_API FFieldNetCache
{
public:
	UField* Field;
	INT RepIndex;
	INT ValueIndex;
	INT ConditionIndex;
	EFieldNetCacheType CacheType;
	FFieldNetCache()
	{}
	FFieldNetCache(ENoInit)
		: Field(NULL), RepIndex(-1), CacheType(FNC_Property)
	{}
	FFieldNetCache( UField* InField, INT InRepIndex, INT InValueIndex, INT InConditionIndex )
	: Field(InField), RepIndex(InRepIndex), ValueIndex(InValueIndex), ConditionIndex(InConditionIndex)
	{}
	friend CORE_API FArchive& operator<<( FArchive& Ar, FFieldNetCache& F );
};

//
// Information about a class, cached for network coordination.
//
class CORE_API FClassNetCache
{
	friend class UPackageMap;
	friend class UPackageMapUTLevel;
public:
	FClassNetCache();
	FClassNetCache( UClass* Class );
	INT GetMaxIndex();
	INT GetMaxPropertyIndex();
	INT GetRepValueCount();
	INT GetRepConditionCount();
	FFieldNetCache* GetFromField( UField* Field );
	FFieldNetCache* GetFromIndex( INT Index );
	CORE_API friend FArchive& operator<<( FArchive& Ar, FClassNetCache& Cache );
private:
	INT RepValueCount, RepConditionCount, MaxPropertyIndex;
	UClass* Class;
	TArray<FFieldNetCache> Fields;
	TMap<UField*,INT> FieldIndices;
};

//
// Ordered information of linker file requirements.
//
class CORE_API FPackageInfo
{
public:
	// Variables.
	FString			URL;				// URL of the package file we need to request.
	ULinkerLoad*	Linker;				// Pointer to the linker, if loaded.
	UObject*		Parent;				// The parent package.
	FGuid			Guid;				// Package identifier.
	INT				FileSize;			// File size.
	INT				ObjectBase;			// Net index of first object.
	INT				ObjectCount;		// Number of objects, defined by server.
	INT				NameBase;			// Net index of first name.
	INT				NameCount;			// Number of names, defined by server.
	INT				Generation;			// Generation to run with.
	INT				ServerGeneration;	// Serverside generation.
	DWORD			PackageFlags;		// Package flags.

	// Functions.
	FPackageInfo(); // Client: Construct with unknown linker.
	FPackageInfo( ULinkerLoad* InLinker ); // Server: Construct with linker.
	CORE_API friend FArchive& operator<<( FArchive& Ar, FPackageInfo& I );

	void SetLinker(ULinkerLoad* InLinker); // Client: Assign a linker to this package information.

	#ifdef UTPG_MD5
	FString ZQ5Purpxfhz(); //elmuerte: MD5 checksum
	FString GetFilename(); //elmuerte: used for GETMD5 console command
	void SetCheck(UBOOL val); //elmuerte: set package validation
	FString GetLanguage();
	#endif

	UBOOL GenerationMismatch() const; // Client: Check if generation information is a mismatch.
	UBOOL GenerationModified() const; // Client: Should notify of package difference with server.

	void SetGeneration(INT NewGen); // Change generation.
};

//
// Maps objects and names to and from indices for network communication.
//
class CORE_API UPackageMap : public UObject
{
	friend class UObject;
	friend class UPackage;

	DECLARE_CLASS(UPackageMap,UObject,CLASS_Transient,Core);

	// UObject interface.
	void Serialize( FArchive& Ar );
	void Destroy();

	// UPackageMap interface.
	virtual UBOOL CanSerializeObject( UObject* Obj );
	virtual UBOOL SerializeObject( FArchive& Ar, UClass* Class, UObject*& Obj );
	virtual UBOOL SerializeName( FArchive& Ar, FName& Name );
	virtual INT ObjectToIndex( UObject* Object );
	virtual UObject* IndexToObject( INT Index, UBOOL Load );
	virtual INT AddLinker( ULinkerLoad* Linker );
	virtual void Compute();
	virtual FClassNetCache* GetClassNetCache( UClass* Class );
	virtual UBOOL SupportsPackage( UObject* InOuter );
	virtual void Copy( UPackageMap* Other );
	virtual void ChangeGeneration(const INT PackageIndex, const INT NewGeneration); // Server: Change generation.

	INT FindPackageLinker(ULinkerLoad* Linker) const;
	INT FindPackageName(const TCHAR* PackageName) const;
	inline INT GetMaxObjectIndex() const { return MaxObjectIndex; }

	// Variables.
	TArray<FPackageInfo> List;
protected:
	TMap<UObject*,INT> LinkerMap;
	TMap<UObject*,FClassNetCache> ClassFieldIndices;
	TArray<DWORD> NameIndices;
	TArray<FName> NameMap;
	DWORD MaxObjectIndex;
	DWORD MaxNameIndex;
	TMap<FName, FGuid> CompatibilityMap;

private:
	TArray<UPackage*> RepackagedList;

	void MakeActive();
	void Deactivate();
	void NotePackageUnload(UPackage* P);
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
