/*=============================================================================
	UnCorObj.h: Standard core object definitions.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

/*-----------------------------------------------------------------------------
	UPackage.
-----------------------------------------------------------------------------*/

//
// A package.
//
class CORE_API UPackage : public UObject
{
	DECLARE_CLASS(UPackage,UObject,0,Core)

	// Variables.
	void* DllHandle;
	DWORD PackageFlags;
	FString PackageURL;
	BITFIELD AttemptedBind : 1;		// Package has attempted DLL linkage.
	BITFIELD bIsDirty : 1;			// Package is modified, needs to be saved (for editor).
	BITFIELD bIsFullyLoaded : 1;	// Package is fully loaded, its safe to be saved (for editor).

	// Constructors.
	UPackage();
	UPackage(ENoInit); // is a transient package.

	// UObject interface.
	void Destroy();
	void Serialize( FArchive& Ar );

	// UPackage interface.
	void* GetDllExport( const TCHAR* ExportName, UBOOL Checked );

	// Obtain a package object for linker
	static UPackage* GetPackageLink(UObject* InOuter, const TCHAR* InName);
};

/*-----------------------------------------------------------------------------
	ULanguage.
-----------------------------------------------------------------------------*/

//
// A language (special case placeholder class).
//
class CORE_API ULanguage : public UObject
{
	DECLARE_ABSTRACT_CLASS(ULanguage,UObject,CLASS_Transient,Core)
	NO_DEFAULT_CONSTRUCTOR(ULanguage)
	ULanguage* SuperLanguage;
};

/*-----------------------------------------------------------------------------
	UTextBuffer.
-----------------------------------------------------------------------------*/

//
// An object that holds a bunch of text.  The text is contiguous and, if
// of nonzero length, is terminated by a NULL at the very last position.
//
class CORE_API UTextBuffer : public UObject, public FOutputDevice
{
	DECLARE_CLASS(UTextBuffer,UObject,0,Core)

	// Variables.
	INT Pos, Top;
	FString Text;

	// Constructors.
	UTextBuffer( const TCHAR* Str=TEXT("") );

	// UObject interface.
	void Serialize( FArchive& Ar );

	// FOutputDevice interface.
	void Serialize( const TCHAR* Data, EName Event );
};

/*----------------------------------------------------------------------------
	USystem.
----------------------------------------------------------------------------*/

class CORE_API USystem : public USubsystem
{
	DECLARE_CLASS(USystem,USubsystem,CLASS_Config,Core)

	// Variables.
#if 1 //LMode added by Legend on 4/12/2000
	//
	// a licensee configurable INI setting that can be used to augment
	// the behavior of LicenseeVer -- especially useful in supporting
	// the transition from "before LiceseeVer" to the new build; for
	// salvaging maps that have licensee-specific data.
	//
	INT LicenseeMode;
#endif
	INT PurgeCacheDays;
	INT UseCPU;						/* Force Unreal to use a specific core */
	INT MaxWarnPerFunc;
	FStringNoInit SavePath;
	FStringNoInit CachePath;
	FStringNoInit CacheExt;
	TArrayNoInit<FString> LangPaths;
	TArrayNoInit<FString> Paths;
	TArrayNoInit<FName> Suppress;
	UBOOL NoRunAway,NoCacheSearch,Force1msTimer,UseRegularAngles,CloseEmptyProperties,NoLogBuffering,CrashAutoRestart,AutoBackupCrashLogs;
	void* MainWindow;				// App main window HWND handle.

	// Constructors.
	void StaticConstructor();
	USystem();
	void Destroy();

	// FExec interface.
	UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar=*GLog );

	// OS specific
	static const TCHAR* GetOS();
	static void appRelaunch(const TCHAR* Cmd);
	static void DumpMemStat(FOutputDevice& Ar);
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
