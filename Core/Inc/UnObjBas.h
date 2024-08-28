/*=============================================================================
	UnObjBas.h: Unreal object base class.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

/*-----------------------------------------------------------------------------
	Core enumerations.
-----------------------------------------------------------------------------*/

//
// Flags for loading objects.
//
enum ELoadFlags : DWORD
{
	LOAD_None			= 0x0000,	// No flags.
	LOAD_NoFail			= 0x0001,	// Critical error if load fails.
	LOAD_NoWarn			= 0x0002,	// Don't display warning if load fails.
	LOAD_Throw			= 0x0008,	// Throw exceptions upon failure.
	LOAD_Verify			= 0x0010,	// Only verify existance; don't actually load.
	LOAD_AllowDll		= 0x0020,	// Allow plain DLLs.
	LOAD_DisallowFiles  = 0x0040,	// Don't load from file.
	LOAD_NoVerify       = 0x0080,   // Don't verify imports yet.
	LOAD_Forgiving      = 0x1000,   // Forgive missing imports (set them to NULL).
	LOAD_Quiet			= 0x2000,   // No log warnings.
	LOAD_NoRemap        = 0x4000,   // No remapping of packages.
	LOAD_NoPrivate		= 0x8000,   // Load package without throwing private object errors.
	LOAD_WarnFileMiss	= 0x10000,
	LOAD_ForceSaveFile	= 0x20000,	// We know for a fact that this package is a saved game.
	LOAD_WarnReplace	= 0x40000,	// Throw a warning every time we replace an object on load.
	LOAD_Propagate      = (LOAD_NoPrivate | LOAD_Forgiving),
};

//
// Package flags.
//
enum EPackageFlags : DWORD
{
	PKG_AllowDownload	= 0x0001,	// Allow downloading package.
	PKG_ClientOptional  = 0x0002,	// Purely optional for clients.
	PKG_ServerSideOnly  = 0x0004,   // Only needed on the server side.
	PKG_BrokenLinks     = 0x0008,   // Loaded from linker with broken import links.
	PKG_Unsecure        = 0x0010,   // Not trusted.
	PKG_DeltaCompress	= 0x0020,   // Delta saved.
	PKG_Need			= 0x8000,	// Client needs to download this package.
};

//
// Internal enums.
//
enum ENativeConstructor    {EC_NativeConstructor};
enum EStaticConstructor    {EC_StaticConstructor};
enum EInternal             {EC_Internal};
enum ECppProperty          {EC_CppProperty};
enum EInPlaceConstructor   {EC_InPlaceConstructor};

//
// Result of GotoState.
//
enum EGotoState
{
	GOTOSTATE_NotFound		= 0,
	GOTOSTATE_Success		= 1,
	GOTOSTATE_Preempted		= 2,
};

//
// Flags describing a class.
//
enum EClassFlags : DWORD
{
	// Base flags.
	CLASS_Abstract          = 0x00001,  // Class is abstract and can't be instantiated directly.
	CLASS_Compiled			= 0x00002,  // Script has been compiled successfully.
	CLASS_Config			= 0x00004,  // Load object configuration at construction time.
	CLASS_Transient			= 0x00008,	// This object type can't be saved; null it out at save time.
	CLASS_Parsed            = 0x00010,	// Successfully parsed.
	CLASS_Localized         = 0x00020,  // Class contains localized text.
	CLASS_SafeReplace       = 0x00040,  // Objects of this class can be safely replaced with default or NULL.
	CLASS_RuntimeStatic     = 0x00080,	// Objects of this class are static during gameplay.
	CLASS_NoExport          = 0x00100,  // Don't export to C++ header.
	CLASS_NoUserCreate      = 0x00200,  // Don't allow users to create in the editor.
	CLASS_PerObjectConfig   = 0x00400,  // Handle object configuration on a per-object basis, rather than per-class.
	CLASS_NativeReplication = 0x00800,  // Replication handled in C++.
	CLASS_NeedsDefProps     = 0x01000,  // Defaultproperties needs to be imported yet.
	CLASS_EditorOnly		= 0x02000,  // Object should only exist in editor.
	CLASS_ClientOnly		= 0x04000,  // Object should not exist in dedicated server.

	// Flags to inherit from base class.
	CLASS_Inherit           = CLASS_Transient | CLASS_Config | CLASS_Localized | CLASS_SafeReplace | CLASS_PerObjectConfig,
	CLASS_RecompilerClear   = CLASS_Inherit | CLASS_Abstract | CLASS_NoExport | CLASS_NativeReplication,
};

//
// Flags associated with each property in a class, overriding the
// property's default behavior.
//
enum EPropertyFlags : DWORD
{
	// Regular flags.
	CPF_Edit		 = 0x00000001, // Property is user-settable in the editor.
	CPF_Const		 = 0x00000002, // Actor's property always matches class's default actor property.
	CPF_Input		 = 0x00000004, // Variable is writable by the input system.
	CPF_ExportObject = 0x00000008, // Object can be exported with actor.
	CPF_OptionalParm = 0x00000010, // Optional parameter (if CPF_Param is set).
	CPF_Net			 = 0x00000020, // Property is relevant to network replication.
	CPF_ConstRef     = 0x00000040, // Reference to a constant object.
	CPF_Parm		 = 0x00000080, // Function/When call parameter.
	CPF_OutParm		 = 0x00000100, // Value is copied out after function call.
	CPF_SkipParm	 = 0x00000200, // Property is a short-circuitable evaluation function parm.
	CPF_ReturnParm	 = 0x00000400, // Return value.
	CPF_CoerceParm	 = 0x00000800, // Coerce args into this function parameter.
	CPF_Native       = 0x00001000, // Property is native: C++ code is responsible for serializing it.
	CPF_Transient    = 0x00002000, // Property is transient: shouldn't be saved, zero-filled at load time.
	CPF_Config       = 0x00004000, // Property should be loaded/saved as permanent profile.
	CPF_Localized    = 0x00008000, // Property should be loaded as localizable text.
	CPF_Travel       = 0x00010000, // Property travels across levels/servers.
	CPF_EditConst    = 0x00020000, // Property is uneditable in the editor.
	CPF_GlobalConfig = 0x00040000, // Load config from base class, not subclass.
	CPF_OnDemand     = 0x00100000, // Object or dynamic array loaded on demand only.
	CPF_New			 = 0x00200000, // Automatically create inner object.
	CPF_NeedCtorLink = 0x00400000, // Fields need construction/destruction.
	CPF_EditInLine   = 0x00800000, // Property is editinline.
	CPF_AutoDestruct = 0x01000000, // Editinline property is automatically destructed.
	CPF_NoDuplicate  = 0x02000000, // Don't duplicate editinline object when instanced.
	CPF_NoEdSave	 = 0x04000000, // Don't serialize this property in editor.
	CPF_LazyArray    = 0x08000000, // This dynamic array is a lazy array.
	CPF_NoRepNotify  = 0x10000000, // This property shouldn't trigger PostNetReceive event.
	CPF_RepNotify    = CPF_OnDemand, // This property should trigger a custom net notify event.

	// Temporary flags.
	CPF_Replicated   = 0x80000000, // Replication condition is true.

	// Combinations of flags.
	CPF_ParmFlags           = CPF_OptionalParm | CPF_Parm | CPF_OutParm | CPF_SkipParm | CPF_ReturnParm | CPF_CoerceParm,
	CPF_PropagateFromStruct = CPF_Const | CPF_Native | CPF_Transient,
};

// 227j: Compile-time property flags.
enum ETempPropertyFlags : DWORD
{
	TCPF_NoWarn		= 0x00000001, // Don't throw 'X' obscures 'Y' defined in base class 'Z' warnings.
	TCPF_Protected	= 0x00000002, // Variable is constant, but accessable from main class itself.
	TCPF_UnsignInt	= 0x00000004, // This integer should export as unsigned integer in C++ codes.
	TCPF_DWORDInt	= 0x00000008, // This integer should export as DWORD in C++ codes.
	TCPF_Bitmasks	= 0x00000010, // Upcoming series of booleans are bitmask grouped.
};

//
// Flags describing an object instance.
//
enum EObjectFlags : DWORD
{
	RF_Transactional    = 0x00000001,   // Object is transactional.
	RF_Unreachable		= 0x00000002,	// Object is not reachable on the object graph.
	RF_Public			= 0x00000004,	// Object is visible outside its package.
	RF_TagImp			= 0x00000008,	// Temporary import tag in load/save.
	RF_TagExp			= 0x00000010,	// Temporary export tag in load/save.
	RF_SourceModified   = 0x00000020,   // Modified relative to source files.
	RF_TagGarbage		= 0x00000040,	// Check during garbage collection.
	//RF_WindowInit		= 0x00000080,	// Object was created in UWindows. NOTE: Unused bit!
	//RF_ExportAll		= 0x00000100,	// Keep everything during export. Used for UPackageFlagCommandlet. Better not use this otherwise. 227j Use bAutosave bit 2 instead.
	RF_HasArchtype		= 0x00000100,   // Has an archetype reference.
	
	//
	RF_NeedLoad			= 0x00000200,   // During load, indicates object needs loading.
	RF_HighlightedName  = 0x00000400,	// A hardcoded name which should be syntax-highlighted.
	RF_EliminateObject  = 0x00000400,   // NULL out references to this during garbage collecion.
	RF_InSingularFunc   = 0x00000800,	// In a singular function.
	RF_RemappedName     = 0x00000800,   // Name is remapped.
	RF_Suppress         = 0x00001000,	// warning: Mirrored in UnName.h. Suppressed log name.
	RF_StateChanged     = 0x00001000,   // Object did a state change.
	RF_InEndState       = 0x00002000,   // Within an EndState call.
	RF_Transient        = 0x00004000,	// Don't save object.
	RF_Preloading       = 0x00008000,   // Data is being preloaded from file.
	RF_LoadForClient	= 0x00010000,	// In-file load for client.
	RF_LoadForServer	= 0x00020000,	// In-file load for client.
	RF_LoadForEdit		= 0x00040000,	// In-file load for client.
	RF_Standalone       = 0x00080000,   // Keep object around for editing even if unreferenced.
	RF_NotForClient		= 0x00100000,	// Don't load this object for the game client.
	RF_NotForServer		= 0x00200000,	// Don't load this object for the game server.
	RF_NotForEdit		= 0x00400000,	// Don't load this object for the editor.
	RF_Destroyed        = 0x00800000,	// Object Destroy has already been called.
	RF_NeedPostLoad		= 0x01000000,   // Object needs to be postloaded.
	RF_HasStack         = 0x02000000,	// Has execution stack.
	RF_Native			= 0x04000000,   // Native (UClass only).
	RF_Marked			= 0x08000000,   // Marked (for debugging).
	RF_ErrorShutdown    = 0x10000000,	// ShutdownAfterError called.
	RF_DebugPostLoad    = 0x20000000,   // For debugging Serialize calls.
	RF_PendingDelete	= 0x40000000,   // Object is marked for deletion (replace pending delete VF).
	RF_DebugDestroy		= 0x80000000,   // For debugging Destroy calls.
	RF_ContextFlags		= RF_NotForClient | RF_NotForServer | RF_NotForEdit, // All context flags.
	RF_LoadContextFlags	= RF_LoadForClient | RF_LoadForServer | RF_LoadForEdit, // Flags affecting loading.
	RF_Load  			= RF_ContextFlags | RF_LoadContextFlags | RF_Public | RF_Standalone | RF_Native | RF_SourceModified | RF_Transactional | RF_HasStack | RF_HasArchtype, // Flags to load from Unrealfiles.
	RF_Keep             = RF_Native | RF_Marked, // Flags to persist across loads.
	RF_ScriptMask		= RF_Transactional | RF_Public | RF_Transient | RF_NotForClient | RF_NotForServer | RF_NotForEdit // Script-accessible flags.
};

/*----------------------------------------------------------------------------
	Core types.
----------------------------------------------------------------------------*/

//
// Globally unique identifier.
//
class CORE_API FGuid
{
public:
	DWORD A,B,C,D;
	FGuid()
	{}
	FGuid( DWORD InA, DWORD InB, DWORD InC, DWORD InD )
	: A(InA), B(InB), C(InC), D(InD)
	{}
	friend UBOOL operator==(const FGuid& X, const FGuid& Y)
		{return X.A==Y.A && X.B==Y.B && X.C==Y.C && X.D==Y.D;}
	friend UBOOL operator!=(const FGuid& X, const FGuid& Y)
		{return X.A!=Y.A || X.B!=Y.B || X.C!=Y.C || X.D!=Y.D;}
	friend FArchive& operator<<( FArchive& Ar, FGuid& G )
	{
		guard(FGuid<<);
		return Ar << G.A << G.B << G.C << G.D;
		unguard;
	}
	TCHAR* String() const
	{
		TCHAR* Result = appStaticString1024();
		appSprintf( Result, TEXT("%08X%08X%08X%08X"), A, B, C, D );
		return Result;
	}
};

inline INT CompareGuids( FGuid* A, FGuid* B )
{
	INT Diff;
	Diff = A->A-B->A; if( Diff ) return Diff;
	Diff = A->B-B->B; if( Diff ) return Diff;
	Diff = A->C-B->C; if( Diff ) return Diff;
	Diff = A->D-B->D; if( Diff ) return Diff;
	return 0;
}

//
// Information about a driver class.
//
class CORE_API FRegistryObjectInfo
{
public:
	FString Object;
	FString Class;
	FString MetaClass;
	FString Description;
	FString Autodetect;
	FRegistryObjectInfo()
	: Object(), Class(), MetaClass(), Description(), Autodetect()
	{}
};

//
// Information about a preferences menu item.
//
class CORE_API FPreferencesInfo
{
public:
	FString Caption, LangCaption;
	FString ParentCaption;
	FString Class;
	FName Category;
	UBOOL Immediate;
	FPreferencesInfo()
	: Caption(), LangCaption(), ParentCaption(), Class(), Category(NAME_None), Immediate(0)
	{}
};

/*----------------------------------------------------------------------------
	Core macros.
----------------------------------------------------------------------------*/

// Special canonical package for FindObject, ParseObject.
#define ANY_PACKAGE ((UPackage*)-1)

// Define private default constructor.
#define NO_DEFAULT_CONSTRUCTOR(cls) \
	protected: cls() {} public:

// Guard macros.
#define unguardobjSlow		unguardfSlow(( TEXT("(%ls)"), GetFullName() ))
#define unguardobj			unguardf(( TEXT("(%ls)"), GetFullName() ))

// Verify the a class definition and C++ definition match up.
#define VERIFY_CLASS_OFFSET(Pre,ClassName,Member) \
{ \
	UProperty* P = FindField<UProperty>(Pre##ClassName::StaticClass(), TEXT(#Member)); \
	if (P && P->Offset != STRUCT_OFFSET(Pre##ClassName, Member)) \
		appErrorf(TEXT("Class %ls Member %ls problem: Script=%i C++=%i"), TEXT(#ClassName), TEXT(#Member), P->Offset, STRUCT_OFFSET(Pre##ClassName,Member) ); \
}

// Verify that C++ and script code agree on the size of a class.
#define VERIFY_CLASS_SIZE(ClassName) \
	checkf(sizeof(ClassName)==ClassName::StaticClass()->GetPropertiesSize(), TEXT("Class %ls size mismatch: Script=%i C++=%i"), TEXT(#ClassName), ClassName::StaticClass()->GetPropertiesSize(), sizeof(ClassName));

// Verify a class definition and C++ definition match up (don't assert).
#define VERIFY_CLASS_OFFSET_NODIE(Pre,ClassName,Member) \
{ \
	LoadObject<UClass>(NULL, Pre##ClassName::StaticClass()->GetPathName(), NULL, 0, NULL); \
	UProperty* P = FindField<UProperty>(Pre##ClassName::StaticClass(), TEXT(#Member)); \
	if(!P) \
	{ \
		debugf(TEXT("VERIFY: Class '%ls' doesn't appear to have a '%ls' member!"), TEXT(#ClassName), TEXT(#Member)); \
		Mismatch = true; \
	} \
	else if (P->Offset != STRUCT_OFFSET(Pre##ClassName, Member)) \
	{ \
		debugf(TEXT("VERIFY: Class %ls Member %ls problem: Script=%i C++=%i"), TEXT(#ClassName), TEXT(#Member), P->Offset, STRUCT_OFFSET(Pre##ClassName, Member)); \
		Mismatch = true; \
	} \
}

#if DO_GUARD_SLOW
#define VERIFY_CLASS_OFFSET_NODIE_SLOW VERIFY_CLASS_OFFSET_NODIE
#else
#define VERIFY_CLASS_OFFSET_NODIE_SLOW(Pre,ClassName,Member)
#endif

// Verify that C++ and script code agree on the size of a class (don't assert).
#define VERIFY_CLASS_SIZE_NODIE(ClassName) \
	if (sizeof(ClassName)!=ClassName::StaticClass()->GetPropertiesSize()) { \
		debugf(TEXT("VERIFY: Class %ls size problem; Script=%i C++=%i"), TEXT(#ClassName), (int) ClassName::StaticClass()->GetPropertiesSize(), sizeof(ClassName)); \
		Mismatch = true; \
	}

// Declare the base UObject class.

#if !__STATIC_LINK
#define DECLARE_BASE_CLASS( TClass, TSuperClass, TStaticFlags, ... ) \
public: \
	/* Identification */ \
	enum {StaticClassFlags=TStaticFlags}; \
	private: static UClass PrivateStaticClass; public: \
	typedef TSuperClass Super;\
	typedef TClass ThisClass;\
	static UClass* StaticClass() \
		{ return &PrivateStaticClass; } \
	void* operator new( size_t Size, UObject* Outer=(UObject*)GetTransientPackage(), FName Name=NAME_None, DWORD SetFlags=0 ) \
		{ return StaticAllocateObject( StaticClass(), Outer, Name, SetFlags ); } \
	void* operator new( size_t Size, EInternal* Mem ) \
		{ return (void*)Mem; }
#else
#define DECLARE_BASE_CLASS( TClass, TSuperClass, TStaticFlags, TPackage ) \
public: \
	/* Identification */ \
	enum {StaticClassFlags=TStaticFlags}; \
	private: \
	static UClass* PrivateStaticClass; \
	public: \
	typedef TSuperClass Super;\
	typedef TClass ThisClass;\
	static UClass* GetPrivateStaticClass##TClass( const TCHAR* Package ); \
	static void InitializePrivateStaticClass##TClass(); \
	static UClass* StaticClass() \
	{ \
		if (!PrivateStaticClass) \
		{ \
			PrivateStaticClass = GetPrivateStaticClass##TClass( TEXT(#TPackage) ); \
			InitializePrivateStaticClass##TClass(); \
		} \
		return PrivateStaticClass; \
	} \
	void* operator new( size_t Size, UObject* Outer=(UObject*)GetTransientPackage(), FName Name=NAME_None, DWORD SetFlags=0 ) \
		{ return StaticAllocateObject( StaticClass(), Outer, Name, SetFlags ); } \
	void* operator new( size_t Size, EInternal* Mem ) \
		{ return (void*)Mem; }
#endif

// Declare a concrete class.
#define DECLARE_CLASS( TClass, TSuperClass, TStaticFlags, ... ) \
	DECLARE_BASE_CLASS( TClass, TSuperClass, TStaticFlags, __VA_ARGS__ ) \
	friend FArchive &operator<<( FArchive& Ar, TClass*& Res ) \
		{ return Ar << *(UObject**)&Res; } \
	virtual ~TClass() noexcept(false)   \
		{ ConditionalDestroy(); } \
	static void InternalConstructor( void* X ) \
		{ new( (EInternal*)X )TClass; } \

// Declare an abstract class.
#define DECLARE_ABSTRACT_CLASS( TClass, TSuperClass, TStaticFlags, ... ) \
	DECLARE_BASE_CLASS( TClass, TSuperClass, TStaticFlags | CLASS_Abstract, __VA_ARGS__ ) \
	friend FArchive &operator<<( FArchive& Ar, TClass*& Res ) \
		{ return Ar << *(UObject**)&Res; } \
	virtual ~TClass() noexcept(false)  \
		{ ConditionalDestroy(); } \

// Declare that objects of class being defined reside within objects of the specified class.
#define DECLARE_WITHIN( TWithinClass ) \
	typedef TWithinClass WithinClass; \
	TWithinClass* GetOuter##TWithinClass() const { return (TWithinClass*)GetOuter(); }

// Register a class at startup time.
#if __STATIC_LINK
#define IMPLEMENT_CLASS(TClass) \
	UClass* TClass::PrivateStaticClass = NULL; \
	UClass* TClass::GetPrivateStaticClass##TClass(const TCHAR* Package) \
		{ \
		UClass* ReturnClass; \
		ReturnClass = ::new UClass \
		(\
			EC_StaticConstructor, \
			sizeof(TClass), \
			StaticClassFlags, \
			FGuid(GUID1, GUID2, GUID3, GUID4), \
			&TEXT(#TClass)[1], \
			Package, \
			StaticConfigName(), \
			RF_Public | RF_Standalone | RF_Transient | RF_Native, \
			(void(*)(void*))TClass::InternalConstructor, \
			(void(UObject::*)())&TClass::StaticConstructor \
		); \
		check(ReturnClass); \
		return ReturnClass; \
		} \
		/* Called from ::StaticClass after GetPrivateStaticClass */ \
		void TClass::InitializePrivateStaticClass##TClass() \
		{ \
			/* No recursive ::StaticClass calls allowed. Setup extras. */ \
			if (TClass::Super::StaticClass() != TClass::PrivateStaticClass) \
				TClass::PrivateStaticClass->SuperField = TClass::Super::StaticClass(); \
			else \
				TClass::PrivateStaticClass->SuperField = NULL; \
			TClass::PrivateStaticClass->ClassWithin = TClass::WithinClass::StaticClass(); \
			TClass::PrivateStaticClass->SetClass(UClass::StaticClass()); \
			/* Perform UObject native registration. */ \
			if (TClass::PrivateStaticClass->GetInitialized() && TClass::PrivateStaticClass->GetClass() == TClass::PrivateStaticClass->StaticClass()) \
				TClass::PrivateStaticClass->Register(); \
		}
#elif _MSC_VER
	#define IMPLEMENT_CLASS(TClass) \
		UClass TClass::PrivateStaticClass \
		( \
			EC_NativeConstructor, \
			sizeof(TClass), \
			TClass::StaticClassFlags, \
			TClass::Super::StaticClass(), \
			TClass::WithinClass::StaticClass(), \
			FGuid(TClass::GUID1,TClass::GUID2,TClass::GUID3,TClass::GUID4), \
			&TEXT(#TClass)[1], \
			::GPackage, \
			StaticConfigName(), \
			RF_Public | RF_Standalone | RF_Transient | RF_Native, \
			(void(*)(void*))TClass::InternalConstructor, \
			(void(UObject::*)())&TClass::StaticConstructor \
		); \
		extern "C" DLL_EXPORT UClass* autoclass##TClass;\
		DLL_EXPORT UClass* autoclass##TClass = TClass::StaticClass();
#else
	#define IMPLEMENT_CLASS(TClass) \
		UClass TClass::PrivateStaticClass \
		( \
			EC_NativeConstructor, \
			sizeof(TClass), \
			TClass::StaticClassFlags, \
			TClass::Super::StaticClass(), \
			TClass::WithinClass::StaticClass(), \
			FGuid(TClass::GUID1,TClass::GUID2,TClass::GUID3,TClass::GUID4), \
			&TEXT(#TClass)[1], \
			::GPackage,			\
			StaticConfigName(), \
			RF_Public | RF_Standalone | RF_Transient | RF_Native, \
			(void(*)(void*))TClass::InternalConstructor, \
			(void(UObject::*)())&TClass::StaticConstructor \
		); \
		DLL_EXPORT {UClass* autoclass##TClass = TClass::StaticClass();}
#endif

// Define the package of the current DLL being compiled.
#define IMPLEMENT_PACKAGE(pkg) \
const TCHAR* GPackage = TEXT(#pkg); \
	IMPLEMENT_PACKAGE_PLATFORM(pkg)

/*-----------------------------------------------------------------------------
	UObject.
-----------------------------------------------------------------------------*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

#if BUILD_64
//
// stijn: Used during object construction
//
class UCompressedPointer
{
#define MAX_TEMPORARY_POINTERS (64 * 1024)
public:
	static const void* Pointers[MAX_TEMPORARY_POINTERS];
	static INT Store(const void* TempPointer);
	static const void* LoadAndInvalidate(INT PointerIndex);
};
#endif

struct FScriptCallPair
{
	class UObject* Object;
	class UObject* Func;

	inline void SetPair(UObject* O, UObject* F)
	{
		Object = O;
		Func = F;
	}
};

struct CORE_API FNativeCallScope
{
	FNativeCallScope(UObject* O, UObject* F);
	~FNativeCallScope();
	static void ChangeTopFunc(UObject* NewF);
};
#define WATCH_NATIVE_CALL(fnc) static UFunction* _FuncObject = FindObjectChecked<UFunction>(StaticClass(), TEXT(#fnc)); FNativeCallScope NScope(this, _FuncObject)
#define WATCH_NATIVE_CALL_PTR(caller, fptr) FNativeCallScope NScope(caller, fptr)

//
// The base class of all objects.
//
class CORE_API UObject
{
	// Declarations.
	DECLARE_BASE_CLASS(UObject,UObject,CLASS_Abstract,Core)
	typedef UObject WithinClass;
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};
	static const TCHAR* StaticConfigName() {return TEXT("System");}

	// Friends.
	friend class FObjectIterator;
	friend class ULinkerLoad;
	friend class ULinkerSave;
	friend class UPackageMap;
	friend class FArchiveTagUsed;
	friend struct FObjectImport;
	friend struct FObjectExport;
	friend class UStruct;
	friend class FSlowGCArc;
	friend struct FFrame;

private:
	// Internal per-object variables.
	INT						Index;				// Index of object into table.
	INT						HashedIndex;		// Index in object hash table.
	UObject*				HashNext;			// Next object in this hash bin.
	FStateFrame*			StateFrame;			// Main script execution stack.
	ULinkerLoad*			_Linker;			// Linker it came from, or NULL if none.

	// Index of this object in the linker's export map.
	// Note from stijn: During construction, a UObject's _LinkerIndex is overwritten with the
	// current value of GAutoRegister (i.e., a UObject pointer). The actual linker index is
	// calculated and stored when the object registers (in UObject::ProcessRegistrants)
	INT						_LinkerIndex;
	FORCE_64BIT_PADDING_DWORD;

	// Object this object resides in.
	// Note from stijn: During construction, a UObject's Outer is overwritten with a pointer
	// to its GPackage string. The actual Outer object pointer is derived and stored during
	// object registration (in UObject::Register).
	UObject*				Outer;
	UObject*				Archetype; // Save with delta compression to another object reference.
	DWORD					ObjectFlags;		// Private EObjectFlags used by object manager.

	// Name of the object.
	// Note from stijn: During construction, a UObject's Name is overwritten with a pointer
	// to a string containing its base name. During registration, the engine looks up
	// the base name in the name table and writes the actual FName value (in UObject::Register).
	FName					Name;
	UClass*					Class;	  			// Class the object belongs to.

	// Private systemwide variables.
	static UBOOL			GObjInitialized;	// Whether initialized.
	static UBOOL            GObjNoRegister;		// Registration disable.
	static INT				GObjBeginLoadCount;	// Count for BeginLoad multiple loads.
	static INT				GObjRegisterCount;  // ProcessRegistrants entry counter.
	static INT				GImportCount;		// Imports for EndLoad optimization.
	static UObject*			GObjHash[4096];		// Object hash.
	static UObject*			GAutoRegister;		// Objects to automatically register.
	static TArray<UObject*> GObjLoaded;			// Objects that might need preloading.
	static TArray<UObject*>	GObjRoot;			// Top of active object graph.
	static TArray<UObject*>	GObjObjects;		// List of all objects.
	static TArray<INT>      GObjAvailable;		// Available object indices.
	static TArray<UObject*>	GObjLoaders;		// Array of loaders.
	static UPackage*		GObjTransientPkg;	// Transient package.
	static TCHAR			GObjCachedLanguage[32]; // Language;
	static FStringNoInit	TraceIndent;
	static TArray<UObject*> GObjRegistrants;		// Registrants during ProcessRegistrants call.
	static TArray<FPreferencesInfo> GObjPreferences; // Preferences cache.
	static TArray<FRegistryObjectInfo> GObjDrivers; // Drivers cache.
	static TMultiMap<FName,FName>* GObjPackageRemap; // Remap table for loading renamed packages.
	static TCHAR GLanguage[256];
	static class UPackageMap* GPackageMapper;

	// Private functions.
	void AddObject( INT Index );
	void SetLinker( ULinkerLoad* L, INT I );

	// Private systemwide functions.
	static ULinkerLoad* GetLoader( INT i );
	static FName MakeUniqueObjectName( UObject* Outer, UClass* Class );
	static UBOOL ResolveName( UObject*& Outer, FString& Name, UBOOL Create, UBOOL Throw );
	static void SafeLoadError( DWORD LoadFlags, const TCHAR* Error, const TCHAR* Fmt, ... );
	static void PurgeGarbage(class FOutputDevice* Out = GLog);
	static void CacheDrivers( UBOOL ForceRefresh );

public:
	// Constructors.
	UObject();
	UObject( const UObject& Src );
	UObject( ENativeConstructor, UClass* InClass, const TCHAR* InName, const TCHAR* InPackageName, DWORD InFlags );
	UObject( EStaticConstructor, const TCHAR* InName, const TCHAR* InPackageName, DWORD InFlags );
	UObject( EInPlaceConstructor, UClass* InClass, UObject* InOuter, FName InName, DWORD InFlags );
	UObject& operator=( const UObject& );
	void StaticConstructor();
	static void InternalConstructor( void* X )
		{ new( (EInternal*)X )UObject; }

	// Destructors.
	virtual ~UObject() noexcept(false);
	void operator delete( void* Object, size_t Size )
		{guard(UObject::operator delete); appFree( Object ); unguard;}

	// UObject interface.
	virtual void ProcessEvent( UFunction* Function, void* Parms, void* Result=NULL );
	virtual void ProcessState( FLOAT DeltaSeconds );
	virtual UBOOL ProcessRemoteFunction( UFunction* Function, void* Parms, FFrame* Stack );
	virtual void Modify();
	virtual void PostLoad();
	virtual void Destroy();
	virtual void Serialize( FArchive& Ar );
	inline UBOOL IsPendingKill() const { return (ObjectFlags & RF_PendingDelete) != 0; }
	virtual EGotoState GotoState( FName State );
	UBOOL GotoState(FOutputDevice* Out, FName State, FName Label = NAME_None);
	virtual INT GotoLabel( FName Label );
	virtual void InitExecution();
	virtual void EndExecution();
	virtual void ShutdownAfterError();
	virtual void PostEditChange();
	virtual void CallFunction( FFrame& TheStack, RESULT_DECL, UFunction* Function );
	void RunScriptFunction(UFunction* F, const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor, FString* ReturnVal=NULL);
	virtual UBOOL ScriptConsoleExec( const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor );
	virtual void Register();
	virtual void LanguageChange();
	void ResetLoaderValidation();
	virtual void onPackageChanged(UObject* OldPackage) {} // Object was renamed to a new package.
	virtual void onPropertyChange(UProperty* Property, UProperty* OuterProperty); // We've edited a property value in properties window.
	virtual void onPropertyFocus(UProperty* Property, UProperty* OuterProperty, INT ArrayIndex) {} // We changed property focus.
	virtual void onEditSubObject(UObject* Obj) {} // We're editing an editinline object.
	virtual void SetNetworkDirty() {} // This object has became network dirty.

	// Systemwide functions.
	static UObject* StaticFindObject( UClass* Class, UObject* InOuter, const TCHAR* Name, UBOOL ExactClass=0 );
	static UObject* StaticFindObjectChecked( UClass* Class, UObject* InOuter, const TCHAR* Name, UBOOL ExactClass=0 );
	static UObject* StaticLoadObject( UClass* Class, UObject* InOuter, const TCHAR* Name, const TCHAR* Filename, DWORD LoadFlags, UPackageMap* Sandbox );
	static UClass* StaticLoadClass( UClass* BaseClass, UObject* InOuter, const TCHAR* Name, const TCHAR* Filename, DWORD LoadFlags, UPackageMap* Sandbox );
	static UObject* StaticAllocateObject(UClass* Class, UObject* InOuter = reinterpret_cast<UObject*>(GetTransientPackage()), FName Name = NAME_None, DWORD SetFlags = 0, UObject* Template = nullptr, FOutputDevice* Error = GError, UObject* Ptr = nullptr);
	static UObject* StaticConstructObject(UClass* Class, UObject* InOuter = reinterpret_cast<UObject*>(GetTransientPackage()), FName Name = NAME_None, DWORD SetFlags = 0, UObject* Template = nullptr, FOutputDevice* Error = GError, UObject* ArchType = nullptr);
	static void StaticInit();
	static void StaticExit();
	static UBOOL StaticExec( const TCHAR* Cmd, FOutputDevice& Ar=*GLog );
	static void StaticTick();
	static UObject* LoadPackage(UObject* InOuter, const TCHAR* Filename, DWORD LoadFlags, FName LoadOnlyType = NAME_None);
	static UBOOL SavePackage(UObject* InOuter, UObject* Base, DWORD TopLevelFlags, const TCHAR* Filename, FOutputDevice* Error = GError, ULinkerLoad* Conform = nullptr, UBOOL bAutoSave = FALSE, struct FSavedGameSummary* SaveSummary = nullptr);
	static void CollectGarbage(DWORD KeepFlags, FOutputDevice* Out = GLog);
	static void TickGarbageCollector();
	static void KeepObject(UObject* Obj); // Prevent slow GC from deleting this object this run.
	static void SerializeRootSet( FArchive& Ar, DWORD KeepFlags, DWORD RequiredFlags );
	static UBOOL IsReferenced( UObject*& Res, DWORD KeepFlags, UBOOL IgnoreReference );
	static UBOOL AttemptDelete( UObject*& Res, DWORD KeepFlags, UBOOL IgnoreReference );
	static void BeginLoad();
	static void EndLoad();
	static void InitProperties( BYTE* Data, INT DataCount, UClass* DefaultsClass, BYTE* Defaults, INT DefaultsCount, UObject* RefObj=NULL );
	static void ExitProperties(BYTE* Data, UClass* Class);
	static void ResetTransientProperties(BYTE* Data, UClass* Class, INT DataSize);
	static void ResetLoaders( UObject* InOuter, UBOOL DynamicOnly, UBOOL ForceLazyLoad );
	static UPackage* CreatePackage( UObject* InOuter, const TCHAR* PkgName );
	static ULinkerLoad* GetPackageLinker( UObject* InOuter, const TCHAR* Filename, DWORD LoadFlags, UPackageMap* Sandbox, FGuid* CompatibleGuid, UBOOL bNoCacheSearch=0, UObject* MainPackage=NULL );
	static void StaticShutdownAfterError();
	static UObject* GetIndexedObject( INT Index );
	static void GlobalSetProperty( const TCHAR* Value, UClass* Class, UProperty* Property, INT Offset, UBOOL Immediate );
	static void ExportProperties( FOutputDevice& Out, UClass* ObjectClass, BYTE* Object, INT Indent, UClass* DiffClass, BYTE* Diff, DWORD RequireFlags=0 );
	static void ResetConfig( UClass* Class );
	static void GetRegistryObjects( TArray<FRegistryObjectInfo>& Results, UClass* Class, UClass* MetaClass, UBOOL ForceRefresh );
	static void GetPreferences( TArray<FPreferencesInfo>& Results, const TCHAR* Category, UBOOL ForceRefresh );
	static UBOOL GetInitialized();
	static UPackage* GetTransientPackage();
	static void VerifyLinker( ULinkerLoad* Linker );
	static void ProcessRegistrants();
	static void BindPackage( UPackage* Pkg );
	static const TCHAR* GetLanguage();
	static void SetLanguage( const TCHAR* LanguageExt );
	static inline INT GetObjectHash( FName ObjName, INT Outer )
	{
		return (ObjName.GetIndex() ^ Outer) & (ARRAY_COUNT(GObjHash)-1);
	}
	static void GetUScriptStack(TArray<FScriptCallPair>& GStack); // Retrive current execution stack.

	// 227j: Network package mapping.
	static void SetActivePackageMap(UPackageMap* NewSandbox);
	static UPackageMap* GetActivePackageMap()
	{
		return GPackageMapper;
	}

	// Functions.
	void AddToRoot();
	void RemoveFromRoot();
	void HashObject();
	void UnhashObject();
	const TCHAR* GetFullName() const;
	const TCHAR* GetPathName( UObject* StopOuter=NULL ) const;
    FString GetFullNameSafe() const; // stijn: binarycompat - improved version
	FString GetPathNameSafe( UObject* StopOuter=NULL ) const; // stijn: binarycompat - improved version
	UBOOL IsValid();
	void ConditionalRegister();
	UBOOL ConditionalDestroy();
	void ConditionalPostLoad();
	void ConditionalShutdownAfterError();
	inline UBOOL IsA( UClass* SomeBaseClass ) const;
	UBOOL IsIn( UObject* SomeOuter ) const;
	UBOOL IsProbing( FName ProbeName );
	void Rename( const TCHAR* NewName=NULL, UObject* NewOuter=((UObject*)-1));
	UField* FindObjectField( FName InName, UBOOL Global=0 );
	UFunction* FindFunction( FName InName, UBOOL Global=0 );
	UFunction* FindFunctionChecked( FName InName, UBOOL Global=0 );
	UState* FindState( FName InName );
	void SaveConfig( DWORD Flags=CPF_Config, const TCHAR* Filename=NULL, BYTE bWriteINI=1 );
	void LoadConfig( UBOOL Propagate=0, UClass* Class=NULL, const TCHAR* Filename=NULL, UBOOL Force=0 );	// rjp
	void ClearConfig( const TCHAR* PropName=NULL );										// rjp
	void LoadLocalized( UBOOL Propagate=0, UClass* Class=NULL );
	void InitClassDefaultObject( UClass* InClass );
	void ProcessInternal( FFrame& TheStack, void*const Result );
	void ParseParms( const TCHAR* Parms );
	UBOOL ParsePropertyValue(void* Result, const TCHAR** Str, BYTE bIntValue);
	void ScriptDestroyObject(); // Gracyfully destroy this object calling ObjectDeleted UnrealScript event.
	virtual UBOOL ShouldExportProperty(UProperty* Property) const; // Editor API: Object should export property value for T3D file.
	virtual void OnCreateObjectNew(UObject* ParentObject, UProperty* PropertyRef) {} // Editor API: Object was created in properties window with 'New'.
	virtual void OnImportDefaults(UClass* OwnerClass) {} // Editor API: notify this object was imported for default object.
	virtual UBOOL ImportScriptVariable(UProperty* Property, const TCHAR* Value) // A custom handle for importing SetPropertyText/ImportFullProperties script variable data.
	{
		return FALSE;
	}
	virtual UBOOL AllowScriptDestroy() // Check if allowed to use delete operator on this in UScript.
	{
		return TRUE;
	}
	virtual UBOOL ShouldSaveObject() const; // Should save this new EditInLine object reference (can be used to avoid from saving unmodified editobjects).
	void DestroyAutoDestructors();

	// Accessors.
	UClass* GetClass() const
	{
		return Class;
	}
	void SetClass(UClass* NewClass)
	{
		Class = NewClass;
	}
	DWORD GetFlags() const
	{
		return ObjectFlags;
	}
	void SetFlags( DWORD NewFlags )
	{
		ObjectFlags |= NewFlags;
		checkSlow(Name!=NAME_None || !(ObjectFlags&RF_Public));
	}
	void ClearFlags( DWORD NewFlags )
	{
		ObjectFlags &= ~NewFlags;
		checkSlow(Name!=NAME_None || !(ObjectFlags&RF_Public));
	}
	const TCHAR* GetName() const
	{
		if (!const_cast<volatile UObject*>(this))
			return TEXT("None");
		const TCHAR* Result;
		try
		{
			Result = *Name;
		}
		catch(...)
		{
			Result = TEXT("<Invalid GetName call>");
		}
		return Result;
	}
	const FName GetFName() const
	{
		if (Name.IsValid())
			return Name;
		else
			return NAME_Invalid;
	}
	UObject* GetOuter() const
	{
		return Outer;
	}
	DWORD GetIndex() const
	{
		return Index;
	}
	ULinkerLoad* GetLinker() const
	{
		return _Linker;
	}
	INT GetLinkerIndex() const
	{
		return _LinkerIndex;
	}
	FStateFrame* GetStateFrame() const
	{
		return StateFrame;
	}
	void SetLatentAction( INT Action )
	{
		if( StateFrame )
			StateFrame->LatentAction = Action;
	}
	inline UObject* GetArchetype() const
	{
		return Archetype;
	}
	inline void SetArchetype(UObject* Obj)
	{
		Archetype = Obj;
		if (Obj != NULL)
			ObjectFlags |= RF_HasArchtype;
		else ObjectFlags &= ~RF_HasArchtype;
	}
	inline UObject* TopOuter() const
	{
		UObject* Obj = const_cast<UObject*>(this);
		if (!Obj)
			return NULL;
		while (Obj->Outer)
			Obj = Obj->Outer;
		return Obj;
	}

	// This object is an registered object.
	inline UBOOL IsRegisteredObject() const
	{
		return (Index > 0);
	}

	// Object is currently hashed.
	inline UBOOL IsHashedObject() const
	{
		return (HashedIndex > 0);
	}

	// Mark this package as dirty.
	void MarkAsDirty();

	// UnrealScript intrinsics.
	#define DECLARE_FUNCTION(func) void func( FFrame& TheStack, RESULT_DECL );
	DECLARE_FUNCTION(execUndefined)
	DECLARE_FUNCTION(execLocalVariable)
	DECLARE_FUNCTION(execInstanceVariable)
	DECLARE_FUNCTION(execDefaultVariable)
	DECLARE_FUNCTION(execArrayElement)
	DECLARE_FUNCTION(execDynArrayElement)
	DECLARE_FUNCTION(execMapKeyElement)
	DECLARE_FUNCTION(execBoolVariable)
	DECLARE_FUNCTION(execClassDefaultVariable)
	DECLARE_FUNCTION(execEndFunctionParms)
	DECLARE_FUNCTION(execNothing)
	DECLARE_FUNCTION(execStop)
	DECLARE_FUNCTION(execEndCode)
	DECLARE_FUNCTION(execSwitch)
	DECLARE_FUNCTION(execCase)
	DECLARE_FUNCTION(execJump)
	DECLARE_FUNCTION(execJumpIfNot)
	DECLARE_FUNCTION(execAssert)
	DECLARE_FUNCTION(execGotoLabel)
	DECLARE_FUNCTION(execLet)
	DECLARE_FUNCTION(execLetBool)
	DECLARE_FUNCTION(execEatString)
	DECLARE_FUNCTION(execEatMiscExpr)
	DECLARE_FUNCTION(execSelf)
	DECLARE_FUNCTION(execContext)
	DECLARE_FUNCTION(execVirtualFunction)
	DECLARE_FUNCTION(execFinalFunction)
	DECLARE_FUNCTION(execGlobalFunction)
	DECLARE_FUNCTION(execStructCmpEq)
	DECLARE_FUNCTION(execStructCmpNe)
	DECLARE_FUNCTION(execStructMember)
	DECLARE_FUNCTION(execIntConst)
	DECLARE_FUNCTION(execFloatConst)
	DECLARE_FUNCTION(execStringConst)
	DECLARE_FUNCTION(execUnicodeStringConst)
	DECLARE_FUNCTION(execEvaluateValue)
	DECLARE_FUNCTION(execObjectConst)
	DECLARE_FUNCTION(execNameConst)
	DECLARE_FUNCTION(execByteConst)
	DECLARE_FUNCTION(execIntZero)
	DECLARE_FUNCTION(execIntOne)
	DECLARE_FUNCTION(execTrue)
	DECLARE_FUNCTION(execFalse)
	DECLARE_FUNCTION(execNoObject)
	DECLARE_FUNCTION(execIntConstByte)
	DECLARE_FUNCTION(execDynamicCast)
	DECLARE_FUNCTION(execMetaCast)
	DECLARE_FUNCTION(execByteToInt)
	DECLARE_FUNCTION(execByteToBool)
	DECLARE_FUNCTION(execByteToFloat)
	DECLARE_FUNCTION(execByteToString)
	DECLARE_FUNCTION(execIntToByte)
	DECLARE_FUNCTION(execIntToBool)
	DECLARE_FUNCTION(execIntToFloat)
	DECLARE_FUNCTION(execIntToString)
	DECLARE_FUNCTION(execBoolToByte)
	DECLARE_FUNCTION(execBoolToInt)
	DECLARE_FUNCTION(execBoolToFloat)
	DECLARE_FUNCTION(execBoolToString)
	DECLARE_FUNCTION(execFloatToByte)
	DECLARE_FUNCTION(execFloatToInt)
	DECLARE_FUNCTION(execFloatToBool)
	DECLARE_FUNCTION(execFloatToString)
	DECLARE_FUNCTION(execRotationConst)
	DECLARE_FUNCTION(execVectorConst)
	DECLARE_FUNCTION(execStringToVector)
	DECLARE_FUNCTION(execStringToRotator)
	DECLARE_FUNCTION(execVectorToBool)
	DECLARE_FUNCTION(execVectorToString)
	DECLARE_FUNCTION(execVectorToRotator)
	DECLARE_FUNCTION(execRotatorToBool)
	DECLARE_FUNCTION(execRotatorToVector)
	DECLARE_FUNCTION(execRotatorToString)
    DECLARE_FUNCTION(execRotRand)
    DECLARE_FUNCTION(execGetUnAxes)
    DECLARE_FUNCTION(execGetAxes)
    DECLARE_FUNCTION(execSubtractEqual_RotatorRotator)
    DECLARE_FUNCTION(execAddEqual_RotatorRotator)
    DECLARE_FUNCTION(execSubtract_RotatorRotator)
    DECLARE_FUNCTION(execAdd_RotatorRotator)
    DECLARE_FUNCTION(execDivideEqual_RotatorFloat)
    DECLARE_FUNCTION(execMultiplyEqual_RotatorFloat)
    DECLARE_FUNCTION(execDivide_RotatorFloat)
    DECLARE_FUNCTION(execMultiply_FloatRotator)
    DECLARE_FUNCTION(execMultiply_RotatorFloat)
    DECLARE_FUNCTION(execNotEqual_RotatorRotator)
    DECLARE_FUNCTION(execEqualEqual_RotatorRotator)
    DECLARE_FUNCTION(execMirrorVectorByNormal)
    DECLARE_FUNCTION(execVRand)
#if 1 //Math added by Legend on 4/12/2000
	DECLARE_FUNCTION(execRandomSpreadVector)
#endif
    DECLARE_FUNCTION(execInvert)
    DECLARE_FUNCTION(execNormal)
    DECLARE_FUNCTION(execVSize)
    DECLARE_FUNCTION(execSubtractEqual_VectorVector)
    DECLARE_FUNCTION(execAddEqual_VectorVector)
    DECLARE_FUNCTION(execDivideEqual_VectorFloat)
    DECLARE_FUNCTION(execMultiplyEqual_VectorVector)
    DECLARE_FUNCTION(execMultiplyEqual_VectorFloat)
    DECLARE_FUNCTION(execCross_VectorVector)
    DECLARE_FUNCTION(execDot_VectorVector)
    DECLARE_FUNCTION(execNotEqual_VectorVector)
    DECLARE_FUNCTION(execEqualEqual_VectorVector)
	DECLARE_FUNCTION(execAvgEqualEqual_VectorVector)
    DECLARE_FUNCTION(execGreaterGreater_VectorRotator)
    DECLARE_FUNCTION(execLessLess_VectorRotator)
    DECLARE_FUNCTION(execSubtract_VectorVector)
    DECLARE_FUNCTION(execAdd_VectorVector)
    DECLARE_FUNCTION(execDivide_VectorFloat)
    DECLARE_FUNCTION(execMultiply_VectorVector)
    DECLARE_FUNCTION(execMultiply_FloatVector)
    DECLARE_FUNCTION(execMultiply_VectorFloat)
    DECLARE_FUNCTION(execSubtract_PreVector)
	DECLARE_FUNCTION(execOrthoRotation);
	DECLARE_FUNCTION(execNormalize);
	DECLARE_FUNCTION(execObjectToBool)
	DECLARE_FUNCTION(execObjectToString)
	DECLARE_FUNCTION(execNameToBool)
	DECLARE_FUNCTION(execNameToString)
	DECLARE_FUNCTION(execStringToByte)
	DECLARE_FUNCTION(execStringToInt)
	DECLARE_FUNCTION(execStringToBool)
	DECLARE_FUNCTION(execStringToFloat)
	DECLARE_FUNCTION(execNot_PreBool)
	DECLARE_FUNCTION(execEqualEqual_BoolBool)
	DECLARE_FUNCTION(execNotEqual_BoolBool)
	DECLARE_FUNCTION(execAndAnd_BoolBool)
	DECLARE_FUNCTION(execXorXor_BoolBool)
	DECLARE_FUNCTION(execOrOr_BoolBool)
	DECLARE_FUNCTION(execMultiplyEqual_ByteByte)
	DECLARE_FUNCTION(execDivideEqual_ByteByte)
	DECLARE_FUNCTION(execAddEqual_ByteByte)
	DECLARE_FUNCTION(execSubtractEqual_ByteByte)
	DECLARE_FUNCTION(execAddAdd_PreByte)
	DECLARE_FUNCTION(execSubtractSubtract_PreByte)
	DECLARE_FUNCTION(execAddAdd_Byte)
	DECLARE_FUNCTION(execSubtractSubtract_Byte)
	DECLARE_FUNCTION(execComplement_PreInt)
	DECLARE_FUNCTION(execSubtract_PreInt)
	DECLARE_FUNCTION(execMultiply_IntInt)
	DECLARE_FUNCTION(execDivide_IntInt)
	DECLARE_FUNCTION(execAdd_IntInt)
	DECLARE_FUNCTION(execSubtract_IntInt)
	DECLARE_FUNCTION(execLessLess_IntInt)
	DECLARE_FUNCTION(execGreaterGreater_IntInt)
	DECLARE_FUNCTION(execGreaterGreaterGreater_IntInt)
	DECLARE_FUNCTION(execLess_IntInt)
	DECLARE_FUNCTION(execGreater_IntInt)
	DECLARE_FUNCTION(execLessEqual_IntInt)
	DECLARE_FUNCTION(execGreaterEqual_IntInt)
	DECLARE_FUNCTION(execEqualEqual_IntInt)
	DECLARE_FUNCTION(execNotEqual_IntInt)
	DECLARE_FUNCTION(execAnd_IntInt)
	DECLARE_FUNCTION(execXor_IntInt)
	DECLARE_FUNCTION(execOr_IntInt)
	DECLARE_FUNCTION(execMultiplyEqual_IntFloat)
	DECLARE_FUNCTION(execDivideEqual_IntFloat)
	DECLARE_FUNCTION(execAddEqual_IntInt)
	DECLARE_FUNCTION(execSubtractEqual_IntInt)
	DECLARE_FUNCTION(execAddAdd_PreInt)
	DECLARE_FUNCTION(execSubtractSubtract_PreInt)
	DECLARE_FUNCTION(execAddAdd_Int)
	DECLARE_FUNCTION(execSubtractSubtract_Int)
	DECLARE_FUNCTION(execRand)
	DECLARE_FUNCTION(execMin)
	DECLARE_FUNCTION(execMax)
	DECLARE_FUNCTION(execClamp)
	DECLARE_FUNCTION(execSubtract_PreFloat)
	DECLARE_FUNCTION(execMultiplyMultiply_FloatFloat)
	DECLARE_FUNCTION(execMultiply_FloatFloat)
	DECLARE_FUNCTION(execDivide_FloatFloat)
	DECLARE_FUNCTION(execPercent_FloatFloat)
	DECLARE_FUNCTION(execAdd_FloatFloat)
	DECLARE_FUNCTION(execSubtract_FloatFloat)
	DECLARE_FUNCTION(execLess_FloatFloat)
	DECLARE_FUNCTION(execGreater_FloatFloat)
	DECLARE_FUNCTION(execLessEqual_FloatFloat)
	DECLARE_FUNCTION(execGreaterEqual_FloatFloat)
	DECLARE_FUNCTION(execEqualEqual_FloatFloat)
	DECLARE_FUNCTION(execNotEqual_FloatFloat)
	DECLARE_FUNCTION(execComplementEqual_FloatFloat)
	DECLARE_FUNCTION(execMultiplyEqual_FloatFloat)
	DECLARE_FUNCTION(execDivideEqual_FloatFloat)
	DECLARE_FUNCTION(execAddEqual_FloatFloat)
	DECLARE_FUNCTION(execSubtractEqual_FloatFloat)
	DECLARE_FUNCTION(execAbs)
	DECLARE_FUNCTION(execDebugInfo) //DEBUGGER
	DECLARE_FUNCTION(execSin)
	DECLARE_FUNCTION(execCos)
	DECLARE_FUNCTION(execAcos)
	DECLARE_FUNCTION(execTan)
	DECLARE_FUNCTION(execAtan)
	DECLARE_FUNCTION(execExp)
	DECLARE_FUNCTION(execLoge)
	DECLARE_FUNCTION(execSqrt)
	DECLARE_FUNCTION(execSquare)
	DECLARE_FUNCTION(execFRand)
	DECLARE_FUNCTION(execFMin)
	DECLARE_FUNCTION(execFMax)
	DECLARE_FUNCTION(execFClamp)
	DECLARE_FUNCTION(execLerp)
	DECLARE_FUNCTION(execSmerp)
	DECLARE_FUNCTION(execConcat_StringString)
	DECLARE_FUNCTION(execConcatEqual_StringString)
	DECLARE_FUNCTION(execAt_StringString)
	DECLARE_FUNCTION(execAtEqual_StringString)
	DECLARE_FUNCTION(execLess_StringString)
	DECLARE_FUNCTION(execGreater_StringString)
	DECLARE_FUNCTION(execLessEqual_StringString)
	DECLARE_FUNCTION(execGreaterEqual_StringString)
	DECLARE_FUNCTION(execEqualEqual_StringString)
	DECLARE_FUNCTION(execNotEqual_StringString)
	DECLARE_FUNCTION(execComplementEqual_StringString)
	DECLARE_FUNCTION(execSubtractEqual_StringString)
	DECLARE_FUNCTION(execLen)
	DECLARE_FUNCTION(execInStr)
	DECLARE_FUNCTION(execMid)
	DECLARE_FUNCTION(execLeft)
	DECLARE_FUNCTION(execRight)
	DECLARE_FUNCTION(execCaps)
	DECLARE_FUNCTION(execChr)
	DECLARE_FUNCTION(execAsc)
	DECLARE_FUNCTION(execLocs)
	DECLARE_FUNCTION(execReplaceStr)
	DECLARE_FUNCTION(execExecFunctionStr)
	DECLARE_FUNCTION(execStartsWith)
	DECLARE_FUNCTION(execStringToName)
	DECLARE_FUNCTION(execIntToStr)
	DECLARE_FUNCTION(execSortArray)
	DECLARE_FUNCTION(execSortStaticArray)
	DECLARE_FUNCTION(execMultiply_CoordsCoords)
	DECLARE_FUNCTION(execMultiplyRes_CoordsCoords)
	DECLARE_FUNCTION(execMultiply_CoordsRotator)
	DECLARE_FUNCTION(execDivide_CoordsRotator)
	DECLARE_FUNCTION(execMultiply_VectorCoords)
	DECLARE_FUNCTION(execGetUnitCoords)
	DECLARE_FUNCTION(execTransformRotatorByNormal);
	DECLARE_FUNCTION(execTransformCoordsByNormal);
	DECLARE_FUNCTION(execEqualEqual_ObjectObject)
	DECLARE_FUNCTION(execNotEqual_ObjectObject)
	DECLARE_FUNCTION(execEqualEqual_NameName)
	DECLARE_FUNCTION(execNotEqual_NameName)
	DECLARE_FUNCTION(execLoadPackageContents);
	DECLARE_FUNCTION(execLog)
	DECLARE_FUNCTION(execWarn)
	DECLARE_FUNCTION(execNew)
	DECLARE_FUNCTION(execRandRange)
	DECLARE_FUNCTION(execRandRangeNum)
	DECLARE_FUNCTION(execRandIntRange)
	DECLARE_FUNCTION(execClassIsChildOf)
	DECLARE_FUNCTION(execClassContext)
	DECLARE_FUNCTION(execGoto)
	DECLARE_FUNCTION(execGotoState)
	DECLARE_FUNCTION(execIsA)
	DECLARE_FUNCTION(execEnable)
	DECLARE_FUNCTION(execDisable)
	DECLARE_FUNCTION(execIterator)
	DECLARE_FUNCTION(execLocalize)
	DECLARE_FUNCTION(execNativeParm)
	DECLARE_FUNCTION(execGetPropertyText)
	DECLARE_FUNCTION(execSetPropertyText)
	DECLARE_FUNCTION(execSaveConfig)
	DECLARE_FUNCTION(execStaticSaveConfig)
	DECLARE_FUNCTION(execResetConfig)
	DECLARE_FUNCTION(execClearConfig)	// rjp
	DECLARE_FUNCTION(execGetPerObjectNames)
	DECLARE_FUNCTION(execStaticClearConfig)
	DECLARE_FUNCTION(execGetEnum)
	DECLARE_FUNCTION(execDynamicLoadObject)
	DECLARE_FUNCTION(execIsInState)
	DECLARE_FUNCTION(execGetStateName)
	DECLARE_FUNCTION(execLineNumber)
	DECLARE_FUNCTION(execSetUTracing)
	DECLARE_FUNCTION(execIsUTracing)
	DECLARE_FUNCTION(execHighNative0)
	DECLARE_FUNCTION(execHighNative1)
	DECLARE_FUNCTION(execHighNative2)
	DECLARE_FUNCTION(execHighNative3)
	DECLARE_FUNCTION(execHighNative4)
	DECLARE_FUNCTION(execHighNative5)
	DECLARE_FUNCTION(execHighNative6)
	DECLARE_FUNCTION(execHighNative7)
	DECLARE_FUNCTION(execHighNative8)
	DECLARE_FUNCTION(execHighNative9)
	DECLARE_FUNCTION(execHighNative10)
	DECLARE_FUNCTION(execHighNative11)
	DECLARE_FUNCTION(execHighNative12)
	DECLARE_FUNCTION(execHighNative13)
	DECLARE_FUNCTION(execHighNative14)
	DECLARE_FUNCTION(execHighNative15)

	#ifdef UTPG_MD5
	DECLARE_FUNCTION(execCalcMD5)
	DECLARE_FUNCTION(execPackageMD5)
	DECLARE_FUNCTION(execPackageGUIDMD5)
	DECLARE_FUNCTION(execMD5TableLookup)
	#endif

	// Smirftsch - add some useful functions
	DECLARE_FUNCTION(execMakeColor)
	DECLARE_FUNCTION(execFindObject)
	DECLARE_FUNCTION(execGetParentClass)
	DECLARE_FUNCTION(execAllObjects)
	DECLARE_FUNCTION(execAllFiles)
	DECLARE_FUNCTION(execAllLinkers)
	DECLARE_FUNCTION(execGetDefaultObject)
	DECLARE_FUNCTION(execDivide)
	DECLARE_FUNCTION(execExtractString)
	DECLARE_FUNCTION(execArray_Size)
	DECLARE_FUNCTION(execArray_Insert)
	DECLARE_FUNCTION(execArray_Remove)
	DECLARE_FUNCTION(execAppSeconds)
	DECLARE_FUNCTION(execFindObjectIndex)
	DECLARE_FUNCTION(execArrayIterator)
	DECLARE_FUNCTION(execMapIterator)
	DECLARE_FUNCTION(execMapRemoveValue)
	DECLARE_FUNCTION(execMapHasValue)
	DECLARE_FUNCTION(execMapEmptyValue)
	DECLARE_FUNCTION(execVSizeSq)
	DECLARE_FUNCTION(execVSize2D)
	DECLARE_FUNCTION(execVSize2DSq)
	DECLARE_FUNCTION(execNormal2D)
	// Smirftsch - quaternion
	DECLARE_FUNCTION(execCoordsToQuat)
	DECLARE_FUNCTION(execQuatToCoords)
	DECLARE_FUNCTION(execRotationToQuat)
	DECLARE_FUNCTION(execQuatToRotation)
	DECLARE_FUNCTION(execMakeQuat)
	DECLARE_FUNCTION(execAnd_RotatorRotator)
	DECLARE_FUNCTION(execMultiply_QuatVector)
	DECLARE_FUNCTION(execMultiply_VectorQuat)
	DECLARE_FUNCTION(execMultiply_QuatFloat)
	DECLARE_FUNCTION(execMultiply_FloatQuat)
	DECLARE_FUNCTION(execMultiply_QuatQuat)
	DECLARE_FUNCTION(execAdd_QuatQuat)
	DECLARE_FUNCTION(execSubtract_QuatQuat)
	DECLARE_FUNCTION(execAddEqual_QuatQuat)
	DECLARE_FUNCTION(execSubtractEqual_QuatQuat)
	DECLARE_FUNCTION(execMultiplyEqual_QuatFloat)
	DECLARE_FUNCTION(execDivideEqual_QuatFloat)
	DECLARE_FUNCTION(execComplement_PreQuat)
	DECLARE_FUNCTION(execDot_QuatQuat)
	DECLARE_FUNCTION(execQuatGetVect)
	DECLARE_FUNCTION(execQuatSizeSquared)
	DECLARE_FUNCTION(execQuatSize)
	DECLARE_FUNCTION(execQuatNormal)
	DECLARE_FUNCTION(execQuatGetAngle)
	DECLARE_FUNCTION(execQuatGetAxis)
	DECLARE_FUNCTION(execQuatRotate)
	DECLARE_FUNCTION(execQuatVRotate)
	DECLARE_FUNCTION(execQuatSlerp)
	DECLARE_FUNCTION(execQuatFromAxisAndAngle)
	DECLARE_FUNCTION(execQuatError)
	DECLARE_FUNCTION(execAlignQuatWith)
	DECLARE_FUNCTION(execCeil)
	DECLARE_FUNCTION(execConstructStruct)
	DECLARE_FUNCTION(execAppIsEditor)
	DECLARE_FUNCTION(execArrayOperator)
	DECLARE_FUNCTION(execFindFunction)
	DECLARE_FUNCTION(execAnyOperator)
	DECLARE_FUNCTION(execGetCallStack)
	DECLARE_FUNCTION(execDeleteObject)
	DECLARE_FUNCTION(execRefIsValid)
	DECLARE_FUNCTION(execMallocConstRef)
	DECLARE_FUNCTION(execInterpCurveEval)
	DECLARE_FUNCTION(execLerpVector)
	DECLARE_FUNCTION(execLerpRotation)
	DECLARE_FUNCTION(execSlerpRotation)
	DECLARE_FUNCTION(execExportFullProperties)
	DECLARE_FUNCTION(execImportFullProperties)
	DECLARE_FUNCTION(execGetClassFlags)

	// UnrealScript calling stubs.
    void eventBeginState()
    {
        ProcessEvent(FindFunctionChecked(NAME_BeginState),NULL);
    }
    void eventEndState()
    {
        ProcessEvent(FindFunctionChecked(NAME_EndState),NULL);
    }
	void eventObjectDeleted()
	{
		UFunction* F = FindFunction(NAME_ObjectDeleted);
		if (F)
			ProcessEvent(F, NULL);
	}
	void eventOnPropertyChange(FName Property, FName OuterProperty = NAME_None);
	static void eventStaticPropertyChange(UClass* C, FName Property, FName OuterProperty = NAME_None);
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

inline FString FObjectName(const UObject* Object)
{
	if (!Object)
		return TEXT("None");
	return Object->GetName();
}

inline FString FObjectPathName(const UObject* Object)
{
	if (!Object)
		return TEXT("None");
	return Object->GetPathNameSafe();
}

inline FString FObjectFullName(const UObject* Object, UBOOL DoubleNone = 0)
{
	if (!Object)
		return DoubleNone ? TEXT("None None") : TEXT("None");
	return Object->GetFullNameSafe();
}

/*----------------------------------------------------------------------------
	Core templates.
----------------------------------------------------------------------------*/

// Hash function.
inline DWORD GetTypeHash( const UObject* A )
{
	return A ? A->GetIndex() : 0;
}

// Parse an object name in the input stream.
template< class T > UBOOL ParseObject( const TCHAR* Stream, const TCHAR* Match, T*& Obj, UObject* Outer )
{
	return ParseObject( Stream, Match, T::StaticClass(), *(UObject **)&Obj, Outer );
}

// Find an optional object.
template< class T > T* FindObject( UObject* Outer, const TCHAR* Name, UBOOL ExactClass=0 )
{
	return reinterpret_cast<T*>(UObject::StaticFindObject( T::StaticClass(), Outer, Name, ExactClass ));
}

// Find an object, no failure allowed.
template< class T > T* FindObjectChecked( UObject* Outer, const TCHAR* Name, UBOOL ExactClass=0 )
{
	return reinterpret_cast<T*>(UObject::StaticFindObjectChecked( T::StaticClass(), Outer, Name, ExactClass ));
}

// Dynamically cast an object type-safely.
template< class T > T* Cast( UObject* Src )
{
	return Src && Src->IsA(T::StaticClass()) ? reinterpret_cast<T*>(Src) : NULL;
}
template< class T, class U > T* CastChecked( U* Src )
{
	if( !Src || !Src->IsA(T::StaticClass()) )
		appErrorf( TEXT("Cast of %ls to %ls failed"), Src ? Src->GetFullName() : TEXT("NULL"), T::StaticClass()->GetName() );
	return (T*)Src;
}

// Construct an object of a particular class.
template< class T > T* ConstructObject( UClass* Class, UObject* Outer=reinterpret_cast<UObject*>(INDEX_NONE), FName Name=NAME_None, DWORD SetFlags=0, UObject* ArchType=NULL )
{
	// check(Class->IsChildOf(T::StaticClass())); // check later, Smirftsch
	if( Outer==reinterpret_cast<UObject*>(INDEX_NONE) )
		Outer = reinterpret_cast<UObject*>(UObject::GetTransientPackage());
	return reinterpret_cast<T*>(UObject::StaticConstructObject( Class, Outer, Name, SetFlags, ArchType, GError, ArchType));
}

// Load an object.
template< class T > T* LoadObject( UObject* Outer, const TCHAR* Name, const TCHAR* Filename, DWORD LoadFlags, UPackageMap* Sandbox )
{
	return reinterpret_cast<T*>(UObject::StaticLoadObject( T::StaticClass(), Outer, Name, Filename, LoadFlags, Sandbox ));
}

// Load a class object.
template< class T > UClass* LoadClass( UObject* Outer, const TCHAR* Name, const TCHAR* Filename, DWORD LoadFlags, UPackageMap* Sandbox )
{
	return UObject::StaticLoadClass( T::StaticClass(), Outer, Name, Filename, LoadFlags, Sandbox );
}

// Get default object of a class.
template< class T > T* GetDefault()
{
	return reinterpret_cast<T*>(&T::StaticClass()->Defaults(0));
}

// Safe pointer that never throws an exception. (written by Marco)
struct FSafePointer
{
protected:
	UObject* Object;
	INT ObjIndex;

public:
	FSafePointer()
	{}
	FSafePointer(UObject* Obj)
		: Object(Obj), ObjIndex(Obj ? (INT)Obj->GetIndex() : INDEX_NONE)
	{}

	inline UObject* Get()
	{
		return (Object && UObject::GetIndexedObject(ObjIndex) == Object) ? Object : NULL;
	}
	inline void Set(UObject* Obj)
	{
		Object = Obj;
		ObjIndex = (Obj ? (INT)Obj->GetIndex() : INDEX_NONE);
	}
	friend FArchive& operator<<(FArchive& Ar, FSafePointer& P)
	{
		if (Ar.IsLoading())
		{
			Ar << P.Object;
			P.ObjIndex = P.Object ? P.Object->GetIndex() : INDEX_NONE;
		}
		else
		{
			if (P.Object && UObject::GetIndexedObject(P.ObjIndex) != P.Object)
				P.Object = NULL;
			Ar << P.Object;
		}
		return Ar;
	}
};
template< class T > struct TSafePointer : protected FSafePointer
{
	TSafePointer()
		: FSafePointer()
	{}
	TSafePointer(T* Obj)
		: FSafePointer(Obj)
	{}

	inline T* Get()
	{
		return (Object && UObject::GetIndexedObject(ObjIndex) == Object) ? reinterpret_cast<T*>(Object) : nullptr;
	}
	inline void Set(T* Obj)
	{
		Object = Obj;
		ObjIndex = (Obj ? (INT)Obj->GetIndex() : INDEX_NONE);
	}

	friend FArchive& operator<<(FArchive& Ar, TSafePointer& P)
	{
		if (Ar.IsLoading())
		{
			Ar << P.Object;
			P.ObjIndex = P.Object ? P.Object->GetIndex() : INDEX_NONE;
		}
		else
		{
			if (P.Object && UObject::GetIndexedObject(P.ObjIndex) != P.Object)
				P.Object = NULL;
			Ar << P.Object;
		}
		return Ar;
	}
};

/*----------------------------------------------------------------------------
	Object iterators.
----------------------------------------------------------------------------*/

//
// Class for iterating through all objects.
//
class FObjectIterator
{
public:
	FObjectIterator( UClass* InClass=UObject::StaticClass() )
	:	Class( InClass ), Index( 0 )
	{
		check(Class);
		++*this;
	}
	void operator++()
	{
		while( ++Index<UObject::GObjObjects.Num() && (!UObject::GObjObjects(Index) || !UObject::GObjObjects(Index)->IsA(Class)) );
	}
	UObject* operator*()
	{
		return UObject::GObjObjects(Index);
	}
	UObject* operator->()
	{
		return UObject::GObjObjects(Index);
	}
	operator UBOOL()
	{
		return Index<UObject::GObjObjects.Num();
	}
protected:
	UClass* Class;
	INT Index;
};

//
// Class for iterating through all objects which inherit from a
// specified base class.
//
template< class T > class TObjectIterator : public FObjectIterator
{
public:
	TObjectIterator()
	:	FObjectIterator( T::StaticClass() )
	{}
	T* operator* ()
	{
		return (T*)FObjectIterator::operator*();
	}
	T* operator-> ()
	{
		return (T*)FObjectIterator::operator->();
	}
};

// Class casting flags, used for fast casting.
 enum EClassCastFlags
{
	CLCAST_None=0, // Uninitialized
	CLCAST_UProperty=(1<<0),
	CLCAST_UFunction=(1<<1),
	CLCAST_UStruct=(1<<2),
	CLCAST_UState=(1<<3),
	CLCAST_UClass=(1<<4),
	CLCAST_AActor=(1<<5),
	CLCAST_APawn=(1<<6),
	CLCAST_APlayerPawn=(1<<7),
	CLCAST_AProjectile=(1<<8),
	CLCAST_UObject=(1<<9),
	CLCAST_UFloatProperty = (1 << 10), // Having this specific thanks to input manager spamming it.
	CLCAST_UObjectProperty = (1 << 11),
};

#define AUTO_INITIALIZE_REGISTRANTS				\
	UObject::StaticClass();						\
	UClass::StaticClass();						\
	USubsystem::StaticClass();					\
	USystem::StaticClass();						\
	UProperty::StaticClass();					\
	UStructProperty::StaticClass();				\
	UField::StaticClass();						\
	UMapProperty::StaticClass();				\
	UArrayProperty::StaticClass();				\
	UFixedArrayProperty::StaticClass();			\
	UStrProperty::StaticClass();				\
	UNameProperty::StaticClass();				\
	UClassProperty::StaticClass();				\
	UObjectProperty::StaticClass();				\
	UFloatProperty::StaticClass();				\
	UBoolProperty::StaticClass();				\
	UIntProperty::StaticClass();				\
	UByteProperty::StaticClass();				\
	ULanguage::StaticClass();					\
	UTextBufferFactory::StaticClass();			\
	UFactory::StaticClass();					\
	UPackage::StaticClass();					\
	UCommandlet::StaticClass();					\
	ULinkerSave::StaticClass();					\
	ULinker::StaticClass();						\
	ULinkerLoad::StaticClass();					\
	UEnum::StaticClass();						\
	UTextBuffer::StaticClass();					\
	UPackageMap::StaticClass();					\
	UConst::StaticClass();						\
	UFunction::StaticClass();					\
	UStruct::StaticClass();						\
	UState::StaticClass();						\
	ULocale::StaticClass();						\
	UScriptHook::StaticClass();					\
	ULogHandler::StaticClass();					\
	UPointerProperty::StaticClass();			\
	UButtonProperty::StaticClass();				\
	UAnyProperty::StaticClass();				\
	UExporter::StaticClass();

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
