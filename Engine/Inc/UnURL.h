/*=============================================================================
	UnURL.h: Unreal URL class.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

/*-----------------------------------------------------------------------------
	FURL.
-----------------------------------------------------------------------------*/

//
// A uniform resource locator.
//
class ENGINE_API FURL
{
public:
	// URL components.
	FString Protocol;	// Protocol, i.e. "unreal" or "http".
	FString Host;		// Optional hostname, i.e. "204.157.115.40" or "unreal.epicgames.com", blank if local.
	INT		Port;       // Optional host port.
	FString Map;		// Map name, i.e. "SkyCity", default is "Index".
	TArray<FString> Op;	// Options.
	FString Portal;		// Portal to enter through, default is "".

	// Status.
	// stijn: hack: when the URL is a valid ipv6 address, we set Valid to 3 (or 11 in binary). These URLs will still pass the validity test, but we also encode one additional bit of information
	DWORD	Valid;		// Whether parsed successfully.

	// Statics.
	static FString DefaultProtocol;
	static FString DefaultProtocolDescription;
	static FString DefaultName;
	static FString DefaultMap;
	static FString DefaultLocalMap;
	static FString AltLocalMap;
	static FString DefaultEntryMap;
	static FString DefaultHost;
	static FString DefaultPortal;
	static FString DefaultMapExt;
	static FString DefaultSaveExt;
	static INT DefaultPort;

	// Constructors.
	FURL( const TCHAR* Filename=NULL );
	FURL( FURL* Base, const TCHAR* TextURL, BYTE TravelType );
	static void StaticInit();
	static void StaticExit();

	// Functions.
	UBOOL IsInternal() const;
	UBOOL IsLocalInternal() const;
	UBOOL HasOption( const TCHAR* Test ) const;
	const TCHAR* GetOption( const TCHAR* Match, const TCHAR* Default ) const;
	void LoadURLConfig( const TCHAR* Section, const TCHAR* Filename=NULL );
	void SaveURLConfig( const TCHAR* Section, const TCHAR* Item, const TCHAR* Filename=NULL ) const;
	void AddOption( const TCHAR* Str );
	FString String( UBOOL FullyQualified=0 ) const;
	FString GetRemoteAddress() const;
	ENGINE_API friend FArchive& operator<<( FArchive& Ar, FURL& U );

	static bool IsIpv4Address(TCHAR*& Host, INT& PortNumber);
	static bool IsIpv6Address(TCHAR*& Host, INT& PortNumber);

	// Operators.
	UBOOL operator==( const FURL& Other ) const;
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
