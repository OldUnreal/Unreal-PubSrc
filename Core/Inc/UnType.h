/*=============================================================================
	UnType.h: Unreal engine base type definitions.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

/*-----------------------------------------------------------------------------
	UProperty.
-----------------------------------------------------------------------------*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

// Property exporting flags.
enum EPropertyPortFlags
{
	PPF_Localized	= (1 << 0),
	PPF_Delimited	= (1 << 1),
	PPF_Exec		= (1 << 2),
	PPF_ExecImport	= (1 << 3),
	PPF_PropWindow	= (1 << 4),
	PPF_RotDegree	= (1 << 5),
	PPF_ObjPointers	= (1 << 6),
	PPF_Config		= (1 << 7),
};

// Type of replication eval.
enum ERepLimitator : DWORD
{
	REPL_NetOwnerOnly	= (1 << 0), // Only if bNetOwner
	REPL_NotNetOwner	= (1 << 1), // Only if !bNetOwner
	REPL_NetInitial		= (1 << 2), // Only if bNetInitial
};

// A custom handle for recognizing pointer indexes.
struct FObjectImportHandle
{
	virtual UObject* FindByIndex(INT InIndex) = 0;
};

//
// An UnrealScript variable.
//
class CORE_API UProperty : public UField
{
	DECLARE_ABSTRACT_CLASS(UProperty,UField,0,Core)
	DECLARE_WITHIN(UField)

	// Persistent variables.
	INT			ArrayDim;
	INT			ElementSize;
	DWORD		PropertyFlags;
	INT			Offset;
	_WORD		RepOffset;
	_WORD		RepIndex;
	FString		Description;
	// stijn: we temporarily store the alignment of the property here
	_WORD   LinkAlignment;

	// In memory variables.
	FName		Category;
	UProperty*	PropertyLinkNext;
	UProperty*	ConfigLinkNext;
	UProperty*	ConstructorLinkNext;
	UProperty*	RepOwner;
	UProperty*	NextReference;
	UProperty*	NextActorReference;
	UProperty*	NextNetActorReference;
	UProperty*	NextAutoDestruct;
	DWORD		TempPropertyFlags; // Not serialized compile-time property flags.
	DWORD		RepLimitator;

	// Compile time information.
	static UClass* _ImportObject;
	static FObjectImportHandle* _ImportHandle;

	// Constructors.
	UProperty();
	UProperty( ECppProperty, INT InOffset, const TCHAR* InCategory, DWORD InFlags, const TCHAR* Desc=NULL );

	// UObject interface.
	void Serialize( FArchive& Ar );
	void StaticConstructor();
	void PostLoad();

	// UProperty interface.
	virtual void Link( FArchive& Ar, UProperty* Prev );
	virtual void ExportCpp( FOutputDevice& Out, UBOOL IsLocal, UBOOL IsParm ) const;
	virtual void ExportUScriptName(FOutputDevice& Out) const {}
	virtual UBOOL NetSerializeItem( FArchive& Ar, UPackageMap* Map, void* Data ) const;
	virtual UBOOL ExportText( INT ArrayElement, FString& ValueStr, BYTE* Data, BYTE* Delta, INT PortFlags ) const;
	virtual void CopySingleValue( void* Dest, void* Src ) const;
	virtual void CopyCompleteValue( void* Dest, void* Src, UObject* Obj=NULL ) const;
	virtual void DestroyValue( void* Dest ) const;
	virtual UBOOL Port() const;
	virtual BYTE GetID() const;
	virtual UBOOL CleanupDestroyed(BYTE* Data) const { return 0; }
	virtual UBOOL ClearNetReference(BYTE* Data, UObject* Other) const { return 0; }
	virtual void CleanupAutoDestructions(BYTE* Data) const {}
	virtual UBOOL Identical( const void* A, const void* B ) const _VF_BASE_RET(0);
	virtual const TCHAR* ImportText( const TCHAR* Buffer, BYTE* Data, INT PortFlags ) const _VF_BASE_RET(NULL);
	virtual void ExportCppItem( FOutputDevice& Out ) const _VF_BASE;
	virtual void SerializeItem( FArchive& Ar, void* Value ) const _VF_BASE;
	virtual void ExportTextItem( FString& ValueStr, BYTE* PropertyValue, BYTE* DefaultValue, INT PortFlags ) const _VF_BASE;
	virtual DWORD GetMapHash(BYTE* Data) const;
	virtual void SerializeObjectRefs(FArchive& Ar, BYTE* Data) const {}
	virtual UBOOL ShouldSerializeItem(const BYTE* Data, INT Index) const;
	virtual INT GetLinkAlignment() const
	{
		return 4;
	}

	// This property may contain an Object reference that needs to be removed when object's deleted.
	virtual UBOOL HasObjectRef()
	{
		return FALSE;
	}
	virtual UBOOL HasActorRef()
	{
		return FALSE;
	}
	virtual UBOOL IsAutoDestruct()
	{
		return FALSE;
	}

	// Inlines.
	UBOOL Matches( const void* A, const void* B, INT ArrayIndex ) const
	{
		guardSlow(UProperty::Matches);
		if ((A && !B) || (B && !A))
			return false;
		INT Ofs = Offset + ArrayIndex * ElementSize;
		return Identical( (BYTE*)A + Ofs, B ? (BYTE*)B + Ofs : NULL );
		unguardobjSlow;
	}
	INT GetSize() const
	{
		guardSlow(UProperty::GetSize);
		return ArrayDim * ElementSize;
		unguardobjSlow;
	}
	inline UBOOL ShouldSerializeValue(FArchive& Ar) const
	{
		guardSlow(UProperty::ShouldSerializeValue);
		return !((PropertyFlags & CPF_Native) || ((PropertyFlags & CPF_Transient) && Ar.IsPersistent()));
		unguardSlow;
	}
	inline UBOOL IsOutReference() const
	{
#if _USE_REF_PARMS
		return (PropertyFlags & (CPF_Parm | CPF_OutParm | CPF_ReturnParm)) == (CPF_Parm | CPF_OutParm);
#else
		return FALSE;
#endif
	}
	inline UBOOL CanReplicate(DWORD RequiredFlags) const
	{
		if (RepLimitator)
		{
			if ((RepLimitator & REPL_NetInitial) && !(RequiredFlags & REPL_NetInitial))
				return FALSE;
			if ((RepLimitator & (REPL_NetOwnerOnly | REPL_NotNetOwner)) && ((RepLimitator & (RequiredFlags & (REPL_NetOwnerOnly | REPL_NotNetOwner))) != (RequiredFlags & (REPL_NetOwnerOnly | REPL_NotNetOwner))))
				return FALSE;
		}
		return TRUE;
	}
};

/*-----------------------------------------------------------------------------
	UByteProperty.
-----------------------------------------------------------------------------*/

//
// Describes an unsigned byte value or 255-value enumeration variable.
//
class CORE_API UByteProperty : public UProperty
{
	DECLARE_CLASS(UByteProperty,UProperty,0,Core)

	// Variables.
	UEnum* Enum;

	// Constructors.
	UByteProperty()
	{}
	UByteProperty( ECppProperty, INT InOffset, const TCHAR* InCategory, DWORD InFlags, UEnum* InEnum=NULL, const TCHAR* Desc = NULL)
	:	UProperty( EC_CppProperty, InOffset, InCategory, InFlags, Desc)
	,	Enum( InEnum )
	{}

	// UObject interface.
	void Serialize( FArchive& Ar );

	// UProperty interface.
	void Link( FArchive& Ar, UProperty* Prev );
	UBOOL Identical( const void* A, const void* B ) const;
	void SerializeItem( FArchive& Ar, void* Value ) const;
	UBOOL NetSerializeItem( FArchive& Ar, UPackageMap* Map, void* Data ) const;
	void ExportCppItem( FOutputDevice& Out ) const;
	void ExportUScriptName(FOutputDevice& Out) const;
	void ExportTextItem( FString& ValueStr, BYTE* PropertyValue, BYTE* DefaultValue, INT PortFlags ) const;
	const TCHAR* ImportText( const TCHAR* Buffer, BYTE* Data, INT PortFlags ) const;
	void CopySingleValue( void* Dest, void* Src ) const;
	void CopyCompleteValue( void* Dest, void* Src, UObject* Obj=NULL ) const;
	DWORD GetMapHash(BYTE* Data) const;

	INT GetLinkAlignment() const
	{
		return alignof(BYTE);
	}
};

/*-----------------------------------------------------------------------------
	UIntProperty.
-----------------------------------------------------------------------------*/

//
// Describes a 32-bit signed integer variable.
//
class CORE_API UIntProperty : public UProperty
{
	DECLARE_CLASS(UIntProperty,UProperty,0,Core)

	// Constructors.
	UIntProperty()
	{}
	UIntProperty( ECppProperty, INT InOffset, const TCHAR* InCategory, DWORD InFlags, const TCHAR* Desc = NULL)
	:	UProperty( EC_CppProperty, InOffset, InCategory, InFlags, Desc)
	{}

	// UProperty interface.
	void Link( FArchive& Ar, UProperty* Prev );
	UBOOL Identical( const void* A, const void* B ) const;
	void SerializeItem( FArchive& Ar, void* Value ) const;
	UBOOL NetSerializeItem( FArchive& Ar, UPackageMap* Map, void* Data ) const;
	void ExportCppItem( FOutputDevice& Out ) const;
	void ExportUScriptName(FOutputDevice& Out) const;
	void ExportTextItem( FString& ValueStr, BYTE* PropertyValue, BYTE* DefaultValue, INT PortFlags ) const;
	const TCHAR* ImportText( const TCHAR* Buffer, BYTE* Data, INT PortFlags ) const;
	void CopySingleValue( void* Dest, void* Src ) const;
	void CopyCompleteValue( void* Dest, void* Src, UObject* Obj=NULL ) const;
	DWORD GetMapHash(BYTE* Data) const;

	INT GetLinkAlignment() const
	{
		return alignof(INT);
	}
};

/*-----------------------------------------------------------------------------
	UBoolProperty.
-----------------------------------------------------------------------------*/

//
// Describes a single bit flag variable residing in a 32-bit unsigned double word.
//
class CORE_API UBoolProperty : public UProperty
{
	DECLARE_CLASS(UBoolProperty,UProperty,0,Core)

	// Variables.
	BITFIELD BitMask;

	// Constructors.
	UBoolProperty()
	{}
	UBoolProperty( ECppProperty, INT InOffset, const TCHAR* InCategory, DWORD InFlags, BITFIELD BitFlags=FIRST_BITFIELD, const TCHAR* Desc = NULL)
	:	UProperty( EC_CppProperty, InOffset, InCategory, InFlags, Desc)
	,	BitMask( BitFlags )
	{}

	// UObject interface.
	void Serialize( FArchive& Ar );

	// UProperty interface.
	void Link( FArchive& Ar, UProperty* Prev );
	UBOOL Identical( const void* A, const void* B ) const;
	void SerializeItem( FArchive& Ar, void* Value ) const;
	UBOOL NetSerializeItem( FArchive& Ar, UPackageMap* Map, void* Data ) const;
	void ExportCppItem( FOutputDevice& Out ) const;
	void ExportUScriptName(FOutputDevice& Out) const;
	void ExportTextItem( FString& ValueStr, BYTE* PropertyValue, BYTE* DefaultValue, INT PortFlags ) const;
	const TCHAR* ImportText( const TCHAR* Buffer, BYTE* Data, INT PortFlags ) const;
	void CopySingleValue( void* Dest, void* Src ) const;
	DWORD GetMapHash(BYTE* Data) const;

	INT GetLinkAlignment() const
	{
		return alignof(BITFIELD);
	}
};

/*-----------------------------------------------------------------------------
	UFloatProperty.
-----------------------------------------------------------------------------*/

//
// Describes an IEEE 32-bit floating point variable.
//
class CORE_API UFloatProperty : public UProperty
{
	DECLARE_CLASS(UFloatProperty,UProperty,0,Core)

	// Constructors.
	UFloatProperty()
	{}
	UFloatProperty( ECppProperty, INT InOffset, const TCHAR* InCategory, DWORD InFlags, const TCHAR* Desc = NULL)
	:	UProperty( EC_CppProperty, InOffset, InCategory, InFlags, Desc)
	{}

	// UProperty interface.
	void Link( FArchive& Ar, UProperty* Prev );
	UBOOL Identical( const void* A, const void* B ) const;
	void SerializeItem( FArchive& Ar, void* Value ) const;
	UBOOL NetSerializeItem( FArchive& Ar, UPackageMap* Map, void* Data ) const;
	void ExportCppItem( FOutputDevice& Out ) const;
	void ExportUScriptName(FOutputDevice& Out) const;
	void ExportTextItem( FString& ValueStr, BYTE* PropertyValue, BYTE* DefaultValue, INT PortFlags ) const;
	const TCHAR* ImportText( const TCHAR* Buffer, BYTE* Data, INT PortFlags ) const;
	void CopySingleValue( void* Dest, void* Src ) const;
	void CopyCompleteValue( void* Dest, void* Src, UObject* Obj=NULL ) const;
	DWORD GetMapHash(BYTE* Data) const;

	INT GetLinkAlignment() const
	{
		return alignof(FLOAT);
	}
};

/*-----------------------------------------------------------------------------
	UObjectProperty.
-----------------------------------------------------------------------------*/

//
// Describes a reference variable to another object which may be nil.
//
class CORE_API UObjectProperty : public UProperty
{
	DECLARE_CLASS(UObjectProperty,UProperty,0,Core)

	// Variables.
	class UClass* PropertyClass;

	// Constructors.
	UObjectProperty()
	{}
	UObjectProperty( ECppProperty, INT InOffset, const TCHAR* InCategory, DWORD InFlags, UClass* InClass, const TCHAR* Desc = NULL)
	:	UProperty( EC_CppProperty, InOffset, InCategory, InFlags, Desc)
	,	PropertyClass( InClass )
	{}

	// UObject interface.
	void Serialize( FArchive& Ar );

	// UProperty interface.
	void Link( FArchive& Ar, UProperty* Prev );
	UBOOL Identical( const void* A, const void* B ) const;
	void SerializeItem( FArchive& Ar, void* Value ) const;
	UBOOL NetSerializeItem( FArchive& Ar, UPackageMap* Map, void* Data ) const;
	void ExportCppItem( FOutputDevice& Out ) const;
	void ExportUScriptName(FOutputDevice& Out) const;
	void ExportTextItem( FString& ValueStr, BYTE* PropertyValue, BYTE* DefaultValue, INT PortFlags ) const;
	const TCHAR* ImportText( const TCHAR* Buffer, BYTE* Data, INT PortFlags ) const;
	static const TCHAR* StaticImportObject(const TCHAR* Buffer, UObject** Result, INT PortFlags, UClass* MetaClass, const TCHAR* Method);
	void CopySingleValue( void* Dest, void* Src ) const;
	void CopyCompleteValue( void* Dest, void* Src, UObject* Obj=NULL ) const;
	UBOOL CleanupDestroyed( BYTE* Data ) const;
	UBOOL ClearNetReference(BYTE* Data, UObject* Other) const;
	void CleanupAutoDestructions(BYTE* Data) const;
	DWORD GetMapHash(BYTE* Data) const;
	UBOOL HasObjectRef()
	{
		return TRUE;
	}
	UBOOL HasActorRef();
	UBOOL IsAutoDestruct()
	{
		return (PropertyFlags & CPF_AutoDestruct) != 0;
	}
	void SerializeObjectRefs(FArchive& Ar, BYTE* Data) const;
	UBOOL ShouldSerializeItem(const BYTE* Data, INT Index) const;

	INT GetLinkAlignment() const
	{
		return POINTER_ALIGNMENT;
	}
};

/*-----------------------------------------------------------------------------
	UObjectProperty.
-----------------------------------------------------------------------------*/

//
// Describes a reference variable to another object which may be nil.
//
class CORE_API UClassProperty : public UObjectProperty
{
	DECLARE_CLASS(UClassProperty,UObjectProperty,0,Core)

	// Variables.
	class UClass* MetaClass;

	// Constructors.
	UClassProperty()
	{}
	UClassProperty( ECppProperty, INT InOffset, const TCHAR* InCategory, DWORD InFlags, UClass* InMetaClass, const TCHAR* Desc = NULL)
	:	UObjectProperty( EC_CppProperty, InOffset, InCategory, InFlags, UClass::StaticClass(), Desc)
	,	MetaClass( InMetaClass )
	{}

	// UObject interface.
	void Serialize( FArchive& Ar );
	UBOOL NetSerializeItem( FArchive& Ar, UPackageMap* Map, void* Data ) const;
	void SerializeItem(FArchive& Ar, void* Value) const;

	// UProperty interface.
	const TCHAR* ImportText( const TCHAR* Buffer, BYTE* Data, INT PortFlags ) const;
	void ExportUScriptName(FOutputDevice& Out) const;
	BYTE GetID() const
	{
		return NAME_ObjectProperty;
	}
	UBOOL HasActorRef() // Class references may never be Actor reference!
	{
		return FALSE;
	}
	UBOOL IsAutoDestruct() // Nope.
	{
		return FALSE;
	}
	UBOOL ShouldSerializeItem(const BYTE* Data, INT Index) const
	{
		return TRUE;
	}
	void CleanupAutoDestructions(BYTE* Data) const {}
};

/*-----------------------------------------------------------------------------
	UNameProperty.
-----------------------------------------------------------------------------*/

//
// Describes a name variable pointing into the global name table.
//
class CORE_API UNameProperty : public UProperty
{
	DECLARE_CLASS(UNameProperty,UProperty,0,Core)

	// Constructors.
	UNameProperty()
	{}
	UNameProperty( ECppProperty, INT InOffset, const TCHAR* InCategory, DWORD InFlags, const TCHAR* Desc = NULL)
	:	UProperty( EC_CppProperty, InOffset, InCategory, InFlags, Desc)
	{}

	// UProperty interface.
	void Link( FArchive& Ar, UProperty* Prev );
	UBOOL Identical( const void* A, const void* B ) const;
	void SerializeItem( FArchive& Ar, void* Value ) const;
	void ExportCppItem( FOutputDevice& Out ) const;
	void ExportUScriptName(FOutputDevice& Out) const;
	void ExportTextItem( FString& ValueStr, BYTE* PropertyValue, BYTE* DefaultValue, INT PortFlags ) const;
	const TCHAR* ImportText( const TCHAR* Buffer, BYTE* Data, INT PortFlags ) const;
	void CopySingleValue( void* Dest, void* Src ) const;
	void CopyCompleteValue( void* Dest, void* Src, UObject* Obj=NULL ) const;
	DWORD GetMapHash(BYTE* Data) const;
	UBOOL HasObjectRef()
	{
		return TRUE; // Names are garbage collected too...
	}
	void SerializeObjectRefs(FArchive& Ar, BYTE* Data) const;

	INT GetLinkAlignment() const
	{
		return alignof(FName);
	}
};

/*-----------------------------------------------------------------------------
	UStrProperty.
-----------------------------------------------------------------------------*/

//
// Describes a dynamic string variable.
//
class CORE_API UStrProperty : public UProperty
{
	DECLARE_CLASS(UStrProperty,UProperty,0,Core)

	// Constructors.
	UStrProperty()
	{}
	UStrProperty( ECppProperty, INT InOffset, const TCHAR* InCategory, DWORD InFlags, const TCHAR* Desc = NULL)
	:	UProperty( EC_CppProperty, InOffset, InCategory, InFlags, Desc)
	{}

	// UObject interface.
	void Serialize( FArchive& Ar );

	// UProperty interface.
	void Link( FArchive& Ar, UProperty* Prev );
	UBOOL Identical( const void* A, const void* B ) const;
	void SerializeItem( FArchive& Ar, void* Value ) const;
	UBOOL NetSerializeItem(FArchive& Ar, UPackageMap* Map, void* Data) const;
	void ExportCppItem( FOutputDevice& Out ) const;
	void ExportUScriptName(FOutputDevice& Out) const;
	void ExportTextItem( FString& ValueStr, BYTE* PropertyValue, BYTE* DefaultValue, INT PortFlags ) const;
	const TCHAR* ImportText( const TCHAR* Buffer, BYTE* Data, INT PortFlags ) const;
	void CopySingleValue( void* Dest, void* Src ) const;
	void DestroyValue( void* Dest ) const;
	DWORD GetMapHash(BYTE* Data) const;

	INT GetLinkAlignment() const
	{
		return alignof(FString);
	}

    // Ini safe string writing/reading
    static void SafeWriteString( FString& Dest, const TCHAR* Src );
	static void SafeReadString(FString& Dest, const TCHAR*& Src, UBOOL bProcessDelimitator = TRUE);
};

/*-----------------------------------------------------------------------------
	UFixedArrayProperty.
-----------------------------------------------------------------------------*/

//
// Describes a fixed length array.
//
class CORE_API UFixedArrayProperty : public UProperty
{
	DECLARE_CLASS(UFixedArrayProperty,UProperty,0,Core)

	// Variables.
	UProperty* Inner;
	INT Count;

	// Constructors.
	UFixedArrayProperty()
	{}
	UFixedArrayProperty( ECppProperty, INT InOffset, const TCHAR* InCategory, DWORD InFlags )
	:	UProperty( EC_CppProperty, InOffset, InCategory, InFlags )
	{}

	// UObject interface.
	void Serialize( FArchive& Ar );

	// UProperty interface.
	void Link( FArchive& Ar, UProperty* Prev );
	UBOOL Identical( const void* A, const void* B ) const;
	void SerializeItem( FArchive& Ar, void* Value ) const;
	UBOOL NetSerializeItem( FArchive& Ar, UPackageMap* Map, void* Data ) const;
	void ExportCppItem( FOutputDevice& Out ) const;
	void ExportTextItem( FString& ValueStr, BYTE* PropertyValue, BYTE* DefaultValue, INT PortFlags ) const;
	const TCHAR* ImportText( const TCHAR* Buffer, BYTE* Data, INT PortFlags ) const;
	void CopySingleValue( void* Dest, void* Src ) const;
	void DestroyValue( void* Dest ) const;
	UBOOL HasObjectRef()
	{
		return Inner->HasObjectRef();
	}
	UBOOL HasActorRef()
	{
		return Inner->HasActorRef();
	}
	INT GetLinkAlignment() const
	{
		return Inner->GetLinkAlignment();
	}

	// UFixedArrayProperty interface.
	void AddCppProperty( UProperty* Property, INT Count );
	UBOOL CleanupDestroyed( BYTE* Data ) const;
	UBOOL ClearNetReference(BYTE* Data, UObject* Other) const;
	void SerializeObjectRefs(FArchive& Ar, BYTE* Data) const;
};

/*-----------------------------------------------------------------------------
	UArrayProperty.
-----------------------------------------------------------------------------*/

//
// Describes a dynamic array.
//
class CORE_API UArrayProperty : public UProperty
{
	DECLARE_CLASS(UArrayProperty,UProperty,0,Core)

	// Variables.
	UProperty* Inner;

	// Constructors.
	UArrayProperty()
	{}
	UArrayProperty( ECppProperty, INT InOffset, const TCHAR* InCategory, DWORD InFlags, const TCHAR* Desc = NULL)
	:	UProperty( EC_CppProperty, InOffset, InCategory, InFlags, Desc)
	{}

	// UObject interface.
	void Serialize( FArchive& Ar );

	// UProperty interface.
	void Link( FArchive& Ar, UProperty* Prev );
	UBOOL Identical( const void* A, const void* B ) const;
	void SerializeItem( FArchive& Ar, void* Value ) const;
	UBOOL NetSerializeItem( FArchive& Ar, UPackageMap* Map, void* Data ) const;
	void ExportCppItem( FOutputDevice& Out ) const;
	void ExportUScriptName(FOutputDevice& Out) const;
	void ExportCpp( FOutputDevice& Out, UBOOL IsLocal, UBOOL IsParm  ) const;
	void ExportTextItem( FString& ValueStr, BYTE* PropertyValue, BYTE* DefaultValue, INT PortFlags ) const;
	const TCHAR* ImportText( const TCHAR* Buffer, BYTE* Data, INT PortFlags ) const;
	void CopySingleValue( void* Dest, void* Src ) const;
	void CopyCompleteValue( void* Dest, void* Src, UObject* Obj=NULL ) const;
	void DestroyValue( void* Dest ) const;
	UBOOL HasObjectRef()
	{
		return Inner->HasObjectRef();
	}
	UBOOL HasActorRef()
	{
		return Inner->HasActorRef();
	}
	UBOOL IsAutoDestruct()
	{
		return Inner->IsAutoDestruct();
	}
	INT GetLinkAlignment() const
	{
		return alignof(FArray);
	}

	// UArrayProperty interface.
	void AddCppProperty( UProperty* Property );
	UBOOL CleanupDestroyed( BYTE* Data ) const;
	UBOOL ClearNetReference(BYTE* Data, UObject* Other) const;
	void SerializeObjectRefs(FArchive& Ar, BYTE* Data) const;
	UBOOL ShouldSerializeItem(const BYTE* Data, INT Index) const;
	void CleanupAutoDestructions(BYTE* Data) const;
};

/*-----------------------------------------------------------------------------
	UMapProperty.
-----------------------------------------------------------------------------*/

//
// Describes a dynamic map.
//
class CORE_API UMapProperty : public UProperty
{
	DECLARE_CLASS(UMapProperty,UProperty,0,Core)

	// Variables.
	UProperty* Key;
	UProperty* Value;
	BYTE CleanupDestroyedMask;

	// Constructors.
	UMapProperty()
	{}
	UMapProperty( ECppProperty, INT InOffset, const TCHAR* InCategory, DWORD InFlags, UProperty* K=NULL, UProperty* V=NULL, const TCHAR* Desc = NULL)
	:	UProperty( EC_CppProperty, InOffset, InCategory, InFlags, Desc)
	,	Key(K)
	,	Value(V)
	{}

	// UObject interface.
	void Serialize( FArchive& Ar );

	// UProperty interface.
	void Link( FArchive& Ar, UProperty* Prev );
	UBOOL Identical( const void* A, const void* B ) const;
	void SerializeItem( FArchive& Ar, void* Value ) const;
	UBOOL NetSerializeItem( FArchive& Ar, UPackageMap* Map, void* Data ) const;
	void ExportCppItem( FOutputDevice& Out ) const;
	void ExportUScriptName(FOutputDevice& Out) const;
	void ExportTextItem( FString& ValueStr, BYTE* PropertyValue, BYTE* DefaultValue, INT PortFlags ) const;
	const TCHAR* ImportText( const TCHAR* Buffer, BYTE* Data, INT PortFlags ) const;
	void CopySingleValue( void* Dest, void* Src ) const;
	void CopyCompleteValue(void* Dest, void* Src, UObject* Obj) const;
	void DestroyValue( void* Dest ) const;
	UBOOL CleanupDestroyed(BYTE* Data) const;
	UBOOL ClearNetReference(BYTE* Data, UObject* Other) const;
	void CleanupAutoDestructions(BYTE* Data) const;
	UBOOL HasObjectRef();
	UBOOL HasActorRef();
	UBOOL IsAutoDestruct()
	{
		return Value->IsAutoDestruct();
	}
	void SerializeObjectRefs(FArchive& Ar, BYTE* Data) const;

	INT GetLinkAlignment() const
	{
		return alignof(FScriptMapBase);
	}
};

/*-----------------------------------------------------------------------------
	UStructProperty.
-----------------------------------------------------------------------------*/

//
// Describes a structure variable embedded in (as opposed to referenced by)
// an object.
//
class CORE_API UStructProperty : public UProperty
{
	DECLARE_CLASS(UStructProperty,UProperty,0,Core)

	// Variables.
	class UStruct* Struct;
	UStructProperty* NextStruct;

	// Constructors.
	UStructProperty()
	{}
	UStructProperty( ECppProperty, INT InOffset, const TCHAR* InCategory, DWORD InFlags, UStruct* InStruct, const TCHAR* Desc = NULL )
	:	UProperty( EC_CppProperty, InOffset, InCategory, InFlags, Desc )
	,	Struct( InStruct )
	{}

	// UObject interface.
	void Serialize( FArchive& Ar );

	// UProperty interface.
	void Link( FArchive& Ar, UProperty* Prev );
	UBOOL Identical( const void* A, const void* B ) const;
	void SerializeItem( FArchive& Ar, void* Value ) const;
	UBOOL NetSerializeItem( FArchive& Ar, UPackageMap* Map, void* Data ) const;
	void ExportCppItem( FOutputDevice& Out ) const;
	void ExportUScriptName(FOutputDevice& Out) const;
	void ExportTextItem( FString& ValueStr, BYTE* PropertyValue, BYTE* DefaultValue, INT PortFlags ) const;
	const TCHAR* ImportText( const TCHAR* Buffer, BYTE* Data, INT PortFlags ) const;
	void CopySingleValue( void* Dest, void* Src ) const;
	void CopyCompleteValue( void* Dest, void* Src, UObject* Obj=NULL ) const;
	void DestroyValue( void* Dest ) const;
	UBOOL CleanupDestroyed( BYTE* Data ) const;
	UBOOL ClearNetReference(BYTE* Data, UObject* Other) const;
	void CleanupAutoDestructions(BYTE* Data) const;
	DWORD GetMapHash(BYTE* Data) const;
	UBOOL HasObjectRef();
	UBOOL HasActorRef();
	UBOOL IsAutoDestruct();
	void SerializeObjectRefs(FArchive& Ar, BYTE* Data) const;
	INT GetLinkAlignment() const;
};

/*-----------------------------------------------------------------------------
	UPointerProperty.
-----------------------------------------------------------------------------*/

//
// Describes a C++ pointer
//
class CORE_API UPointerProperty : public UProperty
{
	DECLARE_CLASS(UPointerProperty, UProperty, 0, Core)

	FString CppType;

	// Constructors.
	UPointerProperty()
	{}
	UPointerProperty(const TCHAR* CppText)
		: CppType(CppText)
	{}
	UPointerProperty(ECppProperty, INT InOffset, const TCHAR* InCategory, DWORD InFlags)
		: UProperty(EC_CppProperty, InOffset, InCategory, InFlags)
	{}

	// UProperty interface.
	void Link(FArchive& Ar, UProperty* Prev);
	UBOOL Identical(const void* A, const void* B) const;
	void SerializeItem(FArchive& Ar, void* Value) const;
	UBOOL NetSerializeItem(FArchive& Ar, UPackageMap* Map, void* Data) const;
	void ExportCppItem(FOutputDevice& Out) const;
	void ExportUScriptName(FOutputDevice& Out) const;
	void ExportTextItem(FString& ValueStr, BYTE* PropertyValue, BYTE* DefaultValue, INT PortFlags) const;
	const TCHAR* ImportText(const TCHAR* Buffer, BYTE* Data, INT PortFlags) const;
	void CopySingleValue(void* Dest, void* Src) const;
	void CopyCompleteValue(void* Dest, void* Src) const;
	DWORD GetMapHash(BYTE* Data) const;
	INT GetLinkAlignment() const
	{
		return POINTER_ALIGNMENT;
	}

	// stijn: UPackages encode the raw (i.e., on-disk) type of UProperties in a
	// 4-bit tag. The GetID() function, which returns the raw type, must therefore
	// return a value between 0 and 15. Unfortunately, UE1 already defined types
	// for the entire 0-15 range before we added UPointerProperty. Luckily, it
	// doesn't really hurt to "impersonate" intproperties here as it only affects
	// the on-disk representation of the pointer.
	BYTE GetID() const
	{
		return NAME_IntProperty;
	}
};

/*-----------------------------------------------------------------------------
	UButtonProperty.
-----------------------------------------------------------------------------*/

//
// A button for properties window.
//
class CORE_API UButtonProperty : public UProperty
{
	DECLARE_CLASS(UButtonProperty, UProperty, 0, Core)

	FString Caption;

	// Constructors.
	UButtonProperty()
	{}
	UButtonProperty(const TCHAR* Capt)
	:	Caption(Capt)
	{}
	UButtonProperty(ECppProperty, INT InOffset, const TCHAR* InCategory, DWORD InFlags, const TCHAR* Capt, const TCHAR* Desc = NULL)
	:	UProperty(EC_CppProperty, InOffset, InCategory, InFlags, Desc)
	,	Caption(Capt)
	{}

	// UObject interface.
	void Serialize(FArchive& Ar);

	// UProperty interface.
	void Link(FArchive& Ar, UProperty* Prev);
	UBOOL Identical(const void* A, const void* B) const;
	void SerializeItem(FArchive& Ar, void* Value) const;
	UBOOL NetSerializeItem(FArchive& Ar, UPackageMap* Map, void* Data) const;
	void ExportCppItem(FOutputDevice& Out) const;
	void ExportUScriptName(FOutputDevice& Out) const;
	void ExportTextItem(FString& ValueStr, BYTE* PropertyValue, BYTE* DefaultValue, INT PortFlags) const;
	const TCHAR* ImportText(const TCHAR* Buffer, BYTE* Data, INT PortFlags) const;
	void CopySingleValue(void* Dest, void* Src) const;
	void CopyCompleteValue(void* Dest, void* Src) const;
	INT GetLinkAlignment() const
	{
		return 0;
	}

	BYTE GetID() const
	{
		return NAME_IntProperty;
	}
};

/*-----------------------------------------------------------------------------
	UAnyProperty.
-----------------------------------------------------------------------------*/

//
// An arbitary property type that can be anything.
//
class CORE_API UAnyProperty : public UProperty
{
	DECLARE_CLASS(UAnyProperty, UProperty, 0, Core)

	// Constructors.
	UAnyProperty()
	{}
	UAnyProperty(ECppProperty, INT InOffset, const TCHAR* InCategory, DWORD InFlags, const TCHAR* Desc = NULL)
	:	UProperty(EC_CppProperty, InOffset, InCategory, InFlags, Desc)
	{}

	// UProperty interface.
	void Link(FArchive& Ar, UProperty* Prev);
	UBOOL Identical(const void* A, const void* B) const;
	void SerializeItem(FArchive& Ar, void* Value) const;
	UBOOL NetSerializeItem(FArchive& Ar, UPackageMap* Map, void* Data) const;
	void ExportCppItem(FOutputDevice& Out) const;
	void ExportUScriptName(FOutputDevice& Out) const;
	void ExportTextItem(FString& ValueStr, BYTE* PropertyValue, BYTE* DefaultValue, INT PortFlags) const;
	const TCHAR* ImportText(const TCHAR* Buffer, BYTE* Data, INT PortFlags) const;
	void CopySingleValue(void* Dest, void* Src) const;
	DWORD GetMapHash(BYTE* Data) const;
	UBOOL HasObjectRef()
	{
		return TRUE; // Can never be sure with this one.
	}
	UBOOL HasActorRef()
	{
		return TRUE;
	}
	void DestroyValue(void* Dest) const;
	UBOOL CleanupDestroyed(BYTE* Data) const;
	UBOOL ClearNetReference(BYTE* Data, UObject* Other) const;
	void SerializeObjectRefs(FArchive& Ar, BYTE* Data) const;
	INT GetLinkAlignment() const
	{
		return alignof(FAnyProperty*);
	}

	BYTE GetID() const
	{
		return NAME_VectorProperty; // NOTE: Reuse VectorProperty name because its not actually used in serialize bits.
	}
};

/*-----------------------------------------------------------------------------
	Field templates.
-----------------------------------------------------------------------------*/

//
// Find a typed field in a struct.
//
template <class T> T* FindField( UStruct* Owner, const TCHAR* FieldName )
{
	guard(FindField);
	for( TFieldIterator<T>It( Owner ); It; ++It )
		if( appStricmp( It->GetName(), FieldName )==0 )
			return *It;
	return NULL;
	unguard;
}

template <class T> T* FindOuter(UObject* Obj,BYTE bOuterMost=0)
{
	guard(FindOuter);
	if (bOuterMost)
	{
		T* Best = NULL;
		while (Obj)
		{
			if (Obj->IsA(T::StaticClass()))
				Best = (T*)Obj;
			Obj = Obj->GetOuter();
		}
		return Best;
	}
	else
	{
		while (Obj)
		{
			if (Obj->IsA(T::StaticClass()))
				return (T*)Obj;
			Obj = Obj->GetOuter();
		}
	}
	return NULL;
	unguard;
}

/*-----------------------------------------------------------------------------
	UObject accessors that depend on UClass.
-----------------------------------------------------------------------------*/

//
// See if this object belongs to the specified class.
//
inline UBOOL UObject::IsA( class UClass* SomeBase ) const
{
	guardSlow(UObject::IsA);
	if( SomeBase && SomeBase->CastFlags && Class  && !Class->IsClassType(SomeBase->CastFlags) )
	{
		//debugf(TEXT("Skip %ls in %ls check."),SomeBase->GetName(),Class->GetName());
		return 0;
	}
	for( UClass* TempClass=Class; TempClass; TempClass=(UClass*)TempClass->SuperField )
		if( TempClass==SomeBase )
			return 1;
	return SomeBase==NULL;
	unguardobjSlow;
}

//
// See if this object is in a certain package.
//
inline UBOOL UObject::IsIn( class UObject* SomeOuter ) const
{
	guardSlow(UObject::IsIn);
	for( UObject* It=GetOuter(); It; It=It->GetOuter() )
		if( It==SomeOuter )
			return 1;
	return SomeOuter==NULL;
	unguardobjSlow;
}

/*-----------------------------------------------------------------------------
	UStruct inlines.
-----------------------------------------------------------------------------*/

//
// UStruct inline comparer.
//
inline UBOOL UStruct::StructCompare( const void* A, const void* B )
{
	guardSlow(UStruct::StructCompare);
	for( TFieldIterator<UProperty> It(this); It; ++It )
		for( INT i=0; i<It->ArrayDim; i++ )
			if( !It->Matches(A,B,i) )
				return 0;
	unguardobjSlow;
	return 1;
}

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*-----------------------------------------------------------------------------
	C++ property macros.
-----------------------------------------------------------------------------*/

// The below, using offset 0 (NULL), makes gcc think we're misusing
//  the offsetof macro...we get around this by using 1 instead.  :) --ryan.
/*
#define CPP_PROPERTY(name) \
	EC_CppProperty, (BYTE*)&((ThisClass*)NULL)->name - (BYTE*)NULL
*/

#define CPP_PROPERTY(name) \
	EC_CppProperty, (BYTE*)&((ThisClass*)1)->name - (BYTE*)1

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
