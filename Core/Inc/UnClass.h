/*=============================================================================
	UnClass.h: UClass definition.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney.
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

/*-----------------------------------------------------------------------------
	FDependency.
-----------------------------------------------------------------------------*/

//
// One dependency record, for incremental compilation.
//
class CORE_API FDependency
{
public:
	// Variables.
	UClass*		Class;
	UBOOL		Deep;
	DWORD		ScriptTextCRC;

	// Functions.
	FDependency();
	FDependency( UClass* InClass, UBOOL InDeep );
	UBOOL IsUpToDate();
	CORE_API friend FArchive& operator<<( FArchive& Ar, FDependency& Dep );
};

/*-----------------------------------------------------------------------------
	FRepLink.
-----------------------------------------------------------------------------*/

//
// A tagged linked list of replicatable variables.
//
class FRepLink
{
public:
	UProperty*	Property;		// Replicated property.
	FRepLink*	Next;			// Next replicated link per class.
	FRepLink( UProperty* InProperty, FRepLink* InNext )
	:	Property	(InProperty)
	,	Next		(InNext)
	{}
};

//
// Information about a property to replicate.
//
struct FRepRecord
{
	UProperty* Property;
	INT Index;
	FRepRecord(UProperty* InProperty, INT InIndex)
		: Property(InProperty), Index(InIndex)
	{}
};

/*-----------------------------------------------------------------------------
	FLabelEntry.
-----------------------------------------------------------------------------*/

//
// Entry in a state's label table.
//
struct CORE_API FLabelEntry
{
	// Variables.
	FName	Name;
	INT		iCode;

	// Functions.
	FLabelEntry( FName InName, INT iInCode );
	CORE_API friend FArchive& operator<<( FArchive& Ar, FLabelEntry &Label );
};

/*-----------------------------------------------------------------------------
	UField.
-----------------------------------------------------------------------------*/

//
// Base class of UnrealScript language objects.
//
class CORE_API UField : public UObject
{
	DECLARE_ABSTRACT_CLASS(UField,UObject,0,Core)
	NO_DEFAULT_CONSTRUCTOR(UField)

	// Constants.
	enum {HASH_COUNT = 256};

	// Variables.
	UField*			SuperField;
	UField*			Next;
	UField*			HashNext;

	// Constructors.
	UField( ENativeConstructor, UClass* InClass, const TCHAR* InName, const TCHAR* InPackageName, DWORD InFlags, UField* InSuperField );
	UField( EStaticConstructor, const TCHAR* InName, const TCHAR* InPackageName, DWORD InFlags );
	UField( UField* InSuperField );

	// UObject interface.
	void Serialize( FArchive& Ar );
	void PostLoad();
	void Register();

	// UField interface.
	virtual void AddCppProperty( UProperty* Property );
	virtual UBOOL MergeBools();
	virtual void Bind();
	virtual UClass* GetOwnerClass();
	virtual INT GetPropertiesSize();
	virtual UField* GetSerializedSuper() const;
};

/*-----------------------------------------------------------------------------
	UStruct.
-----------------------------------------------------------------------------*/

typedef void (*StructNetSer)(class FArchive&, class UPackageMap*, void*);

//
// An UnrealScript structure definition.
//
class CORE_API UStruct : public UField
{
	DECLARE_CLASS(UStruct,UField,0,Core)
	NO_DEFAULT_CONSTRUCTOR(UStruct)

	// Variables.
	UTextBuffer*		ScriptText;
	UField*				Children;
	INT					PropertiesSize;
	INT					PropertiesAlign;
	INT					SerializedSize;
	FName				FriendlyName;
	TArray<BYTE>		Script;

	// Run-time hardcoded references in code.
	TArray<FName>		ScriptNames;
	TArray<UObject*>	ScriptObjects;

	// Compiler info.
	INT					TextPos;
	INT					Line;
	FString				CppExport;

	// In memory only.
	UProperty*			RefLink;
	UProperty*			ActorRefLink;
	UProperty*			NetActorRefLink;
	UProperty*			PropertyLink;
	UProperty*			ConfigLink;
	UProperty*			ConstructorLink;
	UProperty*			AutoDestructLink;
	INT					LinkedRefs;
	StructNetSer		NetSerializer;
	INT					WarningCount; // Number of ScriptWarnings this nodie printed.
	UBOOL				bCodeLoadError; // This function had loading errors.

	// Constructors.
	UStruct( ENativeConstructor, INT InSize, const TCHAR* InName, const TCHAR* InPackageName, DWORD InFlags, UStruct* InSuperStruct );
	UStruct( EStaticConstructor, INT InSize, const TCHAR* InName, const TCHAR* InPackageName, DWORD InFlags );
	UStruct( UStruct* InSuperStruct );

	// UObject interface.
	void Serialize( FArchive& Ar );
	void Destroy();
	void Register();

	// UField interface.
	void AddCppProperty( UProperty* Property );
	void Bind();

	// UStruct interface.
	virtual UStruct* GetInheritanceSuper() {return GetSuperStruct();}
	virtual void Link( FArchive& Ar, UBOOL Props );
	virtual void SerializeBin( FArchive& Ar, BYTE* Data );
	virtual void SerializeTaggedProperties(FArchive& Ar, BYTE* Data, UClass* DefaultsClass, BYTE* Defaults = NULL, INT DefaultsCount = 0);
	virtual void SerializeRefs(FArchive& Ar, BYTE* Data);
	virtual UBOOL CleanupDestroyed( BYTE* Data );
	virtual UBOOL ClearNetReference(BYTE* Data, UObject* Other);
	virtual EExprToken SerializeExpr( INT& iCode, FArchive& Ar );
	INT GetPropertiesSize()
	{
		return PropertiesSize;
	}
	INT GetSerializedSize()
	{
		return SerializedSize;
	}
	DWORD GetScriptTextCRC()
	{
		return ScriptText ? appStrCrc(*ScriptText->Text) : 0;
	}
	void SetPropertiesSize( INT NewSize )
	{
		PropertiesSize = NewSize;
	}
	UBOOL IsChildOf( const UStruct* SomeBase ) const
	{
		guardSlow(UStruct::IsChildOf);
		for( const UStruct* Struct=this; Struct; Struct=Struct->GetSuperStruct() )
			if( Struct==SomeBase )
				return 1;
		return 0;
		unguardobjSlow;
	}
	virtual TCHAR* GetNameCPP()
	{
		TCHAR* Result = appStaticString1024();
		if (GetFName() == NAME_QWORD || GetFName() == NAME_DOUBLE)
			appStrcpy(Result, GetName());
		else appSprintf( Result, TEXT("F%ls"), GetName() );
		return Result;
	}
	UStruct* GetSuperStruct() const
	{
		guardSlow(UStruct::GetSuperStruct);
		//checkSlow(!SuperField);
		return (UStruct*)SuperField;
		unguardSlow;
	}
	UBOOL StructCompare( const void* A, const void* B );
	void CreateRefLinks();
};

/*-----------------------------------------------------------------------------
	TFieldIterator.
-----------------------------------------------------------------------------*/

//
// For iterating through a linked list of fields.
//
template <class T> class TFieldIterator
{
public:
	TFieldIterator( UStruct* InStruct )
	: Struct( InStruct )
	, Field( InStruct ? InStruct->Children : NULL )
	{
		IterateToNext();
	}
	operator UBOOL()
	{
		return Field != NULL;
	}
	void operator++()
	{
		checkSlow(Field);
		Field = Field->Next;
		IterateToNext();
	}
	T* operator*()
	{
		checkSlow(Field);
		return (T*)Field;
	}
	T* operator->()
	{
		checkSlow(Field);
		return (T*)Field;
	}
	UStruct* GetStruct()
	{
		return Struct;
	}
protected:
	void IterateToNext()
	{
		while( Struct )
		{
			while( Field )
			{
				if( Field->IsA(T::StaticClass()) )
					return;
				Field = Field->Next;
			}
			Struct = Struct->GetInheritanceSuper();
			if( Struct )
				Field = Struct->Children;
		}
	}
	UStruct* Struct;
	UField* Field;
};

/*-----------------------------------------------------------------------------
	UFunction.
-----------------------------------------------------------------------------*/

//
// An UnrealScript function.
//
class CORE_API UFunction : public UStruct
{
	DECLARE_CLASS(UFunction,UStruct,CLASS_SafeReplace,Core)
	DECLARE_WITHIN(UState)
	NO_DEFAULT_CONSTRUCTOR(UFunction)

	// Persistent variables.
	DWORD FunctionFlags;
	_WORD iNative;
	_WORD RepOffset;
	BYTE  OperPrecedence;

	// Variables in memory only.
	BYTE  NumParms;
	UFunction* NetworkRef GCC_PACK(INT_ALIGNMENT);
	UFunction* TopMostRef;
	_WORD ParmsSize;
	_WORD ReturnValueOffset;
	void (UObject::*Func)( FFrame& TheStack, RESULT_DECL );
#if DO_GUARD_SLOW
	SQWORD Calls,Cycles;
#endif

	// Hooks
	struct FCallHook
	{
		UFunction* Func;
		class UScriptHook* Obj;
		INT Priority;
		FCallHook(UFunction* F, UScriptHook* O, INT Pri)
			: Func(F), Obj(O), Priority(Pri)
		{}
	};
	TArray<FCallHook> CallHookList;

	// Constructors.
	UFunction( UFunction* InSuperFunction );

	// UObject interface.
	void Serialize( FArchive& Ar );
	void PostLoad();
	void Destroy();

	// UField interface.
	void Bind();
	UField* GetSerializedSuper() const;

	// UStruct interface.
	UBOOL MergeBools() {return 0;}
	UStruct* GetInheritanceSuper() {return NULL;}
	void Link( FArchive& Ar, UBOOL Props );

	// UFunction interface.
	UFunction* GetSuperFunction() const
	{
		guardSlow(UFunction::GetSuperFunction);
		checkSlow(!SuperField||SuperField->IsA(UFunction::StaticClass()));
		return (UFunction*)SuperField;
		unguardSlow;
	}
	UProperty* GetReturnProperty();
	UBOOL ProcessHooks(FFrame& Stack, RESULT_DECL);
	static void RemoveAllHooks();
};

/*-----------------------------------------------------------------------------
	UState.
-----------------------------------------------------------------------------*/

//
// An UnrealScript state.
//
class CORE_API UState : public UStruct
{
	DECLARE_CLASS(UState,UStruct,0,Core)
	NO_DEFAULT_CONSTRUCTOR(UState)

	// Variables.
	QWORD ProbeMask;
	QWORD IgnoreMask;
	DWORD StateFlags;
	_WORD LabelTableOffset;
	UField* VfHash[HASH_COUNT];
	TMap<FName, INT> StateLabels;

	// Constructors.
	UState( ENativeConstructor, INT InSize, const TCHAR* InName, const TCHAR* InPackageName, DWORD InFlags, UState* InSuperState );
	UState( EStaticConstructor, INT InSize, const TCHAR* InName, const TCHAR* InPackageName, DWORD InFlags );
	UState( UState* InSuperState );

	// UObject interface.
	void Serialize( FArchive& Ar );
	void Destroy();

	// UStruct interface.
	UBOOL MergeBools() {return 1;}
	UStruct* GetInheritanceSuper() {return GetSuperState();}
	void Link( FArchive& Ar, UBOOL Props );

	// UState interface.
	UState* GetSuperState() const
	{
		guardSlow(UState::GetSuperState);
		checkSlow(SuperField==nullptr||SuperField->IsA(UState::StaticClass()));
		return (UState*)SuperField;
		unguardSlow;
	}
};

/*-----------------------------------------------------------------------------
	UEnum.
-----------------------------------------------------------------------------*/

//
// An enumeration, a list of names usable by UnrealScript.
//
class CORE_API UEnum : public UField
{
	DECLARE_CLASS(UEnum,UField,0,Core)
	DECLARE_WITHIN(UStruct)
	NO_DEFAULT_CONSTRUCTOR(UEnum)

	// Variables.
	TArray<FName> Names;

	// Constructors.
	UEnum( UEnum* InSuperEnum );

	// UObject interface.
	void Serialize( FArchive& Ar );

	// UEnum interface.
	UEnum* GetSuperEnum() const
	{
		guardSlow(UEnum::GetSuperEnum);
		checkSlow(!SuperField||SuperField->IsA(UEnum::StaticClass()));
		return (UEnum*)SuperField;
		unguardSlow;
	}
};

/*-----------------------------------------------------------------------------
	UClass.
-----------------------------------------------------------------------------*/

//
// An object class.
//
class CORE_API UClass : public UState
{
	DECLARE_CLASS(UClass,UState,0,Core)
	DECLARE_WITHIN(UPackage)

	// Variables.
	DWORD				ClassFlags;
	INT					ClassUnique;
	FGuid				ClassGuid;
	UClass*				ClassWithin;
	FName				ClassConfigName;
	TArray<FRepRecord>	ClassReps;
	TArray<UField*>		NetFields;
	TArray<FDependency> Dependencies;
	TArray<FName>		PackageImports;
	TArray<BYTE>		Defaults;
	TArray<FName>       DependentOn;
	void(*ClassConstructor)(void*);
	void(UObject::*ClassStaticConstructor)();
	_WORD CastFlags;
	UBOOL bEditorExpanded; // Editor use only.

	// In memory only.
	FString				DefaultPropText, CppExtension;

	// Constructors.
	UClass();
	UClass( UClass* InSuperClass );
	UClass( ENativeConstructor, DWORD InSize, DWORD InClassFlags, UClass* InBaseClass, UClass* InWithinClass, FGuid InGuid, const TCHAR* InNameStr, const TCHAR* InPackageName, const TCHAR* InClassConfigName, DWORD InFlags, void(*InClassConstructor)(void*), void(UObject::*InClassStaticConstructor)() );
	UClass( EStaticConstructor, DWORD InSize, DWORD InClassFlags, FGuid InGuid, const TCHAR* InNameStr, const TCHAR* InPackageName, const TCHAR* InClassConfigName, DWORD InFlags, void(*InClassConstructor)(void*), void(UObject::*InClassStaticConstructor)() );

	// UObject interface.
	void Serialize( FArchive& Ar );
	void PostLoad();
	void Destroy();
	void Register();

	// UField interface.
	void Bind();

	// UStruct interface.
	UBOOL MergeBools() {return 1;}
	UStruct* GetInheritanceSuper() {return GetSuperClass();}
	TCHAR* GetNameCPP()
	{
		TCHAR* Result = appStaticString1024();
		UClass* C;
		for( C=this; C; C=C->GetSuperClass() )
			if( appStricmp(C->GetName(),TEXT("Actor"))==0 )
				break;
		appSprintf( Result, TEXT("%ls%ls"), C ? TEXT("A") : TEXT("U"), GetName() );
		return Result;
	}
	void Link( FArchive& Ar, UBOOL Props );

	// UClass interface.
	void AddDependency( UClass* InClass, UBOOL InDeep )
	{
		guard(UClass::AddDependency);
		INT i;
		for( i=0; i<Dependencies.Num(); i++ )
			if( Dependencies(i).Class==InClass )
				Dependencies(i).Deep |= InDeep;
		if( i==Dependencies.Num() )
			new(Dependencies)FDependency( InClass, InDeep );
		unguard;
	}
	UClass* GetSuperClass() const
	{
		guardSlow(UClass::GetSuperClass);
		return (UClass *)SuperField;
		unguardSlow;
	}
	UObject* GetDefaultObject()
	{
		guardSlow(UClass::GetDefaultObject);
		check(Defaults.Num()==GetPropertiesSize());
		return (UObject*)&Defaults(0);
		unguardobjSlow;
	}
	class AActor* GetDefaultActor()
	{
		guardSlow(UClass::GetDefaultActor);
		check(Defaults.Num());
		return (AActor*)&Defaults(0);
		unguardobjSlow;
	}
	BYTE IsSecurityClass();

	inline BYTE IsClassType( _WORD TypeFlags ) const
	{
		return ((TypeFlags!=CLCAST_None && CastFlags!=CLCAST_None) ? ((CastFlags & TypeFlags)==TypeFlags) : 1);
	}
	UBOOL IsChildOf( const UStruct* SomeClass ) const
	{
		guardSlow(UClass::IsChildOf);

		if( !GIsEditor && SomeClass && SomeClass->GetClass()==UClass::StaticClass() && !IsClassType(((UClass*)SomeClass)->CastFlags) )
		{
			//debugf(TEXT("ChildOfSkip %ls in %ls check."),SomeClass->GetName(),GetName());
			return 0;
		}
		return UStruct::IsChildOf(SomeClass);
		unguardobjSlow;
	}
	UBOOL IsChildOfSlow( const UClass* SomeClass ) const
	{
		guardSlow(UClass::IsChildOfSlow);
		return UStruct::IsChildOf(SomeClass);
		unguardobjSlow;
	}

private:
	// Hide IsA because calling IsA on a class almost always indicates
	// an error where the caller should use IsChildOf.
	UBOOL IsA( UClass* Parent ) const {return UObject::IsA(Parent);}
};

/*-----------------------------------------------------------------------------
	UConst.
-----------------------------------------------------------------------------*/

//
// An UnrealScript constant.
//
class CORE_API UConst : public UField
{
	DECLARE_CLASS(UConst,UField,0,Core)
	DECLARE_WITHIN(UStruct)
	NO_DEFAULT_CONSTRUCTOR(UConst)

	// Variables.
	FString Value;

	// Constructors.
	UConst( UConst* InSuperConst, const TCHAR* InValue );

	// UObject interface.
	void Serialize( FArchive& Ar );

	// UConst interface.
	UConst* GetSuperConst() const
	{
		guardSlow(UConst::GetSuperStruct);
		checkSlow(!SuperField||SuperField->IsA(UConst::StaticClass()));
		return (UConst*)SuperField;
		unguardSlow;
	}
};

/*----------------------------------------------------------------------------
	Core templates.
----------------------------------------------------------------------------*/

template< class T > T* CastFastUnverified(UObject* Src, _WORD CastFlags)
{
	return Src && (Src->GetClass()->CastFlags & CastFlags) ? (T*)Src : NULL;
}

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
